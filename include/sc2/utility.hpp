#ifndef SC2_UTILITY_HPP_INCLUDED
#define SC2_UTILITY_HPP_INCLUDED

namespace SC2 {
  struct Utility
  {
    static constexpr void
    indent(std::ostringstream &out, std::size_t indent_level)
    {
      for (std::size_t indents{}; indents < indent_level; ++indents) {
        out << " ";
      }
    }
  };
} // namespace SC2

#endif
