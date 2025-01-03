#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01() {
  auto text = "1234";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_LONG ||
      r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t02() {
  auto text = "\"My thoughts never end ...\"";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_STRING ||
      r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t03() {
  auto text = "true";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_BOOL ||
      r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t04() {
  auto text = "false";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_BOOL ||
      r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t05() {
  auto text = "null";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || !json.is_null() ||
      json.type() != jsonio::JsonType::J_NULL || r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t06() {
  auto text = "3.1415926585";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_DOUBLE ||
      r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

int main() {
  if (t01() && t02() && t03() && t04() && t05() && t06() && true) {
    return 0;
  }
  return -1;
}
