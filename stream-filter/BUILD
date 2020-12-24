load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_binary",
    "envoy_cc_library",
)
load("@envoy_api//bazel:api_build_system.bzl", "api_proto_package")

package(default_visibility = ["//visibility:public"])

envoy_cc_binary(
    name = "envoy",
    repository = "@envoy",
    deps = [
        ":stream_filter_config",
        "@envoy//source/exe:envoy_main_entry_lib",
    ],
)

api_proto_package()

envoy_cc_library(
    name = "stream_filter_lib",
    srcs = ["stream_filter.cc"],
    hdrs = ["stream_filter.h"],
    repository = "@envoy",
    deps = [
        ":pkg_cc_proto",
        "@envoy//include/envoy/http:codes_interface",
        "@envoy//include/envoy/ratelimit:ratelimit_interface",
        "@envoy//source/common/common:assert_lib",
        "@envoy//source/common/http:codes_lib",
        ],
)

envoy_cc_library(
    name = "stream_filter_config",
    srcs = ["stream_filter_config.cc"],
    hdrs = ["stream_filter_config.h"],
    repository = "@envoy",
    deps = [
        ":stream_filter_lib",
        "@envoy//include/envoy/server:filter_config_interface",
        "@envoy//source/extensions/filters/http/common:factory_base_lib",
        "@envoy//source/extensions/filters/http:well_known_names",
    ],
)

# envoy_cc_test(
#     name = "http_filter_integration_test",
#     srcs = ["http_filter_integration_test.cc"],
#     repository = "@envoy",
#     deps = [
#         ":http_filter_config",
#         "@envoy//test/integration:http_integration_lib",
#     ],
# )
