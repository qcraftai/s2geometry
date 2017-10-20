#ifndef S2_THIRD_PARTY_ABSL_STRINGS_STR_SPLIT_H_
#define S2_THIRD_PARTY_ABSL_STRINGS_STR_SPLIT_H_

#include <functional>
#include <string>
#include <vector>

#include "s2/third_party/absl/strings/string_view.h"
#include "s2/third_party/absl/strings/strip.h"

namespace strings {

template <typename String>
std::vector<String> Split(
    String const& text, char delim,
    std::function<bool(absl::string_view)> predicate);
template <typename String>
std::vector<String> Split(String const& text, char delim);

// Returns false if the given StringPiece is empty, indicating that the
// strings::Split() API should omit the empty string.
//
// std::vector<string> v = Split(" a , ,,b,", ',', SkipEmpty());
// EXPECT_THAT(v, ElementsAre(" a ", " ", "b"));
struct SkipEmpty {
  bool operator()(absl::string_view sv) const { return !sv.empty(); }
};

// Returns false if the given string is empty or contains only whitespace,
// indicating that the strings::Split() API should omit the string.
//
// std::vector<string> v = Split(" a , ,,b,", ',', SkipWhitespace());
// EXPECT_THAT(v, ElementsAre(" a ", "b"));
struct SkipWhitespace {
  bool operator()(absl::string_view sv) const {
    StripWhitespace(&sv);
    return !sv.empty();
  }
};

}  // namespace strings

#endif  // S2_THIRD_PARTY_ABSL_STRINGS_STR_SPLIT_H_