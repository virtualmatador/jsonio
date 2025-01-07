#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01() {
  jsonio::json json;
  std::istringstream{R"({"a":)"} >> json;
  std::istringstream{R"("b"})"} >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_OBJECT ||
      r != R"({"a":"b"})") {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t02() {
  jsonio::json json;
  std::istringstream{R"({"a": 12)"} >> json;
  std::istringstream{R"(34})"} >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_OBJECT ||
      r != R"({"a":1234})") {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t03() {
  jsonio::json json;
  std::istringstream{R"({"a": base64;VV)"} >> json;
  std::istringstream{R"(VV})"} >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_OBJECT ||
      r != R"({"a":base64;VVVV})") {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t04() {
  jsonio::json json;
  std::istringstream{R"([octet;)"} >> json;
  std::istringstream{R"(3)"} >> json;
  std::istringstream{R"(;)"} >> json;
  std::istringstream{R"(  )"} >> json;
  std::istringstream{R"( )"} >> json;
  std::istringstream{R"(])"} >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_ARRAY ||
      r != R"([base64;ICAg])") {
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
