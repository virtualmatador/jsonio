#ifndef JSONIO_SRC_JSON_STRING_H
#define JSONIO_SRC_JSON_STRING_H

#include <iostream>
#include <string>
#include <string_view>
#include <variant>

namespace jsonio {

class json_string : public std::string {
private:
  unsigned int flags_;

private:
  enum : unsigned int {
    PHASE_START = 0x0000,
    PHASE_TEXT = 0x0001,
    PHASE_COMPLETED = 0x0003,
    ESCAPING = 0x0004,
    ESCAPED = 0x0008,
  };

public:
  json_string() noexcept;
  json_string(const std::string &text) noexcept;
  json_string(std::string &&text) noexcept;
  json_string(const char *text) noexcept;
  json_string(std::string_view text) noexcept;
  json_string(const json_string &source) noexcept;
  json_string(json_string &&source) noexcept;
  json_string &operator=(const std::string &text) noexcept;
  json_string &operator=(std::string &&text) noexcept;
  json_string &operator=(const char *text) noexcept;
  json_string &operator=(std::string_view text) noexcept;
  json_string &operator=(const json_string &source) noexcept;
  json_string &operator=(json_string &&source) noexcept;
  ~json_string() noexcept;

  void read(std::istream &is);
  void write(std::ostream &os) const;
  bool completed() const;

private:
  void check_escape();

private:
  static char Unescape(const char source);
  static char Escape(const char source);
};

} // namespace jsonio

template <> class std::equal_to<jsonio::json_string> {
public:
  bool operator()(const jsonio::json_string &l,
                  const jsonio::json_string &r) const {
    return *static_cast<const std::string *>(&l) ==
           *static_cast<const std::string *>(&r);
  }
};

template <>
class std::hash<jsonio::json_string> : public std::hash<std::string> {
public:
  std::size_t operator()(const jsonio::json_string &key) const {
    return std::hash<std::string>::operator()(
        *static_cast<const std::string *>(&key));
  }
};

#endif // JSONIO_JSON_STREAM_H
