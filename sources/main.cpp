#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "echo.grpc.pb.h"
#include "echo.pb.h"

#include "suggestService.grpc.pb.h"
#include "suggestService.pb.h"

using grpc::ClientContext;
using grpc::Status;
using suggestService::SuggestRequest;
using suggestService::Suggest;
using suggestService::SuggestResponse;
using suggestService::SuggestService;

int main(int argc, char **argv) {
    if (argc == 1)
        throw std::invalid_argument("empty request");

    auto channel = grpc::CreateChannel("0.0.0.0:9090", grpc::InsecureChannelCredentials());
    auto client = suggestService::NewStub(channel);
    SuggestResponse response;
    ClientContext context;
    SuggestRequest request;
    request.set_data(argv[1]);
    Status status = client->Dialog(&context, request, &response);

    if (status.ok()) {
        json result;
        result["suggestions"] = {};
        for (const auto &suggest : response.suggestions()) {
            nlohmann::json element;
            element["text"] = suggest.text();
            element["position"] = suggest.position();
            result["suggestions"].push_back(element);
        }
        std::cout << std::setw(2) << result << std::endl;
    } else {
        return -1;
    }
}
