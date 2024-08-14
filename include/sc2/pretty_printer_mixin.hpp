#ifndef PRETTY_PRINTER_MIXIN_HPP_INCLUDED
#define PRETTY_PRINTER_MIXIN_HPP_INCLUDED

#include <cstdlib>
#include <ostream>
#include <sstream>

namespace SC2 {
  struct PrettyPrinterMixin
  {
    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) noexcept(false)
      = 0;

    [[nodiscard]] std::string prettyPrint()
    {
      std::ostringstream out{};
      prettyPrintHelper(out, 0);
      return out.str();
    }
  };
} // namespace SC2

#endif
