#ifndef SC2_TOKENS_HPP_INCLUDED
#define SC2_TOKENS_HPP_INCLUDED

#include <sc2/tokens.hpp>

#include <format>
#include <string>
#include <utility>

namespace SC2 {
  class Identifier
  {
    std::string name{};

    public:
    constexpr Identifier() = default;
    explicit constexpr Identifier(std::string_view const name): name{ name } {}
    constexpr Identifier(Identifier const &) = default;
    [[nodiscard]] constexpr auto operator<=>(Identifier const &) const
      = default;
    // References could be a problem
    [[nodiscard]] constexpr std::string const &getName() const noexcept(false)
    {
      return name;
    }
  };

  class LiteralConstant
  {
    int value{};

    public:
    constexpr LiteralConstant() = default;
    explicit constexpr LiteralConstant(int const value): value{ value } {}
    constexpr LiteralConstant(LiteralConstant const &)        = default;
    constexpr auto operator<=>(LiteralConstant const &) const = default;
    [[nodiscard]] constexpr int getValue() const noexcept(false)
    {
      return value;
    }
  };

  enum class Keyword
  {
    INT,
    RETURN,
    VOID
  };

  enum class Parenthesis
  {
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS
  };

  enum class Brace
  {
    LEFT_BRACE,
    RIGHT_BRACE
  };

  inline constexpr struct SemicolonTag
  {
    constexpr auto operator<=>(SemicolonTag const &) const = default;
  } Semicolon{};

  template <typename... Ts> struct overloaded: Ts...
  {
    using Ts::operator()...;
  };

  template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

  class Token
  {
    using type = std::variant<
      SemicolonTag,
      Identifier,
      LiteralConstant,
      Keyword,
      Parenthesis,
      Brace>;
    type token{};

    public:
    constexpr Token() = default;
    explicit constexpr Token(type const &token): token{ token } {}
    constexpr Token(Token const &)                    = default;
    constexpr auto   operator<=>(Token const &) const = default;
    constexpr Token &operator=(Token const &)         = default;
    [[nodiscard]] constexpr std::string toString() const noexcept(false)
    {
      return std::visit(
        overloaded(
          [](SemicolonTag) -> std::string { return "semicolon"; },
          [](Identifier const &identifier) -> std::string {
        return std::format("Identifier: {}", identifier.getName());
      },
          [](LiteralConstant const &literalConstant) -> std::string {
        return std::format("Literal constant: {}", literalConstant.getValue());
      },
          [](Keyword keyword) -> std::string {
        return std::format("Keyword: {}", [keyword]() {
          switch (keyword) {
            case Keyword::INT: return "int"; break;
            case Keyword::RETURN: return "return"; break;
            case Keyword::VOID: return "void"; break;
            default: std::unreachable();
          }
        }());
      },
          [](Parenthesis parenthesis) -> std::string {
        switch (parenthesis) {
          case Parenthesis::LEFT_PARENTHESIS: return "left parenthesis"; break;
          case Parenthesis::RIGHT_PARENTHESIS:
            return "right parenthesis";
            break;
          default: std::unreachable();
        }
      },
          [](Brace brace) -> std::string {
        switch (brace) {
          case Brace::LEFT_BRACE: return "left curly brace"; break;
          case Brace::RIGHT_BRACE: return "right curly brace"; break;
          default: std::unreachable();
        }
      }
        ),
        token
      );
    }
    [[nodiscard]] Identifier      getIdentifier() const noexcept(false);
    [[nodiscard]] LiteralConstant getLiteralConstant() const;
    [[nodiscard]] Keyword         getKeyword() const noexcept(false);
    [[nodiscard]] Parenthesis     getParenthesis() const noexcept(false);
    [[nodiscard]] Brace           getBrace() const noexcept(false);
    [[nodiscard]] SemicolonTag    getSemicolon() const noexcept(false);
  };

  class TokenConversionError: public std::exception
  {
    std::string const message{};
    Token const       source_token{};
    std::string const destination_token_type{};

    public:
    constexpr TokenConversionError(
      Token const           &source_token,
      std::string_view const destination_token_type
    )
      : message{ std::format(
          "Cannot convert {} token into a(n) {} token",
          source_token.toString(),
          destination_token_type
        ) }
      , source_token{ source_token }
      , destination_token_type{ destination_token_type }
    {}
    constexpr Token       getSourceToken() const { return source_token; }
    constexpr std::string getDestinationTokenType() const
    {
      return destination_token_type;
    }
    virtual constexpr char const *what() const noexcept
    {
      return message.c_str();
    }
    virtual ~TokenConversionError() override = default;
  };

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

} // namespace SC2

#endif
