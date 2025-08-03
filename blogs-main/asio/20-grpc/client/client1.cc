#include "demo.grpc.pb.h"
#include "demo.pb.h"

#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>
#include <memory>

class DemoClient {
public:
  DemoClient(std::shared_ptr<grpc::Channel> channel)
    : _stub(hello::HelloEndpoint::NewStub(channel)) {}

  std::string SayHello(const std::string& name) {
    hello::HelloRequest request;
    request.set_req(name);

    // 响应对象
    hello::HelloResponse response;

    // 创建上下文
    grpc::ClientContext context;

    // 调用远程方法
    grpc::Status status = _stub->SayHello(&context, request, &response);

    if (status.ok()) {
      return response.rsp();
    } else {
      std::cerr << "RPC failed: " << status.error_message() << std::endl;
      return "";
    }
  }

private:
  std::unique_ptr<hello::HelloEndpoint::Stub> _stub;
};

void RunClient() {
  // 创建 rpc 通道
  auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

  // 创建客户端
  DemoClient client(channel);

  std::string reply = client.SayHello("chulan");
  if (!reply.empty()) {
    std::cout << "Server replied: " << reply << std::endl;
  } else {
    std::cout << "Failed to get a valid response from server." << std::endl;
  }
}

int main() {
  RunClient();
}