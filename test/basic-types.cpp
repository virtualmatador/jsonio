#include <iostream>
#include <regex>
#include <spanstream>
#include <sstream>

#include <json.hpp>

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

bool t07() {
  auto text = "base64;VVVV";
  jsonio::json json;
  std::istringstream{text} >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_BINARY ||
      r != text || json.get_binary().size() != 3 ||
      json.get_binary()[0] != std::byte{'U'} ||
      json.get_binary()[1] != std::byte{'U'} ||
      json.get_binary()[2] != std::byte{'U'}) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t08() {
  auto text = "octet;3;UUU";
  jsonio::json json;
  std::istringstream{text} >> json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_BINARY ||
      r != "base64;VVVV" || json.get_binary().size() != 3 ||
      json.get_binary()[0] != std::byte{'U'} ||
      json.get_binary()[1] != std::byte{'U'} ||
      json.get_binary()[2] != std::byte{'U'}) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t09() {
  jsonio::json json;
  std::ispanstream{"stream;4;UUU"
                   "\255"
                   "\x00\x00\x00\x00"
                   "\x00\x00\x00\001"} >>
      json;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_BINARY ||
      r != "base64;VVVV" || json.get_binary().size() != 3 ||
      json.get_binary()[0] != std::byte{'U'} ||
      json.get_binary()[1] != std::byte{'U'} ||
      json.get_binary()[2] != std::byte{'U'}) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t10() {
  std::istringstream is{"UUU"};
  jsonio::json json = &is;
  auto r = (std::ostringstream{} << json).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_STREAM ||
      r != "base64;VVVV") {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t11() {
  std::istringstream is{"UUU"};
  jsonio::json json = &is;
  auto r = (std::ostringstream{} << json.format().bytes_as_binary()).str();
  if (!json.completed() || json.type() != jsonio::JsonType::J_STREAM ||
      std::memcmp(
          r.data(),
          "stream;16;UUU"
          "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
          "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0D",
          r.size()) != 0) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

int main() {
  if (t01() && t02() && t03() && t04() && t05() && t06() && t07() && t08() &&
      t09() && t10() && t11() && true) {
    return 0;
  }
  return -1;
}
