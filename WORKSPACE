load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

git_repository(
    name = "gflags",
    tag = "v2.2.1",  # current as of 2018/6/30
    remote = "git://github.com/gflags/gflags.git",
)

new_git_repository(
    name = "glog",
    tag = "v0.3.5",
    remote = "git://github.com/google/glog.git",
    build_file = "third_party/glog.BUILD",
)
