#include "stream-filter/ratelimit_impl.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "envoy/config/core/v3/grpc_service.pb.h"
#include "envoy/stats/scope.h"

#include "common/common/assert.h"
#include "common/http/header_map_impl.h"
#include "common/http/headers.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace MGW {

GrpcClientImpl::GrpcClientImpl(Grpc::RawAsyncClientPtr&& async_client,
                            const absl::optional<std::chrono::milliseconds>& timeout,
                            envoy::config::core::v3::ApiVersion transport_api_version)
                            : async_client_(std::move(async_client)), timeout_(timeout),
                            service_method_(
                                getMethodDescriptor()),
                            transport_api_version_(transport_api_version) {}

GrpcClientImpl::~GrpcClientImpl() { ASSERT(!callbacks_); }

void GrpcClientImpl::cancel() {
  ASSERT(callbacks_ != nullptr);
  request_->cancel();
  callbacks_ = nullptr;
}

void GrpcClientImpl::createRequest(sample::RateLimitRequest& request,
                                   const std::string& domain
                                   ) {
  request.set_domain(domain);
}


void GrpcClientImpl::limit(RequestCallbacks& callbacks, const std::string& domain,
                           Tracing::Span& parent_span, const StreamInfo::StreamInfo& stream_info) {
  ASSERT(callbacks_ == nullptr);
  callbacks_ = &callbacks;

  sample::RateLimitRequest request;
  createRequest(request, domain);

  request_ =
      async_client_->send(service_method_, request, *this, parent_span,
                          Http::AsyncClient::RequestOptions().setTimeout(timeout_).setParentContext(
                              Http::AsyncClient::ParentContext{&stream_info}),
                          transport_api_version_);
}


void GrpcClientImpl::onSuccess(
    std::unique_ptr<sample::RateLimitResponse>&&,
    Tracing::Span&) {
    //ENVOY_LOG(trace, "response:{}", response);
    //ENVOY_LOG(trace, "span:{}", span);
  // LimitStatus status = LimitStatus::OK;
  // ASSERT(response->overall_code() != envoy::service::ratelimit::v3::RateLimitResponse::UNKNOWN);
  // if (response->overall_code() == envoy::service::ratelimit::v3::RateLimitResponse::OVER_LIMIT) {
  //   status = LimitStatus::OverLimit;
  //   span.setTag(Constants::get().TraceStatus, Constants::get().TraceOverLimit);
  // } else {
  //   span.setTag(Constants::get().TraceStatus, Constants::get().TraceOk);
  // }

  // Http::ResponseHeaderMapPtr response_headers_to_add;
  // Http::RequestHeaderMapPtr request_headers_to_add;
  // if (!response->response_headers_to_add().empty()) {
  //   response_headers_to_add = Http::ResponseHeaderMapImpl::create();
  //   for (const auto& h : response->response_headers_to_add()) {
  //     response_headers_to_add->addCopy(Http::LowerCaseString(h.key()), h.value());
  //   }
  // }

  // if (!response->request_headers_to_add().empty()) {
  //   request_headers_to_add = Http::RequestHeaderMapImpl::create();
  //   for (const auto& h : response->request_headers_to_add()) {
  //     request_headers_to_add->addCopy(Http::LowerCaseString(h.key()), h.value());
  //   }
  // }

  // DescriptorStatusListPtr descriptor_statuses = std::make_unique<DescriptorStatusList>(
  //     response->statuses().begin(), response->statuses().end());
  // callbacks_->complete(status, std::move(descriptor_statuses), std::move(response_headers_to_add),
  //                      std::move(request_headers_to_add));
  callbacks_ = nullptr;
}

void GrpcClientImpl::onFailure(Grpc::Status::GrpcStatus status, const std::string&,
                               Tracing::Span&) {
  //ASSERT(status != Grpc::Status::WellKnownGrpcStatus::Ok);
  ENVOY_LOG(trace, "status:{}", status);
  callbacks_->complete(LimitStatus::Error);
  callbacks_ = nullptr;
}

ClientPtr rateLimitClient(Server::Configuration::FactoryContext& context,
                          const envoy::config::core::v3::GrpcService& grpc_service,
                          const std::chrono::milliseconds timeout,
                          envoy::config::core::v3::ApiVersion transport_api_version) {
  // TODO(ramaraochavali): register client to singleton when GrpcClientImpl supports concurrent
  // requests.
  const auto async_client_factory =
      context.clusterManager().grpcAsyncClientManager().factoryForGrpcService(
          grpc_service, context.scope(), true);
  return std::make_unique<GrpcClientImpl>(
      async_client_factory->create(), timeout, transport_api_version);
}

const Protobuf::MethodDescriptor& GrpcClientImpl::getMethodDescriptor(){
  constexpr char V3[] = "sample.RateLimitService.ShouldRateLimit";
  const auto* descriptor = Protobuf::DescriptorPool::generated_pool()->FindMethodByName(V3);
  ASSERT(descriptor != nullptr);
  return *descriptor;
}


} // namespace MGW
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy