#include <iostream>
#include <regex>
#include <sstream>

#include <json.hpp>

bool t01() {
  auto text1 = "1234";
  jsonio::json json1;
  std::istringstream{text1} >> json1;
  jsonio::json json2;
  auto text2 = "5678";
  std::istringstream{text2} >> json2;
  json1.steal(json2, false);
  auto r1 = (std::ostringstream{} << json1).str();
  if (!json1.completed() || json1.type() != jsonio::JsonType::J_LONG ||
      r1 != text2) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  auto text3 = "9.101";
  jsonio::json json3;
  std::istringstream{text3} >> json3;
  json1.steal(json3, false);
  auto r2 = (std::ostringstream{} << json1).str();
  if (!json1.completed() || json1.type() != jsonio::JsonType::J_LONG ||
      r2 != text2) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t02() {
  auto text1 = R"({"a": true, "b": true})";
  jsonio::json json1;
  std::istringstream{text1} >> json1;
  jsonio::json json2;
  auto text2 = R"({"a": 0, "bb": false})";
  std::istringstream{text2} >> json2;
  json1.steal(json2, true);
  auto r = (std::ostringstream{} << json1.format().sort()).str();
  auto text3 = R"({"a":false,"b":true})";
  if (!json1.completed() || json1.type() != jsonio::JsonType::J_OBJECT ||
      r != text3) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t03() {
  auto text1 = R"({"a": true, "b": [true]})";
  jsonio::json json1;
  std::istringstream{text1} >> json1;
  jsonio::json json2;
  auto text2 = R"({"aa": false, "b": "1"})";
  std::istringstream{text2} >> json2;
  json1.steal(json2, true);
  auto r = (std::ostringstream{} << json1.format().sort()).str();
  auto text3 = R"({"a":true,"b":["1"]})";
  if (!json1.completed() || json1.type() != jsonio::JsonType::J_OBJECT ||
      r != text3) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t04() {
  auto text1 = R"({"a": true, "b": "abc"})";
  jsonio::json json1;
  std::istringstream{text1} >> json1;
  jsonio::json json2;
  auto text2 = R"({"aa": false, "b":[["1"]]})";
  std::istringstream{text2} >> json2;
  json1.steal(json2, true);
  auto r = (std::ostringstream{} << json1.format().sort()).str();
  auto text3 = R"({"a":true,"b":"1"})";
  if (!json1.completed() || json1.type() != jsonio::JsonType::J_OBJECT ||
      r != text3) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

int main() {
  if (t01() && t02() && t03() && t04() && true) {
    return 0;
  }
  return -1;
}
