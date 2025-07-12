#include "person.pb.h"
#include <string>

int main() {
  person::Person person;
  person.set_name("chulan");
  person.set_age(20);
  person.set_sex("man");

  std::string res_str;
  person.SerializeToString(&res_str);
  std::cout << res_str << '\n';

  person::Person person2;
  person2.ParseFromString(res_str);
  std::cout << person2.name() << ' ' << person2.age() << ' ' << person2.sex() << '\n';
}