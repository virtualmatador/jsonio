#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01()
{
    auto text = "[\"one\",\"two\",\"three\"]";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_ARRAY ||
        os.str() != text)
    {
        std::cerr << __FUNCTION__ << std::endl;
        return false;
    }
    return true;
}

bool t02()
{
    auto text = "{\"key1\":\"value s\",\"key2\":1363}";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    auto arr = os.str();
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_OBJECT ||
        os.str() != text)
    {
        std::cerr << __FUNCTION__ << std::endl;
        return false;
    }
    return true;
}

bool t03()
{
    auto text = "{\"key1\":{\"name\":\"Bob\"},"
        "\"key2\":[2154734578,-1234567890],\"key3\":null}";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_OBJECT ||
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
        t03() &&
        true)
    {
        return 0;
    }
    return -1;
}
