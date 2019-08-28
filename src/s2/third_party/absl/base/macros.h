//
// Copyright 2017 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -----------------------------------------------------------------------------
// File: macros.h
// -----------------------------------------------------------------------------
//
// This header file defines the set of language macros used within Abseil code.
// For the set of macros used to determine supported compilers and platforms,
// see s2_absl/base/config.h instead.
//
// This code is compiled directly on many platforms, including client
// platforms like Windows, Mac, and embedded systems.  Before making
// any changes here, make sure that you're not breaking any platforms.

#ifndef S2_THIRD_PARTY_ABSL_BASE_MACROS_H_
#define S2_THIRD_PARTY_ABSL_BASE_MACROS_H_

#include <cassert>
#include <cstddef>

#include "s2/third_party/absl/base/port.h"

// S2_ABSLARRAYSIZE()
//
// Returns the number of elements in an array as a compile-time constant, which
// can be used in defining new arrays. If you use this macro on a pointer by
// mistake, you will get a compile-time error.
#define S2_ABSLARRAYSIZE(array) \
  (sizeof(::s2_absl::macros_internal::ArraySizeHelper(array)))

namespace s2_absl {
namespace macros_internal {
// Note: this internal template function declaration is used by S2_ABSLARRAYSIZE.
// The function doesn't need a definition, as we only use its type.
template <typename T, size_t N>
auto ArraySizeHelper(const T (&array)[N]) -> char (&)[N];
}  // namespace macros_internal
}  // namespace s2_absl

