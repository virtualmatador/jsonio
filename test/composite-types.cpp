#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01() {
  auto text = "[\"one\",\"two\",\"three\",base64;ICA=]";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_ARRAY ||
      r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t02() {
  auto text = "{\"key\":\"value = ' - $ # ! @ ^ % & * ( ) [ ] { } '\"}";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_OBJECT ||
      r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t03() {
  auto text = "{\"key1\":{\"name\":\"Bob\"},"
              "\"key2\":[2154734578,-1234567890],\"key3\":null}";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json.format().sort()).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_OBJECT ||
      r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t04() {
  auto text = "{\"key1\":[]}";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_OBJECT ||
      r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t05() {
  auto text = "[]";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_ARRAY ||
      r != text) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

int main() {
  if (t01() && t02() && t03() && t04() && t05() && true) {
    return 0;
  }
  return -1;
}
