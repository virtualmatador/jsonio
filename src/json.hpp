#ifndef JSONIO_SRC_JSON_H
#define JSONIO_SRC_JSON_H

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

#include "json_string.hpp"

#include "json_array.hpp"
#include "json_object.hpp"

namespace jsonio {

enum class JsonType : std::size_t {
  J_NULL,
  J_STRING,
  J_LONG,
  J_DOUBLE,
  J_BOOL,
  J_BINARY,
  J_STREAM,
  J_ARRAY,
  J_OBJECT,
};

class json;

using json_obj = json_object<json>;
using json_arr = json_array<json>;

using PARENT_TYPE =
    std::variant<std::monostate, json_string, long, double, bool,
                 std::vector<std::byte>, std::istream *, json_arr, json_obj>;

class json : public PARENT_TYPE {
private:
  unsigned int flags_;
  std::string buffer_;

  inline static const std::string_view null_{"null"};
  inline static const std::string_view true_{"true"};
  inline static const std::string_view false_{"false"};
  inline static const std::string_view base64_{"base64;"};
  inline static const std::string_view octet_{"octet;"};
  inline static const std::string_view stream_{"stream;"};
  inline static const std::string_view base64_chars_{
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/"};

public:
  class formatter {
  private:
    const json &object_;
    unsigned int flags_;

  private:
    formatter(const json &object) : object_{object}, flags_{0x00000420} {}
    void write(std::ostream &os) const { object_.write(os, false, 0, flags_); }
    static void indent(std::ostream &os, int indents, unsigned int flags) {
      os << '\n';
      for (int i = 0; i < indents * reinterpret_cast<char *>(&flags)[1]; ++i) {
        os << reinterpret_cast<char *>(&flags)[0];
      }
    }
    enum Format_Options : unsigned int {
      prettify_ = 0x00010000,
      new_line_bracket_ = 0x00020000,
      sort_asc_ = 0x00040000,
      sort_desc_ = 0x00080000,
      bytes_as_binary_ = 0x00100000,
    };

  public:
    formatter &prettify() {
      flags_ |= Format_Options::prettify_;
      return *this;
    }

    formatter &use_tabs() {
      reinterpret_cast<char *>(flags_)[0] = '\t';
      return *this;
    }

    formatter &separators(int count) {
      reinterpret_cast<char *>(flags_)[1] = static_cast<char>(count);
      return *this;
    }

    formatter &new_line_bracket() {
      flags_ |= Format_Options::new_line_bracket_;
      return *this;
    }

    formatter &sort(bool asc = true) {
      flags_ &= ~(Format_Options::sort_asc_ | Format_Options::sort_desc_);
      flags_ |= asc ? Format_Options::sort_asc_ : Format_Options::sort_desc_;
      return *this;
    }

    formatter &bytes_as_binary() {
      flags_ |= Format_Options::bytes_as_binary_;
      return *this;
    }

    formatter &reset() {
      flags_ = 0;
      return *this;
    }

  public:
    friend json;
    friend json_arr;
    friend json_obj;
    friend std::ostream &operator<<(std::ostream &os,
                                    const json::formatter &source) {
      source.write(os);
      return os;
    }
  };

private:
  enum : unsigned int {
    PHASE_START = 0x0000,
    PHASE_ARRAY = 0x0001,
    PHASE_OBJECT = 0x0002,
    PHASE_STRING = 0x0003,
    PHASE_NULL = 0x0004,
    PHASE_TRUE = 0x0005,
    PHASE_FALSE = 0x0006,
    PHASE_BASE64_LITERAl = 0x0007,
    PHASE_BASE64_DATA = 0x0008,
    PHASE_OCTET_LITERAL = 0x0009,
    PHASE_OCTET_SIZE = 0x000A,
    PHASE_OCTET_DATA = 0x000B,
    PHASE_STREAM_LITERAL = 0x000C,
    PHASE_STREAM_DIVIDER_SIZE = 0x000D,
    PHASE_STREAM_DATA = 0x000E,
    PHASE_INT = 0x000F,
    PHASE_FLOAT = 0x0010,
    PHASE_DELIMITER = 0x0011,
    PHASE_COMPLETED = 0x00FF,
    EMPTY_VALUE = 0x0100,
  };

public:
  json() noexcept : flags_{PHASE_START} {}

  json(const json &that) noexcept { *this = that; }

  json(json &&that) noexcept { *this = std::move(that); }

  json(const std::string &string_value) { *this = string_value; }

  json(std::string &&string_value) { *this = std::move(string_value); }

  json(const char *string_value) { *this = string_value; }

  json(std::string_view string_value) { *this = string_value; }

  json(const long &long_value) { *this = long_value; }

  json(const int &int_value) { *this = int_value; }

  json(const std::size_t &size_t_value) { *this = size_t_value; }

  json(const double &double_value) { *this = double_value; }

  json(const bool &bool_value) { *this = bool_value; }

  json(const std::vector<std::byte> &binary_value) { *this = binary_value; }

  json(std::vector<std::byte> &&binary_value) {
    *this = std::move(binary_value);
  }

  json(std::istream *is) { *this = is; }

  json(const json_arr &json_array_value) { *this = json_array_value; }

  json(json_arr &&json_array_value) { *this = std::move(json_array_value); }

  json(const json_obj &json_object_value) { *this = json_object_value; }

  json(json_obj &&json_object_value) { *this = std::move(json_object_value); }

  json &operator=(const json &that) noexcept {
    if (this != &that) {
      PARENT_TYPE::operator=(*(PARENT_TYPE *)&that);
      flags_ = that.flags_;
      buffer_ = that.buffer_;
    }
    return *this;
  }

  json &operator=(json &&that) noexcept {
    if (this != &that) {
      PARENT_TYPE::operator=(std::move(*(PARENT_TYPE *)&that));
      flags_ = that.flags_;
      that.flags_ = PHASE_START;
      buffer_ = std::move(that.buffer_);
      that.buffer_ = {};
    }
    return *this;
  }

  json &operator=(const std::string &string_value) {
    PARENT_TYPE::operator=(json_string{string_value});
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(std::string &&string_value) {
    PARENT_TYPE::operator=(json_string{std::move(string_value)});
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(const char *string_value) {
    PARENT_TYPE::operator=(json_string{string_value});
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(std::string_view string_value) {
    PARENT_TYPE::operator=(json_string{string_value});
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(const long &long_value) {
    PARENT_TYPE::operator=(long_value);
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(const int &int_value) {
    PARENT_TYPE::operator=(static_cast<long>(int_value));
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(const std::size_t &size_t_value) {
    PARENT_TYPE::operator=(static_cast<long>(size_t_value));
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(const double &double_value) {
    PARENT_TYPE::operator=(double_value);
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(const bool &bool_value) {
    PARENT_TYPE::operator=(bool_value);
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(const std::vector<std::byte> &binary_value) {
    PARENT_TYPE::operator=(binary_value);
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(std::vector<std::byte> &&binary_value) {
    PARENT_TYPE::operator=(std::move(binary_value));
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(std::istream *is) {
    PARENT_TYPE::operator=(is);
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(const json_arr &json_array_value) {
    PARENT_TYPE::operator=(json_array_value);
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(json_arr &&json_array_value) {
    PARENT_TYPE::operator=(std::move(json_array_value));
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(const json_obj &json_object_value) {
    PARENT_TYPE::operator=(json_object_value);
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  json &operator=(json_obj &&json_object_value) {
    PARENT_TYPE::operator=(std::move(json_object_value));
    flags_ = PHASE_COMPLETED;
    return *this;
  }

  void steal(const json &source, bool convert) {
    switch (type()) {
    case JsonType::J_NULL:
      switch (source.type()) {
      case JsonType::J_NULL:
        break;
      case JsonType::J_ARRAY:
        if (convert) {
          if (source.get_array().size() == 1) {
            steal(source.get_array()[0], convert);
          }
        }
        break;
      }
      break;
    case JsonType::J_STRING:
      switch (source.type()) {
      case JsonType::J_NULL:
        if (convert) {
          get_string() = "";
        }
        break;
      case JsonType::J_STRING:
        get_string() = source.get_string();
        break;
      case JsonType::J_LONG:
        if (convert) {
          get_string() = std::to_string(source.get_long());
        }
        break;
      case JsonType::J_DOUBLE:
        if (convert) {
          get_string() = std::to_string(source.get_double());
        }
        break;
      case JsonType::J_BOOL:
        if (convert) {
          get_string() = source.get_bool() ? "true" : "false";
        }
        break;
      case JsonType::J_ARRAY:
        if (convert) {
          if (source.get_array().size() == 1) {
            steal(source.get_array()[0], convert);
          }
        }
        break;
      }
      break;
    case JsonType::J_LONG:
      switch (source.type()) {
      case JsonType::J_STRING:
        if (convert) {
          char *progress;
          auto d = std::strtod(source.get_string().c_str(), &progress);
          if (*progress == *source.get_string().end()) {
            if (d == std::round(d)) {
              if (source.get_string() == std::to_string(d)) {
                get_long() = d;
              }
            }
          }
        }
        break;
      case JsonType::J_LONG:
        get_long() = source.get_long();
        break;
      case JsonType::J_DOUBLE:
        if (convert) {
          if (source.get_double() == std::round(source.get_double())) {
            get_long() = static_cast<long>(source.get_double());
          }
        }
        break;
      case JsonType::J_BOOL:
        if (convert) {
          get_long() = source.get_bool() ? 1 : 0;
        }
        break;
      case JsonType::J_ARRAY:
        if (convert) {
          if (source.get_array().size() == 1) {
            steal(source.get_array()[0], convert);
          }
        }
        break;
      }
      break;
    case JsonType::J_DOUBLE:
      switch (source.type()) {
      case JsonType::J_STRING:
        if (convert) {
          char *progress;
          auto d = std::strtod(source.get_string().c_str(), &progress);
          if (*progress == *source.get_string().end()) {
            if (source.get_string() == std::to_string(d)) {
              get_double() = d;
            }
          }
        }
        break;
      case JsonType::J_LONG:
        if (convert) {
          get_double() = source.get_long();
        }
        break;
      case JsonType::J_DOUBLE:
        get_double() = source.get_double();
        break;
      case JsonType::J_BOOL:
        if (convert) {
          get_double() = source.get_bool() ? 1.0 : 0.0;
        }
        break;
      case JsonType::J_ARRAY:
        if (convert) {
          if (source.get_array().size() == 1) {
            steal(source.get_array()[0], convert);
          }
        }
        break;
      }
      break;
    case JsonType::J_BOOL:
      switch (source.type()) {
      case JsonType::J_STRING:
        if (convert) {
          std::string l_source;
          std::transform(source.get_string().begin(), source.get_string().end(),
                         std::back_inserter(l_source),
                         [](auto c) { return std::tolower(c); });
          if (l_source == "0" || l_source == "false" || l_source == "no" ||
              l_source == "off") {
            get_bool() = false;
          } else if (l_source == "1" || l_source == "true" ||
                     l_source == "yes" || l_source == "on") {
            get_bool() = true;
          }
        }
        break;
      case JsonType::J_LONG:
        if (convert) {
          if (source.get_long() == 0) {
            get_bool() = false;
          } else if (source.get_long() == 1) {
            get_bool() = true;
          }
        }
        break;
      case JsonType::J_DOUBLE:
        if (convert) {
          if (source.get_double() == 0.0) {
            get_bool() = false;
          } else if (source.get_double() == 1.0) {
            get_bool() = true;
          }
        }
        break;
      case JsonType::J_BOOL:
        get_bool() = source.get_bool();
        break;
      case JsonType::J_ARRAY:
        if (convert) {
          if (source.get_array().size() == 1) {
            steal(source.get_array()[0], convert);
          }
        }
        break;
      }
      break;
    case JsonType::J_BINARY:
      switch (source.type()) {
      case JsonType::J_BINARY:
        get_binary() = source.get_binary();
        break;
      case JsonType::J_ARRAY:
        if (convert) {
          if (source.get_array().size() == 1) {
            steal(source.get_array()[0], convert);
          }
        }
        break;
      }
      break;
    case JsonType::J_STREAM:
      switch (source.type()) {
      case JsonType::J_STREAM:
        get_stream() = source.get_stream();
        break;
      case JsonType::J_ARRAY:
        if (convert) {
          if (source.get_array().size() == 1) {
            steal(source.get_array()[0], convert);
          }
        }
        break;
      }
      break;
    case JsonType::J_ARRAY:
      switch (source.type()) {
      case JsonType::J_NULL:
      case JsonType::J_STRING:
      case JsonType::J_LONG:
      case JsonType::J_DOUBLE:
      case JsonType::J_BOOL:
      case JsonType::J_BINARY:
      case JsonType::J_STREAM:
      case JsonType::J_OBJECT:
        if (convert) {
          get_array().clear();
          get_array().push_back(source);
        }
        break;
      case JsonType::J_ARRAY:
        get_array() = source.get_array();
        break;
      }
      break;
    case JsonType::J_OBJECT:
      switch (source.type()) {
      case JsonType::J_OBJECT:
        get_object().steal(source.get_object(), convert);
        break;
      case JsonType::J_ARRAY:
        if (convert) {
          if (source.get_array().size() == 1) {
            steal(source.get_array()[0], convert);
          }
        }
        break;
      }
      break;
    }
  }

  void clear() {
    flags_ = PHASE_START;
    PARENT_TYPE::operator=(PARENT_TYPE{});
  }

  bool operator==(const json &that) const {
    return *static_cast<const PARENT_TYPE *>(this) ==
           *static_cast<const PARENT_TYPE *>(&that);
  }

  bool completed() const {
    return (flags_ & PHASE_COMPLETED) == PHASE_COMPLETED;
  }

  JsonType type() const { return static_cast<JsonType>(this->index()); }

  json &operator[](std::size_t index) {
    return std::get<json_arr>(*this).operator[](index);
  }

  const json &operator[](std::size_t index) const {
    return std::get<json_arr>(*this).operator[](index);
  }

  json *at(std::size_t index) { return std::get<json_arr>(*this).at(index); }

  const json *at(std::size_t index) const {
    return std::get<json_arr>(*this).at(index);
  }

  json &operator[](const std::string &key) {
    return std::get<json_obj>(*this).operator[](key);
  }

  const json &operator[](const std::string &key) const {
    return std::get<json_obj>(*this).operator[](key);
  }

  json *at(const std::string &key) { return std::get<json_obj>(*this).at(key); }

  const json *at(const std::string &key) const {
    return std::get<json_obj>(*this).at(key);
  }

  json_obj::iterator find(const std::string &key) {
    return std::get<json_obj>(*this).find(key);
  }

  json_obj::const_iterator find(const std::string &key) const {
    return std::get<json_obj>(*this).find(key);
  }

  json_obj::iterator begin() { return std::get<json_obj>(*this).begin(); }

  json_obj::const_iterator begin() const {
    return std::get<json_obj>(*this).begin();
  }

  json_obj::iterator end() { return std::get<json_obj>(*this).end(); }

  json_obj::const_iterator end() const {
    return std::get<json_obj>(*this).end();
  }

  bool is_null() { return type() == JsonType::J_NULL; }

  std::string &get_string() {
    return const_cast<std::string &>(
        static_cast<const json &>(*this).get_string());
  }

  const std::string &get_string() const { return std::get<json_string>(*this); }

  long &get_long() {
    return const_cast<long &>(static_cast<const json &>(*this).get_long());
  }

  const long &get_long() const { return std::get<long>(*this); }

  double &get_double() {
    return const_cast<double &>(static_cast<const json &>(*this).get_double());
  }

  const double &get_double() const { return std::get<double>(*this); }

  bool &get_bool() {
    return const_cast<bool &>(static_cast<const json &>(*this).get_bool());
  }

  const bool &get_bool() const { return std::get<bool>(*this); }

  std::vector<std::byte> &get_binary() {
    return const_cast<std::vector<std::byte> &>(
        static_cast<const json &>(*this).get_binary());
  }

  const std::vector<std::byte> &get_binary() const {
    return std::get<std::vector<std::byte>>(*this);
  }

  std::istream *&get_stream() {
    return const_cast<std::istream *&>(
        static_cast<const json &>(*this).get_stream());
  }

  std::istream *const &get_stream() const {
    return std::get<std::istream *>(*this);
  }

  json_arr &get_array() {
    return const_cast<json_arr &>(static_cast<const json &>(*this).get_array());
  }

  const json_arr &get_array() const { return std::get<json_arr>(*this); }

  json_obj &get_object() {
    return const_cast<jsonio::json_obj &>(
        static_cast<const json &>(*this).get_object());
  }

  const json_obj &get_object() const { return std::get<json_obj>(*this); }

  int get_int() const { return static_cast<int>(std::get<long>(*this)); }

  int get_uint() const {
    return static_cast<unsigned int>(std::get<long>(*this));
  }

  float get_float() const {
    return static_cast<float>(std::get<double>(*this));
  }

  formatter format() const { return formatter(*this); }

private:
  std::size_t read(std::istream &is, const std::string &delimiters) {
    if ((flags_ & PHASE_COMPLETED) == PHASE_COMPLETED) {
      flags_ = PHASE_START;
    }
    if ((flags_ & PHASE_COMPLETED) == PHASE_START) {
      char source;
      while (is >> source) {
        if (std::isspace(source)) {
          continue;
        } else if (source == '{') {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_OBJECT;
          PARENT_TYPE::operator=(json_obj{json_obj::SKIP_PREFIX});
        } else if (source == '[') {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_ARRAY;
          PARENT_TYPE::operator=(json_arr{json_arr::SKIP_PREFIX});
        } else if (source == '\"') {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_STRING;
          PARENT_TYPE::operator=(json_string{});
        } else if (source == null_[0]) {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_NULL;
        } else if (source == true_[0]) {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_TRUE;
        } else if (source == false_[0]) {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_FALSE;
        } else if (source == base64_[0]) {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_BASE64_LITERAl;
        } else if (source == octet_[0]) {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_OCTET_LITERAL;
        } else if (source == stream_[0]) {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_STREAM_LITERAL;
        } else if (source == '.') {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_FLOAT;
          buffer_.append(1, source);
        } else if ((source >= '0' && source <= '9') || source == '+' ||
                   source == '-') {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_INT;
          buffer_.append(1, source);
        } else if (auto delimiter = delimiters.find(source);
                   delimiter != std::string::npos) {
          flags_ |= PHASE_COMPLETED;
          flags_ |= EMPTY_VALUE;
          return delimiter;
        } else {
          is.setstate(std::ios::iostate::_S_badbit);
          return std::string::npos;
        }
        break;
      }
    }
    switch (flags_ & PHASE_COMPLETED) {
    case PHASE_OBJECT:
      std::get<json_obj>(*this).read(is);
      if (std::get<json_obj>(*this).completed()) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_DELIMITER;
        return read_delimiter(is, delimiters);
      }
      break;
    case PHASE_ARRAY:
      std::get<json_arr>(*this).read(is);
      if (std::get<json_arr>(*this).completed()) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_DELIMITER;
        return read_delimiter(is, delimiters);
      }
      break;
    case PHASE_STRING:
      std::get<json_string>(*this).read(is);
      if (std::get<json_string>(*this).completed()) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_DELIMITER;
        return read_delimiter(is, delimiters);
      }
      break;
    case PHASE_NULL:
      if (read_literal(is, null_)) {
        PARENT_TYPE::operator=({});
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_DELIMITER;
        return read_delimiter(is, delimiters);
      }
      break;
    case PHASE_TRUE:
      if (read_literal(is, true_)) {
        PARENT_TYPE::operator=(true);
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_DELIMITER;
        return read_delimiter(is, delimiters);
      }
      break;
    case PHASE_FALSE:
      if (read_literal(is, false_)) {
        PARENT_TYPE::operator=(false);
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_DELIMITER;
        return read_delimiter(is, delimiters);
      }
      break;
    case PHASE_BASE64_LITERAl:
      if (read_literal(is, base64_)) {
        PARENT_TYPE::operator=(std::vector<std::byte>{});
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_BASE64_DATA;
        return read_base64_data(is, delimiters);
      }
      break;
    case PHASE_BASE64_DATA:
      return read_base64_data(is, delimiters);
      break;
    case PHASE_OCTET_LITERAL:
      if (read_literal(is, octet_)) {
        PARENT_TYPE::operator=(std::vector<std::byte>{});
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_OCTET_SIZE;
        return read_octet_size(is, delimiters);
      }
      break;
    case PHASE_OCTET_SIZE:
      return read_octet_size(is, delimiters);
      break;
    case PHASE_OCTET_DATA:
      return read_octet_data(is, delimiters);
      break;
    case PHASE_STREAM_LITERAL:
      if (read_literal(is, stream_)) {
        PARENT_TYPE::operator=(std::vector<std::byte>{});
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_STREAM_DIVIDER_SIZE;
        return read_stream_divider_size(is, delimiters);
      }
      break;
    case PHASE_STREAM_DIVIDER_SIZE:
      return read_stream_divider_size(is, delimiters);
      break;
    case PHASE_STREAM_DATA:
      return read_stream_data(is, delimiters, false);
      break;
    case PHASE_INT:
      return read_int(is, delimiters);
      break;
    case PHASE_FLOAT:
      return read_float(is, delimiters);
      break;
    case PHASE_DELIMITER:
      return read_delimiter(is, delimiters);
      break;
    }
    return std::string::npos;
  }

  bool read_literal(std::istream &is, std::string_view literal) {
    char source;
    while (is >> source) {
      if (source == literal[buffer_.size() + 1]) {
        if (buffer_.size() + 2 == literal.size()) {
          buffer_.clear();
          return true;
        } else {
          buffer_.append(1, source);
        }
      } else {
        is.setstate(std::ios::iostate::_S_badbit);
        break;
      }
    }
    return false;
  }

  std::size_t read_base64_data(std::istream &is,
                               const std::string &delimiters) {
    char source;
    while (is >> source) {
      if (std::isspace(source)) {
        continue;
      } else if (auto delimiter = delimiters.find(source);
                 delimiter != std::string::npos) {
        if (buffer_.size() == 0) {
          flags_ |= PHASE_COMPLETED;
        } else {
          is.setstate(std::ios::iostate::_S_badbit);
        }
        return delimiter;
      } else if (buffer_.append(1, source); buffer_.size() == 4) {
        if (buffer_[3] != '=') {
          get_binary().resize(get_binary().size() + 3);
          b64_decode_chunk(reinterpret_cast<const char (&)[4]>(*buffer_.data()),
                           reinterpret_cast<std::byte(&)[3]>(
                               get_binary().at(get_binary().size() - 3)));
          buffer_.clear();
        } else {
          if (buffer_[2] != '=') {
            buffer_[3] = 'A';
            std::byte dest[3];
            b64_decode_chunk(
                reinterpret_cast<const char (&)[4]>(*buffer_.data()), dest);
            std::copy(dest, dest + 2, std::back_insert_iterator(get_binary()));
          } else {
            buffer_[2] = buffer_[3] = 'A';
            std::byte dest[3];
            b64_decode_chunk(
                reinterpret_cast<const char (&)[4]>(*buffer_.data()), dest);
            std::copy(dest, dest + 1, std::back_insert_iterator(get_binary()));
          }
          buffer_.clear();
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_DELIMITER;
          return read_delimiter(is, delimiters);
        }
      }
    }
    if (delimiters == "\n") {
      if (buffer_.size() == 0) {
        flags_ |= PHASE_COMPLETED;
      } else {
        is.setstate(std::ios::iostate::_S_badbit);
      }
    }
    return std::string::npos;
  }

  std::size_t read_octet_size(std::istream &is, const std::string &delimiters) {
    char source;
    while (is >> source) {
      if (std::isspace(source)) {
        continue;
      } else if (source == ';') {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_OCTET_DATA;
        return read_octet_data(is, delimiters);
      } else {
        buffer_.append(1, source);
      }
    }
    return std::string::npos;
  }

  std::size_t read_octet_data(std::istream &is, const std::string &delimiters) {
    char *end_ptr;
    std::size_t size = strtol(buffer_.c_str(), &end_ptr, 0);
    if (*end_ptr == '\0') {
      get_binary().reserve(size);
      for (;;) {
        auto pre_size = get_binary().size();
        get_binary().resize(size);
        auto read_size = is.readsome(
            reinterpret_cast<char *>(get_binary().data() + pre_size),
            size - pre_size);
        get_binary().resize(pre_size + read_size);
        if (read_size == 0) {
          break;
        } else if (get_binary().size() == size) {
          buffer_.clear();
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_DELIMITER;
          return read_delimiter(is, delimiters);
        }
      }
    } else {
      is.setstate(std::ios::iostate::_S_badbit);
    }
    return std::string::npos;
  }

  std::size_t read_stream_divider_size(std::istream &is,
                                       const std::string &delimiters) {
    char source;
    while (is >> source) {
      if (std::isspace(source)) {
        continue;
      } else if (source == ';') {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_STREAM_DATA;
        return read_stream_data(is, delimiters, true);
      } else {
        buffer_.append(1, source);
      }
    }
    return std::string::npos;
  }

  std::size_t read_stream_data(std::istream &is, const std::string &delimiters,
                               bool grow) {
    char *end_ptr;
    std::size_t divider_size = strtol(buffer_.c_str(), &end_ptr, 0);
    if (*end_ptr == '\0') {
      if (grow) {
        get_binary().resize(get_binary().size() + divider_size);
        get_binary().back() = static_cast<std::byte>(divider_size);
      }
      bool zero = get_binary().size() >= 2 * divider_size &&
                  std::all_of(get_binary().end().base() - 2 * divider_size,
                              get_binary().end().base() - divider_size,
                              [](auto &d) { return d == std::byte{0x00}; });
      for (;;) {
        auto read_request = static_cast<std::size_t>(get_binary().back());
        auto read_response = is.readsome(
            reinterpret_cast<char *>(get_binary().end().base()) - read_request,
            read_request);
        if (read_response == 0) {
          break;
        } else if (read_response < read_request) {
          get_binary().back() =
              static_cast<std::byte>(read_request - read_response);
        } else {
          if (zero) {
            if (get_binary().back() == std::byte{0x00}) {
              zero = false;
            } else {
              get_binary().resize(
                  get_binary().size() - 2 * divider_size -
                  static_cast<std::size_t>(get_binary().back()));
              flags_ &= ~PHASE_COMPLETED;
              flags_ |= PHASE_DELIMITER;
              return read_delimiter(is, delimiters);
            }
          } else {
            zero = std::all_of(get_binary().end().base() - divider_size,
                               get_binary().end().base(),
                               [](auto &d) { return d == std::byte{0x00}; });
            get_binary().resize(get_binary().size() + divider_size);
          }
          get_binary().back() = static_cast<std::byte>(divider_size);
        }
      }
    } else {
      is.setstate(std::ios::iostate::_S_badbit);
    }
    return std::string::npos;
  }

  std::size_t read_int(std::istream &is, const std::string &delimiters) {
    char source;
    while (is >> source) {
      if (std::isspace(source)) {
        continue;
      } else if (auto delimiter = delimiters.find(source);
                 delimiter != std::string::npos) {
        convert_int(is);
        return delimiter;
      } else if (buffer_.append(1, source); buffer_.back() == '.') {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_FLOAT;
        return read_float(is, delimiters);
      }
    }
    if (delimiters == "\n") {
      convert_int(is);
    }
    return std::string::npos;
  }

  std::size_t read_float(std::istream &is, const std::string &delimiters) {
    char source;
    while (is >> source) {
      if (std::isspace(source)) {
        continue;
      } else if (auto delimiter = delimiters.find(source);
                 delimiter != std::string::npos) {
        convert_float(is);
        return delimiter;
      } else {
        buffer_.append(1, source);
      }
    }
    if (delimiters == "\n") {
      convert_float(is);
    }
    return std::string::npos;
  }

  std::size_t read_delimiter(std::istream &is, const std::string &delimiters) {
    if (delimiters == "\n") {
      flags_ |= PHASE_COMPLETED;
    } else {
      char source;
      while (is >> source) {
        if (std::isspace(source)) {
          continue;
        } else if (auto delimiter = delimiters.find(source);
                   delimiter != std::string::npos) {
          flags_ |= PHASE_COMPLETED;
          return delimiter;
        } else {
          is.setstate(std::ios::iostate::_S_badbit);
          break;
        }
      }
    }
    return std::string::npos;
  }

  void convert_int(std::istream &is) {
    char *end_ptr;
    PARENT_TYPE::operator=(strtol(buffer_.c_str(), &end_ptr, 0));
    if (*end_ptr == '\0') {
      buffer_.clear();
      flags_ |= PHASE_COMPLETED;
    } else {
      is.setstate(std::ios::iostate::_S_badbit);
    }
  }

  void convert_float(std::istream &is) {
    char *end_ptr;
    PARENT_TYPE::operator=(strtod(buffer_.c_str(), &end_ptr));
    if (*end_ptr == '\0') {
      buffer_.clear();
      flags_ |= PHASE_COMPLETED;
    } else {
      is.setstate(std::ios::iostate::_S_badbit);
    }
  }

  void write(std::ostream &os, bool separate, int indents,
             unsigned int flags = 0) const {
    if (completed()) {
      switch (type()) {
      case JsonType::J_NULL:
        if (flags & formatter::Format_Options::prettify_) {
          if (separate) {
            os << " ";
          }
        }
        os << null_;
        break;
      case JsonType::J_STRING:
        if (flags & formatter::Format_Options::prettify_) {
          if (separate) {
            os << " ";
          }
        }
        os << '\"';
        std::get<json_string>(*this).write(os);
        os << '\"';
        break;
      case JsonType::J_LONG:
        if (flags & formatter::Format_Options::prettify_) {
          if (separate) {
            os << " ";
          }
        }
        os << get_long();
        break;
      case JsonType::J_DOUBLE:
        if (flags & formatter::Format_Options::prettify_) {
          if (separate) {
            os << " ";
          }
        }
        os << std::setprecision(std::numeric_limits<double>::max_digits10)
           << get_double();
        break;
      case JsonType::J_BOOL:
        if (flags & formatter::Format_Options::prettify_) {
          if (separate) {
            os << " ";
          }
        }
        os << (get_bool() ? true_ : false_);
        break;
      case JsonType::J_BINARY:
        if (flags & formatter::Format_Options::prettify_) {
          if (separate) {
            os << " ";
          }
        }
        if ((flags & formatter::Format_Options::bytes_as_binary_)) {
          os << octet_ << get_binary().size() << ';';
          os.write(reinterpret_cast<const char *>(get_binary().data()),
                   get_binary().size());
        } else {
          os << base64_;
          std::size_t i = 0;
          char chunk[4];
          for (; i + 2 < get_binary().size(); i += 3) {
            b64_encode_chunk(reinterpret_cast<const std::byte(&)[3]>(
                                 *(get_binary().data() + i)),
                             chunk);
            os.write(chunk, sizeof(chunk));
          }
          if (i == get_binary().size() - 1) {
            std::byte source[3]{get_binary().data()[i], std::byte{0},
                                std::byte{0}};
            b64_encode_chunk(source, chunk);
            chunk[2] = chunk[3] = '=';
            os.write(chunk, sizeof(chunk));
          } else if (i == get_binary().size() - 2) {
            std::byte source[3]{get_binary().data()[i],
                                get_binary().data()[i + 1], std::byte{0}};
            b64_encode_chunk(source, chunk);
            chunk[3] = '=';
            os.write(chunk, sizeof(chunk));
          }
        }
        break;
      case JsonType::J_STREAM:
        if (flags & formatter::Format_Options::prettify_) {
          if (separate) {
            os << " ";
          }
        }
        if ((flags & formatter::Format_Options::bytes_as_binary_)) {
          std::byte data[16];
          os << stream_ << sizeof(data) << ';';
          std::size_t read_size = 0;
          while ((read_size =
                      get_stream()->readsome(reinterpret_cast<char *>(&data),
                                             sizeof(data))) == sizeof(data)) {
            os.write(reinterpret_cast<char *>(&data), sizeof(data));
            if (std::all_of(std::begin(data), std::end(data),
                            [](auto &d) { return d == std::byte{0x00}; })) {
              os.write(reinterpret_cast<char *>(&data), sizeof(data));
            }
          }
          std::fill(std::begin(data) + read_size,
                    std::begin(data) + sizeof(data), std::byte{0xff});
          os.write(reinterpret_cast<char *>(&data), sizeof(data));
          std::fill(std::begin(data), std::end(data), std::byte{0x00});
          os.write(reinterpret_cast<char *>(&data), sizeof(data));
          data[sizeof(data) - 1] =
              static_cast<std::byte>(sizeof(data) - read_size);
          os.write(reinterpret_cast<char *>(&data), sizeof(data));
        } else {
          os << base64_;
          std::byte data[3];
          char chunk[4];
          std::size_t read_size = 0;
          while ((read_size =
                      get_stream()->readsome(reinterpret_cast<char *>(data),
                                             sizeof(data))) == sizeof(data)) {
            b64_encode_chunk(data, chunk);
            os.write(chunk, sizeof(chunk));
          }
          if (read_size == 1) {
            data[1] = data[2] = std::byte{0};
            b64_encode_chunk(data, chunk);
            chunk[2] = chunk[3] = '=';
            os.write(chunk, sizeof(chunk));
          } else if (read_size == 2) {
            data[2] = std::byte{0};
            b64_encode_chunk(data, chunk);
            chunk[3] = '=';
            os.write(chunk, sizeof(chunk));
          }
        }
        break;
      case JsonType::J_ARRAY:
        if (flags & formatter::Format_Options::prettify_) {
          if (separate) {
            if (flags & formatter::Format_Options::new_line_bracket_) {
              json::formatter::indent(os, indents, flags);
            } else {
              os << " ";
            }
          }
        }
        std::get<json_arr>(*this).write(os, indents + 1, flags);
        break;
      case JsonType::J_OBJECT:
        if (flags & formatter::Format_Options::prettify_) {
          if (separate) {
            if (flags & formatter::Format_Options::new_line_bracket_) {
              json::formatter::indent(os, indents, flags);
            } else {
              os << " ";
            }
          }
        }
        std::get<json_obj>(*this).write(os, indents + 1, flags);
        break;
      }
    }
  }

  void b64_encode_chunk(const std::byte (&source)[3], char (&dest)[4]) const {
    dest[0] = base64_chars_[static_cast<std::size_t>(source[0]) / 4];
    dest[1] = base64_chars_[(static_cast<std::size_t>(source[0]) % 4) * 16 +
                            static_cast<std::size_t>(source[1]) / 16];
    dest[2] = base64_chars_[(static_cast<std::size_t>(source[1]) % 16) * 4 +
                            static_cast<std::size_t>(source[2]) / 64];
    dest[3] = base64_chars_[static_cast<std::size_t>(source[2]) % 64];
  }

  void b64_decode_chunk(const char (&source)[4], std::byte (&dest)[3]) const {
    dest[0] = static_cast<std::byte>(base64_chars_.find(source[0]) * 4 +
                                     base64_chars_.find(source[1]) / 16);
    dest[1] = static_cast<std::byte>((base64_chars_.find(source[1]) % 16) * 16 +
                                     base64_chars_.find(source[2]) / 4);
    dest[2] = static_cast<std::byte>((base64_chars_.find(source[2]) % 4) * 64 +
                                     base64_chars_.find(source[3]));
  }

public:
  friend class json_array<json>;
  friend class json_object<json>;

  friend std::istream &operator>>(std::istream &is, json &target) {
    target.read(is, "\n");
    return is;
  }

  friend std::ostream &operator<<(std::ostream &os, const json &source) {
    source.write(os, false, 0);
    return os;
  }
};

} // namespace jsonio

#endif // JSONIO_SRC_JSON_H
