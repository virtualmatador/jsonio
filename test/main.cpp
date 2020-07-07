#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01()
{
    auto text = "1234";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_LONG || os.str() != text)
    {
        std::cerr << "t01" << std::endl;
        return false;
    }
    return true;
}

bool t02()
{
    auto text = "\"My thoughts never end ...\"";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_STRING || os.str() != text)
    {
        std::cerr << "t02" << std::endl;
        return false;
    }
    return true;
}

bool t03()
{
    auto text = "true";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_BOOL || os.str() != text)
    {
        std::cerr << "t03" << std::endl;
        return false;
    }
    return true;
}

bool t04()
{
    auto text = "false";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_BOOL || os.str() != text)
    {
        std::cerr << "t04" << std::endl;
        return false;
    }
    return true;
}

bool t05()
{
    auto text = "3.1415926585";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_DOUBLE || os.str() != text)
    {
        std::cerr << "t05" << std::endl;
        return false;
    }
    return true;
}

bool t06()
{
    auto text = "[\"one\",\"two\",\"three\"]";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    auto arr = os.str();
    std::regex pattern{"\r|\n|\t"};
    std::string out;
    std::regex_replace(std::back_insert_iterator(out), arr.begin(), arr.end(), pattern, "");
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_ARRAY || out != text)
    {
        std::cerr << "t06" << std::endl;
        return false;
    }
    return true;
}

bool t07()
{
    auto text = "{\"key1\": \"value s\",\"key2\": 1363}";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    auto arr = os.str();
    std::regex pattern{"\r|\n|\t"};
    std::string out;
    std::regex_replace(std::back_insert_iterator(out), arr.begin(), arr.end(), pattern, "");
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_OBJECT || out != text)
    {
        std::cerr << "t07" << std::endl;
        return false;
    }
    return true;
}

bool t08()
{
    auto text = "{\"key1\": {\"name\": \"Bob\"},\"key2\": [2154734578,-1234567890],\"key3\": null}";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    std::ostringstream os;
    os << json;
    auto arr = os.str();
    std::regex pattern{"\r|\n|\t"};
    std::string out;
    std::regex_replace(std::back_insert_iterator(out), arr.begin(), arr.end(), pattern, "");
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_OBJECT || out != text)
    {
        std::cerr << out << "t08" << std::endl;
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
        t04() &&
        t05() &&
        t06() &&
        t07() &&
        t08() &&
        true)
    {
        return 0;
    }
    return -1;
}
