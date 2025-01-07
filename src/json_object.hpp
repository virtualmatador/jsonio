#ifndef JSONIO_SRC_JSON_OBJECT_HPP
#define JSONIO_SRC_JSON_OBJECT_HPP

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "json_string.h"

namespace jsonio {

template <class json> class json_object;

template <class json>
using JSON_OBJECT_PARENT = std::unordered_map<json_string, json>;

template <class json> class json_object : public JSON_OBJECT_PARENT<json> {
private:
  using PARENT_TYPE = JSON_OBJECT_PARENT<json>;
  unsigned int flags_;
  json_string key_;
  std::unique_ptr<json> value_;

public:
  enum : unsigned int {
    PHASE_START = 0x0000,
    PHASE_KEY_START = 0x0001,
    PHASE_KEY_TEXT = 0x0002,
    PHASE_COLON = 0x0003,
    PHASE_VALUE = 0x0004,
    PHASE_COMPLETED = 0x0007,
    SKIP_PREFIX = 0x0008,
  };

public:
  json_object(std::vector<std::pair<json_string, json>> &&init) noexcept
      : flags_{PHASE_COMPLETED}, value_{std::make_unique<json>()} {
    for (auto &[key, value] : init) {
      PARENT_TYPE::insert({std::move(key), std::move(value)});
    }
  }

  json_object() noexcept
      : flags_{PHASE_COMPLETED}, value_{std::make_unique<json>()} {}

  json_object(const unsigned int flags) noexcept
      : flags_{flags}, value_{std::make_unique<json>()} {}

  json_object(const json_object &source) noexcept
      : value_{std::make_unique<json>()} {
    *this = source;
  }

  json_object(json_object &&source) noexcept
      : value_{std::make_unique<json>()} {
    *this = std::move(source);
  }

  json_object &operator=(const json_object &source) noexcept {
    if (this != &source) {
      PARENT_TYPE::operator=(*(PARENT_TYPE *)&source);
      flags_ = source.flags_;
      key_ = source.key_;
      *value_ = *source.value_;
    }
    return *this;
  }

  json_object &operator=(json_object &&source) noexcept {
    if (this != &source) {
      PARENT_TYPE::operator=(std::move(*(PARENT_TYPE *)&source));
      flags_ = source.flags_;
      source.flags_ = PHASE_START;
      key_ = std::move(source.key_);
      *value_ = std::move(*source.value_);
    }
    return *this;
  }

  ~json_object() noexcept {}

  bool completed() const {
    return (flags_ & PHASE_COMPLETED) == PHASE_COMPLETED;
  }

  json &operator[](const std::string &key) {
    auto pair = PARENT_TYPE::find(key);
    if (pair == PARENT_TYPE::end()) {
      auto [node, inserted] = PARENT_TYPE::insert({key, {}});
      pair = node;
    }
    return pair->second;
  }

  const json &operator[](const std::string &key) const {
    auto pair = PARENT_TYPE::find(key);
    if (pair == PARENT_TYPE::end()) {
      throw std::invalid_argument("jsonio::json_obj::operator[] " + key);
    }
    return pair->second;
  }

  json *at(const std::string &key) {
    return const_cast<json *>(static_cast<const json_object &>(*this).at(key));
  }

  const json *at(const std::string &key) const {
    if (auto it = PARENT_TYPE::find(key); it != PARENT_TYPE::end()) {
      return &it->second;
    }
    return nullptr;
  }

  void steal(const json_object &source, bool convert) {
    for (auto &[key, index_value] : *this) {
      auto source_node = source.find(key);
      if (source_node != source.end()) {
        index_value.steal(source_node->second, convert);
      }
    }
  }

  void read(std::istream &is) {
    if ((flags_ & PHASE_COMPLETED) == PHASE_COMPLETED) {
      flags_ = PHASE_START;
    }
    if ((flags_ & PHASE_COMPLETED) == PHASE_START) {
      PARENT_TYPE::clear();
      if (flags_ & SKIP_PREFIX) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_KEY_START;
      } else {
        char source;
        while (is >> source) {
          if (std::isspace(source)) {
            continue;
          } else if (source == '{') {
            flags_ &= ~PHASE_COMPLETED;
            flags_ |= PHASE_KEY_START;
          } else {
            is.setstate(std::ios::iostate::_S_badbit);
          }
          break;
        }
      }
    }
    if ((flags_ & PHASE_COMPLETED) == PHASE_KEY_START) {
      key_.flags_ = json_string::PHASE_START;
      char source;
      while (is >> source) {
        if (std::isspace(source)) {
          continue;
        } else if (source == '\"') {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_KEY_TEXT;
          break;
        } else if (source == '}') {
          if (PARENT_TYPE::size() == 0) {
            flags_ |= PHASE_COMPLETED;
          } else {
            is.setstate(std::ios::iostate::_S_badbit);
          }
          break;
        } else {
          is.setstate(std::ios::iostate::_S_badbit);
        }
        break;
      }
    }
    if ((flags_ & PHASE_COMPLETED) == PHASE_KEY_TEXT) {
      key_.read(is);
      if (is.good()) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_COLON;
      }
    }
    if ((flags_ & PHASE_COMPLETED) == PHASE_COLON) {
      char source;
      while (is >> source) {
        if (std::isspace(source)) {
          continue;
        } else if (source == ':') {
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_VALUE;
        } else {
          is.setstate(std::ios::iostate::_S_badbit);
        }
        break;
      }
    }
    if ((flags_ & PHASE_COMPLETED) == PHASE_VALUE) {
      const std::string delimiters = ",}";
      std::size_t delimiter = value_->read(is, delimiters);
      if (is.good()) {
        bool read_again = false;
        bool append = true;
        if (delimiters[delimiter] == ',') {
          read_again = true;
          flags_ &= ~PHASE_COMPLETED;
          flags_ |= PHASE_KEY_START;
        } else if (delimiters[delimiter] == '}') {
          flags_ |= PHASE_COMPLETED;
        } else {
          append = false;
          flags_ = PHASE_START;
          is.setstate(std::ios::iostate::_S_badbit);
        }
        if (append) {
          PARENT_TYPE::insert({std::move(key_), std::move(*value_)});
        }
        if (read_again) {
          read(is);
        }
      }
    }
  }

  void write(std::ostream &os, int indents, unsigned int flags) const {
    if (completed()) {
      if (flags & json::Format_Options::prettify) {
        for (int i = 0; i < indents; ++i) {
          os << '\t';
        }
      }
      os << '{';
      if (flags & json::Format_Options::sort_asc) {
        write_sorted<std::less<json_string>>(os, indents, flags);
      } else if (flags & json::Format_Options::sort_desc) {
        write_sorted<std::greater<json_string>>(os, indents, flags);
      } else {
        write_unsorted(os, indents, flags);
      }
      if (flags & json::Format_Options::prettify) {
        os << '\n';
        for (int i = 0; i < indents; ++i) {
          os << '\t';
        }
      }
      os << '}';
    }
  }

