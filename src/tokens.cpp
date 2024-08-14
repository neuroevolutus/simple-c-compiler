#include <sc2/tokens.hpp>

#include <variant>

namespace SC2 {
  [[nodiscard]] Identifier Token::getIdentifier() const noexcept(false)
  try {
    return std::get<Identifier>(token);
  } catch (std::bad_variant_access const &) {
    throw TokenConversionError(*this, "identifier");
  }

  [[nodiscard]] LiteralConstant Token::getLiteralConstant() const
    noexcept(false)
  try {
    return std::get<LiteralConstant>(token);
  } catch (std::bad_variant_access const &) {
    throw TokenConversionError(*this, "literal constant");
  }

  [[nodiscard]] Keyword Token::getKeyword() const noexcept(false)
  try {
    return std::get<Keyword>(token);
  } catch (std::bad_variant_access const &) {
    throw TokenConversionError(*this, "keyword");
  }

  [[nodiscard]] Parenthesis Token::getParenthesis() const noexcept(false)
  try {
    return std::get<Parenthesis>(token);
  } catch (std::bad_variant_access const &) {
    throw TokenConversionError(*this, "parenthesis");
  }

  [[nodiscard]] Brace Token::getBrace() const noexcept(false)
  try {
    return std::get<Brace>(token);
  } catch (std::bad_variant_access const &) {
    throw TokenConversionError(*this, "brace");
  }

  [[nodiscard]] SemicolonTag Token::getSemicolon() const noexcept(false)
  try {
    return std::get<SemicolonTag>(token);
  } catch (std::bad_variant_access const &) {
    throw TokenConversionError(*this, "semicolon");
  }

  [[nodiscard]] TildeTag Token::getTilde() const noexcept(false)
  try {
    return std::get<TildeTag>(token);
  } catch (std::bad_variant_access const &) {
    throw TokenConversionError(*this, "tilde");
  }

  [[nodiscard]] HyphenTag Token::getHyphen() const noexcept(false)
  try {
    return std::get<HyphenTag>(token);
  } catch (std::bad_variant_access const &) {
    throw TokenConversionError(*this, "hyphen");
  }

  [[nodiscard]] DecrementTag Token::getDecrement() const noexcept(false)
  try {
    return std::get<DecrementTag>(token);
  } catch (std::bad_variant_access const &) {
    throw TokenConversionError(*this, "decrement");
  }
} // namespace SC2
