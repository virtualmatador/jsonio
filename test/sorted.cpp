#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01() {
  auto text = R"({"2":2,"1":1,"3":3})";
  jsonio::json obj;
  std::istringstream{text} >> obj;
  auto r = (std::ostringstream{} << obj.format().sort()).str();
  if (r != R"({"1":1,"2":2,"3":3})") {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t02() {
  auto text = R"({"2": 2, "1": 1, "3": 3})";
  jsonio::json obj;
  std::istringstream{text} >> obj;
  auto r = (std::ostringstream{} << obj.format().sort(false)).str();
  if (r != R"({"3":3,"2":2,"1":1})") {
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
