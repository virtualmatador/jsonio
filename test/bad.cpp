#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01() {
  auto text = "{]";
  jsonio::json json;
  std::istringstream{text} >> json;
  auto r = (std::ostringstream{} << json).str();
  if (json.completed() || json.type() != jsonio::JsonType::J_OBJECT ||
      r != "") {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t02() {
  auto text = "[1, 2,]";
  jsonio::json json;
  std::istringstream{text} >> json;
  auto r = (std::ostringstream{} << json).str();
  if (json.completed() || json.type() != jsonio::JsonType::J_ARRAY || r != "") {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

int main() {
  if (t01() && t02() && true) {
    return 0;
  }
  return -1;
}
