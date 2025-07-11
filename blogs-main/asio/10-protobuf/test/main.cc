#include "person.pb.h"
#include <iostream>

int main() {
    // 验证 protobuf 版本
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    try {
        person::data::Person person;
        std::cout << "Person 对象创建成功" << '\n';

        person.set_name("chulan");
        std::cout << "设置 name 成功" << '\n';

        person.set_id(55);
        std::cout << "设置 id 成功" << '\n';

        std::cout << "Name: " << person.name() << '\n';
        std::cout << "ID: " << person.id() << '\n';

        // 测试序列化
        std::string serialized_data;
        if (person.SerializeToString(&serialized_data)) {
            std::cout << "序列化成功，大小: " << serialized_data.size() << " 字节" << '\n';
        } else {
            std::cout << "序列化失败" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << '\n';
        return 1;
    }

    // 清理 protobuf 库
    google::protobuf::ShutdownProtobufLibrary();

    std::cout << "程序正常结束" << '\n';
    return 0;
}