private:
  void write_unsorted(std::ostream &os, int indents, unsigned int flags) const {
    bool comma = false;
    for (const auto &pair : *this) {
      write_pair(os, indents, flags, comma, pair);
    }
  }

  template <class comparator>
  void write_sorted(std::ostream &os, int indents, unsigned int flags) const {
    std::vector<std::reference_wrapper<const typename PARENT_TYPE::value_type>>
        pairs(PARENT_TYPE::cbegin(), PARENT_TYPE::cend());
    std::sort(pairs.begin(), pairs.end(), [](const auto &l, const auto &r) {
      return comparator()(l.get().first, r.get().first);
    });
    bool comma = false;
    for (const auto &pair : pairs) {
      write_pair(os, indents, flags, comma, pair.get());
    }
  }

  void write_pair(std::ostream &os, int indents, unsigned int flags,
                  bool &comma, const PARENT_TYPE::value_type &pair) const {
    if (comma) {
      os << ',';
    } else {
      comma = true;
    }
    if (flags & json::Format_Options::prettify) {
      os << '\n';
      for (int i = 0; i < indents + 1; ++i) {
        os << '\t';
      }
    }
    os << '\"' << pair.first << "\":";
    pair.second.write(os, true, indents + 1, flags);
  }

public:
  template <class>
  friend std::istream &operator>>(std::istream &is, json_object<json> &target);
  template <class>
  friend std::ostream &operator<<(std::ostream &os,
                                  const json_object<json> &source);
};

template <class json>
std::istream &operator>>(std::istream &is, json_object<json> &target) {
  target.read(is);
  return is;
}

template <class json>
std::ostream &operator<<(std::ostream &os, const json_object<json> &source) {
  source.write(os, 0, 0);
  return os;
}

} // namespace jsonio

#endif // JSONIO_SRC_JSON_OBJECT_HPP