// TODO(b/62370839): Replace arraysize() with S2_ABSLARRAYSIZE().
template <typename T, size_t N>
auto ArraySizeHelper(const T (&array)[N]) -> char (&)[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

// kLinkerInitialized
//
// An enum used only as a constructor argument to indicate that a variable has
// static storage duration, and that the constructor should do nothing to its
// state. Use of this macro indicates to the reader that it is legal to
// declare a static instance of the class, provided the constructor is given
// the s2_absl::base_internal::kLinkerInitialized argument.
//
// Normally, it is unsafe to declare a static variable that has a constructor or
// a destructor because invocation order is undefined. However, if the type can
// be zero-initialized (which the loader does for static variables) into a valid
// state and the type's destructor does not affect storage, then a constructor
// for static initialization can be declared.
//
// Example:
//       // Declaration
//       explicit MyClass(s2_absl::base_internal:LinkerInitialized x) {}
//
//       // Invocation
//       static MyClass my_global(s2_absl::base_internal::kLinkerInitialized);
namespace s2_absl {
namespace base_internal {
enum LinkerInitialized {
  kLinkerInitialized = 0,
  LINKER_INITIALIZED = 0,
};
}  // namespace base_internal
}  // namespace s2_absl

namespace base {
using s2_absl::base_internal::LinkerInitialized;
using s2_absl::base_internal::LINKER_INITIALIZED;
}  // namespace base

// S2_ABSLFALLTHROUGH_INTENDED
//
// Annotates implicit fall-through between switch labels, allowing a case to
// indicate intentional fallthrough and turn off warnings about any lack of a
// `break` statement. The S2_ABSLFALLTHROUGH_INTENDED macro should be followed by
// a semicolon and can be used in most places where `break` can, provided that
// no statements exist between it and the next switch label.
//
// Example:
//
//  switch (x) {
//    case 40:
//    case 41:
//      if (truth_is_out_there) {
//        ++x;
//        S2_ABSLFALLTHROUGH_INTENDED;  // Use instead of/along with annotations
//                                    // in comments
//      } else {
//        return x;
//      }
//    case 42:
//      ...
//
// Notes: when compiled with clang in C++11 mode, the S2_ABSLFALLTHROUGH_INTENDED
// macro is expanded to the [[clang::fallthrough]] attribute, which is analysed
// when  performing switch labels fall-through diagnostic
// (`-Wimplicit-fallthrough`). See clang documentation on language extensions
// for details:
// http://clang.llvm.org/docs/AttributeReference.html#fallthrough-clang-fallthrough
//
// When used with unsupported compilers, the S2_ABSLFALLTHROUGH_INTENDED macro
// has no effect on diagnostics. In any case this macro has no effect on runtime
// behavior and performance of code.
// TODO(b/62370839): Replace FALLTHROUGH_INTENDED with
// S2_ABSLFALLTHROUGH_INTENDED.
#if defined(__clang__) && defined(__has_warning)
#if __has_feature(cxx_attributes) && __has_warning("-Wimplicit-fallthrough")
#define FALLTHROUGH_INTENDED [[clang::fallthrough]]
#endif
#elif defined(__GNUC__) && __GNUC__ >= 7
#define FALLTHROUGH_INTENDED [[gnu::fallthrough]]
#endif

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif
#ifdef S2_ABSLFALLTHROUGH_INTENDED
#error "S2_ABSLFALLTHROUGH_INTENDED should not be defined."
#endif

// TODO(user): Use c++17 standard [[fallthrough]] macro, when supported.
#if defined(__clang__) && defined(__has_warning)
#if __has_feature(cxx_attributes) && __has_warning("-Wimplicit-fallthrough")
#define S2_ABSLFALLTHROUGH_INTENDED [[clang::fallthrough]]
#endif
#elif defined(__GNUC__) && __GNUC__ >= 7
#define S2_ABSLFALLTHROUGH_INTENDED [[gnu::fallthrough]]
#endif

#ifndef S2_ABSLFALLTHROUGH_INTENDED
#define S2_ABSLFALLTHROUGH_INTENDED \
  do {                            \
  } while (0)
#endif

// S2_ABSLDEPRECATED()
//
// Marks a deprecated class, struct, enum, function, method and variable
// declarations. The macro argument is used as a custom diagnostic message (e.g.
// suggestion of a better alternative).
//
// Example:
//
//   class S2_ABSLDEPRECATED("Use Bar instead") Foo {...};
//   S2_ABSLDEPRECATED("Use Baz instead") void Bar() {...}
//
// Every usage of a deprecated entity will trigger a warning when compiled with
// clang's `-Wdeprecated-declarations` option. This option is turned off by
// default, but the warnings will be reported by clang-tidy.
#if defined(__clang__) && __cplusplus >= 201103L
#define S2_ABSLDEPRECATED(message) __attribute__((deprecated(message)))
#endif

#ifndef S2_ABSLDEPRECATED
#define S2_ABSLDEPRECATED(message)
#endif

// S2_ABSLBAD_CALL_IF()
//
// Used on a function overload to trap bad calls: any call that matches the
// overload will cause a compile-time error. This macro uses a clang-specific
// "enable_if" attribute, as described at
// http://clang.llvm.org/docs/AttributeReference.html#enable-if
//
// Overloads which use this macro should be bracketed by
// `#ifdef S2_ABSLBAD_CALL_IF`.
//
// Example:
//
//   int isdigit(int c);
//   #ifdef S2_ABSLBAD_CALL_IF
//   int isdigit(int c)
//     S2_ABSLBAD_CALL_IF(c <= -1 || c > 255,
//                       "'c' must have the value of an unsigned char or EOF");
//   #endif // S2_ABSLBAD_CALL_IF

#if defined(__clang__)
# if __has_attribute(enable_if)
#  define S2_ABSLBAD_CALL_IF(expr, msg) \
    __attribute__((enable_if(expr, "Bad call trap"), unavailable(msg)))
# endif
#endif

// S2_ABSLASSERT()
//
// In C++11, `assert` can't be used portably within constexpr functions.
// S2_ABSLASSERT functions as a runtime assert but works in C++11 constexpr
// functions.  Example:
//
// constexpr double Divide(double a, double b) {
//   return S2_ABSLASSERT(b != 0), a / b;
// }
//
// This macro is inspired by
// https://akrzemi1.wordpress.com/2017/05/18/asserts-in-constexpr-functions/
#if defined(NDEBUG)
#define S2_ABSLASSERT(expr) (false ? (void)(expr) : (void)0)
#else
#define S2_ABSLASSERT(expr)              \
  (S2_ABSLPREDICT_TRUE((expr)) ? (void)0 \
                             : [] { assert(false && #expr); }())  // NOLINT
#endif

#ifdef S2_ABSLHAVE_EXCEPTIONS
#define S2_ABSLINTERNAL_TRY try
#define S2_ABSLINTERNAL_CATCH_ANY catch (...)
#define S2_ABSLINTERNAL_RETHROW do { throw; } while (false)
#else  // S2_ABSLHAVE_EXCEPTIONS
#define S2_ABSLINTERNAL_TRY if (true)
#define S2_ABSLINTERNAL_CATCH_ANY else if (false)
#define S2_ABSLINTERNAL_RETHROW do {} while (false)
#endif  // S2_ABSLHAVE_EXCEPTIONS

#endif  // S2_THIRD_PARTY_ABSL_BASE_MACROS_H_
