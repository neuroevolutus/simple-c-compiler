#ifndef PRETTY_PRINTER_MIXIN_HPP_INCLUDED
#define PRETTY_PRINTER_MIXIN_HPP_INCLUDED

#include <cstdlib>
#include <sstream>

namespace SC2 {
  struct PrettyPrinterMixin
  {
    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) noexcept(false)
      = 0;
    [[nodiscard]] constexpr std::string prettyPrint()
    {
      std::ostringstream out{};
      prettyPrintHelper(out, 0);
      return out.str();
    }
  };
} // namespace SC2

#endif
