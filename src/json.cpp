#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <memory>

#include "json.h"

jsonio::json::json() noexcept : flags_{PHASE_START} {}

jsonio::json::json(const jsonio::json &source) noexcept { *this = source; }

jsonio::json::json(jsonio::json &&source) noexcept {
  *this = std::move(source);
}

jsonio::json::json(const void *null_value) { *this = null_value; }

jsonio::json::json(const std::string &string_value) { *this = string_value; }

jsonio::json::json(std::string &&string_value) {
  *this = std::move(string_value);
}

jsonio::json::json(const char *string_value) { *this = string_value; }

jsonio::json::json(const long &long_value) { *this = long_value; }

jsonio::json::json(const int &int_value) { *this = int_value; }

jsonio::json::json(const std::size_t &size_t_value) { *this = size_t_value; }

jsonio::json::json(const double &double_value) { *this = double_value; }

jsonio::json::json(const bool &bool_value) { *this = bool_value; }

jsonio::json::json(const json_obj &json_object_value) {
  *this = json_object_value;
}

jsonio::json::json(json_obj &&json_object_value) {
  *this = std::move(json_object_value);
}

jsonio::json::json(const json_arr &json_array_value) {
  *this = json_array_value;
}

jsonio::json::json(json_arr &&json_array_value) {
  *this = std::move(json_array_value);
}

jsonio::json &jsonio::json::operator=(const jsonio::json &source) noexcept {
  if (this != &source) {
    PARENT_TYPE::operator=(*(PARENT_TYPE *)&source);
    flags_ = source.flags_;
    binary_ = source.binary_;
  }
  return *this;
}

jsonio::json &jsonio::json::operator=(jsonio::json &&source) noexcept {
  if (this != &source) {
    PARENT_TYPE::operator=(std::move(*(PARENT_TYPE *)&source));
    flags_ = source.flags_;
    source.flags_ = PHASE_START;
    binary_ = std::move(source.binary_);
    source.binary_.clear();
  }
  return *this;
}

