#include <sc2/tokens.hpp>

#include <memory>

namespace SC2 {
  [[nodiscard]] std::shared_ptr<IdentifierToken> Token::getIdentifier() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<IdentifierToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "identifier");
  }

  [[nodiscard]] std::shared_ptr<LiteralConstantToken>
  Token::getLiteralConstant() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<LiteralConstantToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "literal constant");
  }

  [[nodiscard]] std::shared_ptr<IntKeywordToken> Token::getIntKeyword() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<IntKeywordToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "int keyword");
  }

  [[nodiscard]] std::shared_ptr<ReturnKeywordToken>
  Token::getReturnKeyword() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<ReturnKeywordToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "return keyword");
  }

  [[nodiscard]] std::shared_ptr<VoidKeywordToken> Token::getVoidKeyword() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<VoidKeywordToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "void keyword");
  }

  [[nodiscard]] std::shared_ptr<ParenthesisToken>
  Token::getLeftParenthesis() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<LeftParenthesisToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "left parenthesis");
  }

  [[nodiscard]] std::shared_ptr<ParenthesisToken>
  Token::getRightParenthesis() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<RightParenthesisToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "right parenthesis");
  }

  [[nodiscard]] std::shared_ptr<CurlyBraceToken>
  Token::getLeftCurlyBrace() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<LeftCurlyBraceToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "left curly brace");
  }

  [[nodiscard]] std::shared_ptr<CurlyBraceToken>
  Token::getRightCurlyBrace() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<RightCurlyBraceToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "right curly brace");
  }

  [[nodiscard]] std::shared_ptr<SemicolonToken> Token::getSemicolon() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<SemicolonToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "semicolon");
  }

  [[nodiscard]] std::shared_ptr<TildeToken> Token::getTilde() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<TildeToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "tilde");
  }

  [[nodiscard]] std::shared_ptr<HyphenToken> Token::getHyphen() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<HyphenToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "hyphen");
  }

  [[nodiscard]] std::shared_ptr<DecrementToken> Token::getDecrement() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<DecrementToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "decrement");
  }

  [[nodiscard]] std::shared_ptr<PlusSignToken> Token::getPlusSign() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<PlusSignToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "plus sign");
  }

  [[nodiscard]] std::shared_ptr<AsteriskToken> Token::getAsterisk() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<AsteriskToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "asterisk");
  }

  [[nodiscard]] std::shared_ptr<ForwardSlashToken>
  Token::getForwardSlash() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<ForwardSlashToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "forward slash");
  }

  [[nodiscard]] std::shared_ptr<PercentSignToken> Token::getPercentSign() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<PercentSignToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "percent sign");
  }

  [[nodiscard]] std::shared_ptr<BitwiseAndToken> Token::getBitwiseAnd() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<BitwiseAndToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "bitwise and");
  }

  [[nodiscard]] std::shared_ptr<BitwiseOrToken> Token::getBitwiseOr() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<BitwiseOrToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "bitwise or");
  }

  [[nodiscard]] std::shared_ptr<BitwiseXorToken> Token::getBitwiseXor() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<BitwiseXorToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "bitwise xor");
  }

  [[nodiscard]] std::shared_ptr<LeftShiftToken> Token::getLeftShift() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<LeftShiftToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "left shift");
  }

  [[nodiscard]] std::shared_ptr<RightShiftToken> Token::getRightShift() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<RightShiftToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "right shift");
  }

  [[nodiscard]] std::shared_ptr<ExclamationPointToken>
  Token::getExclamationPoint() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<ExclamationPointToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "exclamation point");
  }

  [[nodiscard]] std::shared_ptr<DoubleAmpersandToken>
  Token::getDoubleAmpersand() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<DoubleAmpersandToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "double ampersand");
  }

  [[nodiscard]] std::shared_ptr<DoublePipeToken> Token::getDoublePipe() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<DoublePipeToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "double pipe");
  }

  [[nodiscard]] std::shared_ptr<EqualToToken> Token::getEqualTo() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<EqualToToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "equal to");
  }

  [[nodiscard]] std::shared_ptr<NotEqualToToken> Token::getNotEqualTo() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<NotEqualToToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "not equal to");
  }

  [[nodiscard]] std::shared_ptr<LessThanToken> Token::getLessThan() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<LessThanToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "less than");
  }

  [[nodiscard]] std::shared_ptr<GreaterThanToken> Token::getGreaterThan() const
  {
    if (auto const ptr{ std::dynamic_pointer_cast<GreaterThanToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "greater than");
  }

  [[nodiscard]] std::shared_ptr<LessThanOrEqualToToken>
  Token::getLessThanOrEqualTo() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<LessThanOrEqualToToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "less than or equal to");
  }

  [[nodiscard]] std::shared_ptr<GreaterThanOrEqualToToken>
  Token::getGreaterThanOrEqualTo() const
  {
    if (auto const ptr{
          std::dynamic_pointer_cast<GreaterThanOrEqualToToken>(token) })
      return ptr;
    else
      throw TokenConversionError(*this, "greater than or equal to");
  }
} // namespace SC2
