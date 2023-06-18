#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01()
{
    auto text = R"({"2":2,"1":1,"3":3})";
    jsonio::json_obj obj;
    std::istringstream{text} >> obj;
    std::ostringstream os;
    os << std::skipws << obj;
    if (os.str() != text)
    {
        std::cerr << __FUNCTION__ << std::endl;
        return false;
    }
    return true;
}

bool t02()
{
    auto text = R"({"2": 2, "1": 1, "3": 3})";
    jsonio::json_obj obj;
    std::istringstream{text} >> obj;
    std::ostringstream os;
    os << std::skipws << std::right << obj;
    if (os.str() != R"({"1":1,"2":2,"3":3})")
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
