#ifndef JSONIO_SRC_JSON_STRING_HPP
#define JSONIO_SRC_JSON_STRING_HPP

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
  json_string() noexcept : flags_{PHASE_START} {}

  json_string(const std::string &text) noexcept { *this = text; }

  json_string(std::string &&text) noexcept { *this = std::move(text); }

  json_string(const char *text) noexcept { *this = text; }

  json_string(std::string_view text) noexcept { *this = text; }

  json_string(const json_string &that) noexcept { *this = that; }

  json_string(json_string &&that) noexcept { *this = std::move(that); }

  json_string &operator=(const std::string &text) noexcept {
    flags_ = PHASE_COMPLETED;
    std::string::operator=(text);
    check_escape();
    return *this;
  }

  json_string &operator=(std::string &&text) noexcept {
    flags_ = PHASE_COMPLETED;
    std::string::operator=(std::move(text));
    check_escape();
    return *this;
  }

  json_string &operator=(const char *text) noexcept {
    flags_ = PHASE_COMPLETED;
    std::string::operator=(text);
    check_escape();
    return *this;
  }

  json_string &operator=(std::string_view text) noexcept {
    flags_ = PHASE_COMPLETED;
    std::string::operator=(text);
    check_escape();
    return *this;
  }

  json_string &operator=(const json_string &that) noexcept {
    if (this != &that) {
      std::string::operator=(*(std::string *)&that);
      flags_ = that.flags_;
    }
    return *this;
  }
  json_string &operator=(json_string &&that) noexcept {
    if (this != &that) {
      std::string::operator=(std::move(*(std::string *)&that));
      flags_ = that.flags_;
      that.flags_ = PHASE_START;
    }
    return *this;
  }
  ~json_string() noexcept {}

  void read(std::istream &is) {
    if ((flags_ & PHASE_COMPLETED) == PHASE_COMPLETED) {
      flags_ = PHASE_START;
    }
    if ((flags_ & PHASE_COMPLETED) == PHASE_START) {
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_TEXT;
      std::string::clear();
    }
    if ((flags_ & PHASE_COMPLETED) == PHASE_TEXT) {
      char source;
      while (is.get(source)) {
        if (flags_ & ESCAPING) {
          source = Unescape(source);
          if (source != '\0') {
            flags_ &= ~ESCAPING;
            flags_ |= ESCAPED;
            append(1, source);
          } else {
            is.setstate(std::ios::iostate::_S_badbit);
            break;
          }
        } else if (source != '\"') {
          if (source != '\\') {
            append(1, source);
          } else {
            flags_ |= ESCAPING;
          }
        } else {
          flags_ |= PHASE_COMPLETED;
          break;
        }
      }
    }
  }

  void write(std::ostream &os) const {
    if (completed()) {
      if (flags_ & ESCAPED) {
        for (const char source : *this) {
          if (!os.good()) {
            break;
          }
          char escaped_source = Escape(source);
          if (escaped_source == '\0') {
            os << source;
          } else {
            os << '\\';
            os << escaped_source;
          }
        }
      } else {
        os << *((std::string *)this);
      }
    }
  }

  bool completed() const {
    return (flags_ & PHASE_COMPLETED) == PHASE_COMPLETED;
  }

private:
  void check_escape() {
    for (auto source : *(std::string *)this) {
      if (Escape(source) != '\0') {
        flags_ |= ESCAPED;
        break;
      }
    }
  }

private:
  static char Unescape(const char source) {
    switch (source) {
    case 'b':
      return '\b';
      break;
    case 'f':
      return '\f';
      break;
    case 'n':
      return '\n';
      break;
    case 'r':
      return '\r';
      break;
    case 't':
      return '\t';
      break;
    case '\"':
      return '\"';
      break;
    case '\\':
      return '\\';
      break;
    }
    return '\0';
  }

  static char Escape(const char source) {
    switch (source) {
    case '\b':
      return 'b';
      break;
    case '\f':
      return 'f';
      break;
    case '\n':
      return 'n';
      break;
    case '\r':
      return 'r';
      break;
    case '\t':
      return 't';
      break;
    case '\"':
      return '\"';
      break;
    case '\\':
      return '\\';
      break;
    }
    return '\0';
  }
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

#endif // JSONIO_JSON_STREAM_HPP
