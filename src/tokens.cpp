#include <sc2/tokens.hpp>

#include <memory>

namespace SC2 {
  [[nodiscard]] std::shared_ptr<IdentifierToken> Token::getIdentifier() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<IdentifierToken>(token) };
        ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "identifier");
  }

  [[nodiscard]] std::shared_ptr<LiteralConstantToken>
  Token::getLiteralConstant() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<LiteralConstantToken>(token) };
        ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "literal constant");
  }

  [[nodiscard]] std::shared_ptr<IntKeywordToken> Token::getIntKeyword() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<IntKeywordToken>(token) };
        ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "int keyword");
  }

  [[nodiscard]] std::shared_ptr<ReturnKeywordToken>
  Token::getReturnKeyword() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<ReturnKeywordToken>(token) };
        ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "return keyword");
  }

  [[nodiscard]] std::shared_ptr<VoidKeywordToken> Token::getVoidKeyword() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<VoidKeywordToken>(token) };
        ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "void keyword");
  }

  [[nodiscard]] std::shared_ptr<ParenthesisToken>
  Token::getLeftParenthesis() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<LeftParenthesisToken>(token) };
        ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "left parenthesis");
  }

  [[nodiscard]] std::shared_ptr<ParenthesisToken>
  Token::getRightParenthesis() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<RightParenthesisToken>(token) };
        ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "right parenthesis");
  }

  [[nodiscard]] std::shared_ptr<CurlyBraceToken>
  Token::getLeftCurlyBrace() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<LeftCurlyBraceToken>(token) };
        ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "left curly brace");
  }

  [[nodiscard]] std::shared_ptr<CurlyBraceToken>
  Token::getRightCurlyBrace() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<RightCurlyBraceToken>(token) };
        ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "right curly brace");
  }

  [[nodiscard]] std::shared_ptr<SemicolonToken> Token::getSemicolon() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<SemicolonToken>(token) }; ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "semicolon");
  }

  [[nodiscard]] std::shared_ptr<TildeToken> Token::getTilde() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<TildeToken>(token) }; ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "tilde");
  }

  [[nodiscard]] std::shared_ptr<HyphenToken> Token::getHyphen() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<HyphenToken>(token) }; ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "hyphen");
  }

  [[nodiscard]] std::shared_ptr<DecrementToken> Token::getDecrement() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<DecrementToken>(token) }; ptr)
      return ptr;
    else
      throw TokenConversionError(*this, "decrement");
  }
} // namespace SC2