jsonio::json &jsonio::json::operator=(const void *null_value) {
  PARENT_TYPE::operator=(PARENT_TYPE{});
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const std::string &string_value) {
  PARENT_TYPE::operator=(json_string(string_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(std::string &&string_value) {
  PARENT_TYPE::operator=(json_string(std::move(string_value)));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const char *string_value) {
  PARENT_TYPE::operator=(json_string(string_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const long &long_value) {
  PARENT_TYPE::operator=(long_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const int &int_value) {
  PARENT_TYPE::operator=(static_cast<long>(int_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const std::size_t &size_t_value) {
  PARENT_TYPE::operator=(static_cast<long>(size_t_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const double &double_value) {
  PARENT_TYPE::operator=(double_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const bool &bool_value) {
  PARENT_TYPE::operator=(bool_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &
jsonio::json::operator=(const jsonio::json_obj &json_object_value) {
  PARENT_TYPE::operator=(json_object_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(jsonio::json_obj &&json_object_value) {
  PARENT_TYPE::operator=(std::move(json_object_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &
jsonio::json::operator=(const jsonio::json_arr &json_array_value) {
  PARENT_TYPE::operator=(json_array_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(jsonio::json_arr &&json_array_value) {
  PARENT_TYPE::operator=(std::move(json_array_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json::~json() noexcept {}

void jsonio::json::steal(const json &source, bool convert) {
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
        } else if (l_source == "1" || l_source == "true" || l_source == "yes" ||
                   l_source == "on") {
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
  case JsonType::J_ARRAY:
    switch (source.type()) {
    case JsonType::J_NULL:
    case JsonType::J_STRING:
    case JsonType::J_LONG:
    case JsonType::J_DOUBLE:
    case JsonType::J_BOOL:
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
    }
    break;
  }
}

void jsonio::json::clear() {
  flags_ = PHASE_START;
  PARENT_TYPE::operator=(PARENT_TYPE{});
}

bool jsonio::json::operator==(const json &that) const {
  return *static_cast<const PARENT_TYPE *>(this) ==
         *static_cast<const PARENT_TYPE *>(&that);
}

bool jsonio::json::completed() const {
  return (flags_ & MASK_PHASE) == PHASE_COMPLETED;
}

std::size_t jsonio::json::read(std::istream &is,
                               const std::string &delimiters) {
  std::size_t delimiter = -1;
  if ((flags_ & MASK_PHASE) == PHASE_COMPLETED) {
    flags_ = PHASE_START;
  }
  if ((flags_ & MASK_PHASE) == PHASE_START) {
    char source;
    while (is >> source) {
      if (!isspace(source)) {
        if (source == ']') {
          delimiter = delimiters.find(source);
          flags_ &= ~MASK_PHASE;
          flags_ |= PHASE_COMPLETED;
          flags_ |= EMPTY_VALUE;
        } else if (source == '{') {
          flags_ &= ~MASK_PHASE;
          flags_ |= PHASE_OBJECT;
          PARENT_TYPE::operator=(json_obj(json_obj::SKIP_PREFIX));
        } else if (source == '[') {
          flags_ &= ~MASK_PHASE;
          flags_ |= PHASE_ARRAY;
          PARENT_TYPE::operator=(json_arr(json_arr::SKIP_PREFIX));
        } else if (source == '\"') {
          flags_ &= ~MASK_PHASE;
          flags_ |= PHASE_STRING;
          PARENT_TYPE::operator=(json_string());
        } else {
          flags_ &= ~MASK_PHASE;
          flags_ |= PHASE_BINARY;
          binary_.clear();
          binary_.append(1, source);
        }
        break;
      }
    }
  }
  if ((flags_ & MASK_PHASE) == PHASE_OBJECT) {
    std::get<json_obj>(**this).read(is);
    if (is.good()) {
      flags_ &= ~MASK_PHASE;
      flags_ |= PHASE_DELIMITER;
    }
  }
  if ((flags_ & MASK_PHASE) == PHASE_ARRAY) {
    std::get<json_arr>(**this).read(is);
    if (is.good()) {
      flags_ &= ~MASK_PHASE;
      flags_ |= PHASE_DELIMITER;
    }
  }
  if ((flags_ & MASK_PHASE) == PHASE_STRING) {
    std::get<json_string>(**this).read(is);
    if (is.good()) {
      flags_ &= ~MASK_PHASE;
      flags_ |= PHASE_DELIMITER;
    }
  }
  if ((flags_ & MASK_PHASE) == PHASE_BINARY) {
    char source;
    while (is >> source) {
      delimiter = delimiters.find(source);
      if (delimiter == std::string::npos) {
        binary_.append(1, source);
      } else {
        break;
      }
    }
    if (is.good() || delimiters == "\n") {
      binary_.erase(std::find_if(binary_.rbegin(), binary_.rend(),
                                 [](const char c) { return !std::isspace(c); })
                        .base(),
                    binary_.end());
      if (binary_ == "null") {
        PARENT_TYPE::operator=(PARENT_TYPE{});
        flags_ &= ~MASK_PHASE;
        flags_ |= PHASE_COMPLETED;
      } else if (binary_ == "true") {
        PARENT_TYPE::operator=(true);
        flags_ &= ~MASK_PHASE;
        flags_ |= PHASE_COMPLETED;
      } else if (binary_ == "false") {
        PARENT_TYPE::operator=(false);
        flags_ &= ~MASK_PHASE;
        flags_ |= PHASE_COMPLETED;
      } else {
        char *end_ptr;
        if (binary_.find('.') != std::string::npos) {
          PARENT_TYPE::operator=(strtod(binary_.c_str(), &end_ptr));
        } else {
          PARENT_TYPE::operator=(strtol(binary_.c_str(), &end_ptr, 0));
        }
        if (*end_ptr == '\0') {
          flags_ &= ~MASK_PHASE;
          flags_ |= PHASE_COMPLETED;
        } else {
          is.setstate(std::ios::iostate::_S_badbit);
        }
      }
      binary_.clear();
    }
  }
  if ((flags_ & MASK_PHASE) == PHASE_DELIMITER) {
    if (delimiters == "\n") {
      flags_ &= ~MASK_PHASE;
      flags_ |= PHASE_COMPLETED;
    } else {
      char source;
      while (is >> source) {
        delimiter = delimiters.find(source);
        if (delimiter != std::string::npos) {
          flags_ &= ~MASK_PHASE;
          flags_ |= PHASE_COMPLETED;
          break;
        } else if (!isspace(source)) {
          is.setstate(std::ios::iostate::_S_badbit);
          break;
        }
      }
    }
  }
  return delimiter;
}

void jsonio::json::write(std::ostream &os, int indents) const {
  if (completed()) {
    switch (type()) {
    case JsonType::J_NULL:
      for (int i = 0; i < indents; ++i) {
        os << '\t';
      }
      os << "null";
      break;
    case JsonType::J_STRING:
      for (int i = 0; i < indents; ++i) {
        os << '\t';
      }
      os << '\"';
      std::get<json_string>(**this).write(os);
      os << '\"';
      break;
    case JsonType::J_LONG:
      for (int i = 0; i < indents; ++i) {
        os << '\t';
      }
      os << get_long();
      break;
    case JsonType::J_DOUBLE:
      for (int i = 0; i < indents; ++i) {
        os << '\t';
      }
      os << std::setprecision(std::numeric_limits<double>::max_digits10)
         << get_double();
      break;
    case JsonType::J_BOOL:
      for (int i = 0; i < indents; ++i) {
        os << '\t';
      }
      os << (get_bool() ? "true" : "false");
      break;
    case JsonType::J_ARRAY:
      std::get<json_arr>(**this).write(os, indents);
      break;
    case JsonType::J_OBJECT:
      std::get<json_obj>(**this).write(os, indents);
      break;
    }
  }
}

jsonio::JsonType jsonio::json::type() const {
  if (!has_value()) {
    return JsonType::J_NULL;
  }
  return static_cast<JsonType>((*this)->index());
}

jsonio::json &jsonio::json::operator[](const std::string &key) {
  return std::get<json_obj>(**this).operator[](key);
}

const jsonio::json &jsonio::json::operator[](const std::string &key) const {
  return std::get<json_obj>(**this).operator[](key);
}

jsonio::json &jsonio::json::operator[](std::size_t index) {
  return std::get<json_arr>(**this).operator[](index);
}

const jsonio::json &jsonio::json::operator[](std::size_t index) const {
  return std::get<json_arr>(**this).operator[](index);
}

jsonio::json *jsonio::json::at(const std::string &key) {
  return std::get<json_obj>(**this).at(key);
}

const jsonio::json *jsonio::json::at(const std::string &key) const {
  return std::get<json_obj>(**this).at(key);
}

bool jsonio::json::is_null() { return !has_value(); }

std::string &jsonio::json::get_string() {
  return const_cast<std::string &>(
      static_cast<const json &>(*this).get_string());
}

const std::string &jsonio::json::get_string() const {
  return std::get<json_string>(**this);
}

long &jsonio::json::get_long() {
  return const_cast<long &>(static_cast<const json &>(*this).get_long());
}

const long &jsonio::json::get_long() const { return std::get<long>(**this); }

double &jsonio::json::get_double() {
  return const_cast<double &>(static_cast<const json &>(*this).get_double());
}

const double &jsonio::json::get_double() const {
  return std::get<double>(**this);
}

bool &jsonio::json::get_bool() {
  return const_cast<bool &>(static_cast<const json &>(*this).get_bool());
}

const bool &jsonio::json::get_bool() const { return std::get<bool>(**this); }

jsonio::json_obj &jsonio::json::get_object() {
  return const_cast<jsonio::json_obj &>(
      static_cast<const json &>(*this).get_object());
}

const jsonio::json_obj &jsonio::json::get_object() const {
  return std::get<json_obj>(**this);
}

jsonio::json_arr &jsonio::json::get_array() {
  return const_cast<json_arr &>(static_cast<const json &>(*this).get_array());
}

const jsonio::json_arr &jsonio::json::get_array() const {
  return std::get<json_arr>(**this);
}

int jsonio::json::get_int() const {
  return static_cast<int>(std::get<long>(**this));
}

int jsonio::json::get_uint() const {
  return static_cast<unsigned int>(std::get<long>(**this));
}

float jsonio::json::get_float() const {
  return static_cast<float>(std::get<double>(**this));
}

std::istream &jsonio::operator>>(std::istream &is, jsonio::json &target) {
  target.read(is, "\n");
  return is;
}

std::ostream &jsonio::operator<<(std::ostream &os, const jsonio::json &source) {
  source.write(os, 0);
  return os;
}

template <>
std::istream &jsonio::operator>>(std::istream &is, jsonio::json_arr &target) {
  target.read(is);
  return is;
}

template <>
std::ostream &jsonio::operator<<(std::ostream &os,
                                 const jsonio::json_arr &source) {
  source.write(os, 0);
  return os;
}

template <>
std::istream &jsonio::operator>>(std::istream &is, jsonio::json_obj &target) {
  target.read(is);
  return is;
}

template <>
std::ostream &jsonio::operator<<(std::ostream &os,
                                 const jsonio::json_obj &source) {
  source.write(os, 0);
  return os;
}
