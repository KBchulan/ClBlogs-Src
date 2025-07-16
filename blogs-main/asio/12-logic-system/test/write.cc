#include <iostream>
#include <json/json.h>
#include <string>

int main() {
  // 创建 JSON 对象
  Json::Value root;
  root["name"] = "chulan";
  root["age"] = 20;
  root["is_student"] = true;

  // 创建数组
  Json::Value hobbies(Json::arrayValue);
  hobbies.append("coding");
  hobbies.append("reading");
  root["hobbies"] = hobbies;

  // 创建嵌套对象
  Json::Value address;
  address["city"] = "Beijing";
  address["country"] = "China";
  root["address"] = address;

  // 序列化操作
  // 可以简单使用
  std::string json_str2 = root.toStyledString();
  std::cout << json_str2 << '\n';

  std::cout << "--------------------------------\n";

  // Writer性能更高且可以支持自定义
  Json::StreamWriterBuilder builder;
  builder["indentation"] = "  ";          // 设置缩进为2个空格
  builder["precision"] = 6;               // 设置浮点数精度
  builder["dropNullPlaceholders"] = true; // 丢弃null值
  std::string json_str = Json::writeString(builder, root);
  std::cout << json_str << '\n';

  return 0;
}