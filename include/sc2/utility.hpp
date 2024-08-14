#ifndef SC2_UTILITY_HPP_INCLUDED
#define SC2_UTILITY_HPP_INCLUDED

#include <sc2/tokens.hpp>

#include <cstddef>
#include <format>
#include <ostream>
#include <string>

namespace SC2 {
  struct Utility
  {
    static constexpr void indent(std::ostream &out, std::size_t indent_level)
    {
      for (std::size_t indents{}; indents < indent_level; ++indents) {
        out << " ";
      }
    }

    static constexpr void emitAssemblyEpilogue(std::ostream &out)
    {
#if defined(__linux__)
      out << R"(.section .note.GNU-stack,"",@progbits)";
#endif
    }

    static constexpr std::string
    specialiseFunctionNameForOS(std::string_view name)
    {
      return
#if defined(__APPLE__) || defined(__MACH__)
        "_" + std::string{ name }
#else
        name
#endif
      ;
    }

    static Identifier generateFreshIdentifier(Identifier identifier)
    {
      static std::size_t counter{};
      return Identifier(std::format("{}.{}", identifier.getName(), counter++));
    }
  };
} // namespace SC2

#endif
