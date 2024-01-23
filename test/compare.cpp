#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01() {
  auto text1 = R"({"c":"d","a":"b"})", text2 = R"({"a":"b", "c":"d"})";
  jsonio::json json1, json2;
  std::istringstream{text1} >> json1;
  std::istringstream{text2} >> json2;
  if (!json1.completed() || !json2.completed() || json1 != json2) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

int main() {
  if (t01() && true) {
    return 0;
  }
  return -1;
}
