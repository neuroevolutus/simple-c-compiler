#ifndef SEMANTIC_ANALYSIS_ERROR_HPP_INCLUDED
#define SEMANTIC_ANALYSIS_ERROR_HPP_INCLUDED

#include <exception>

namespace SC2 {
  struct SemanticAnalysisError: public std::exception
  {
    virtual ~SemanticAnalysisError() = default;
  };
} // namespace SC2

#endif
