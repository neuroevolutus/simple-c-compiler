#ifndef COMPILER_ERROR_HPP_INCLUDED
#define COMPILER_ERROR_HPP_INCLUDED

#include <exception>

namespace SC2 {
  struct CompilerError: public std::exception
  {
    virtual ~CompilerError() = default;
  };
} // namespace SC2

#endif
