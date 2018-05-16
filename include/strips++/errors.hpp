#pragma once
#include <stdexcept>

namespace strips {

class DuktapeError : public std::runtime_error {

public:
  DuktapeError(const char *what) : std::runtime_error(what) {}
  DuktapeError(const std::string &what) : std::runtime_error(what) {}
  virtual ~DuktapeError() {}
};

} // namespace strips