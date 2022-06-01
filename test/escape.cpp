#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01()
{
    auto text = R"({"a":"a\t1\nb\t2"})";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    auto arr = os.str();
    if (!json.completed() ||
        json.type() != jsonio::JsonType::J_OBJECT ||
        os.str() != text)
    {
        std::cerr << __FUNCTION__ << std::endl;
        return false;
    }
    return true;
}

bool t02()
{
    auto text = R"({"k":"a\nb"})";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    json["k"] = "a\nb";
    std::ostringstream os;
    os << json;
    if (!json.completed() ||
        json.type() != jsonio::JsonType::J_OBJECT ||
        os.str() != text)
    {
        std::cerr << __FUNCTION__ << std::endl;
        return false;
    }
    return true;
}

int main()
{
    if (
        t01() &&
        t02() &&
        true)
    {
        return 0;
    }
    return -1;
}
