#include "demo.grpc.pb.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>
#include <grpcpp/security/server_credentials.h>

#include <print>
#include <memory>

class DemoServiceImpl final : public hello::HelloEndpoint::Service {
public:
  grpc::Status SayHello(grpc::ServerContext* context, const hello::HelloRequest* request, hello::HelloResponse* response) {
    response->set_rsp("Server1 response: " + request->req());
    return grpc::Status::OK;
  }
};

void startServer() {
  std::string server_address("0.0.0.0:50051");

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

  DemoServiceImpl service;
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::print("Server listening on {}\n", server_address);
  server->Wait();
}

int main() {
  startServer();
}