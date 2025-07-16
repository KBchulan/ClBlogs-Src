#include <ios>
#include <iostream>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <string>
#include <fstream>

int main() {
  // 从文件读取 JSON 数据
  std::ifstream file("../data.json", std::ios_base::in);
  if (!file.is_open()) {
    std::cout << "Error: Cannot open data.json file" << '\n';
    return 1;
  }

  // 解析 JSON 文件
  Json::Value root;
  Json::CharReaderBuilder builder;
  std::string errors;

  if (Json::parseFromStream(builder, file, &root, &errors)) {
    // 读取基本类型
    std::cout << "Name: " << root["name"].asString() << '\n';
    std::cout << "Age: " << root["age"].asInt() << '\n';
    std::cout << "Is student: " << root["is_student"].asBool() << '\n';

    // 读取数组
    const Json::Value &hobbies = root["hobbies"];
    std::cout << "Hobbies: ";
    for (const auto &hobby : hobbies) {
      std::cout << hobby.asString() << " ";
    }
    std::cout << '\n';

    // 读取嵌套对象
    const Json::Value &address = root["address"];
    std::cout << "Address: " << address["city"].asString() << ", "
              << address["country"].asString() << '\n';
  } else {
    std::cout << "Parse error: " << errors << '\n';
  }

  return 0;
}