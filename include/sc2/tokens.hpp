#ifndef SC2_TOKENS_HPP_INCLUDED
#define SC2_TOKENS_HPP_INCLUDED

#include <sc2/compiler_error.hpp>

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

  inline constexpr struct TildeTag
  {
    constexpr auto operator<=>(TildeTag const &) const = default;
  } Tilde{};

  inline constexpr struct HyphenTag
  {
    constexpr auto operator<=>(HyphenTag const &) const = default;
  } Hyphen{};

  inline constexpr struct DecrementTag
  {
    constexpr auto operator<=>(DecrementTag const &) const = default;
  } Decrement{};

  template <typename... Ts> struct Overloaded: Ts...
  {
    using Ts::operator()...;
  };

  template <typename... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

  class Token
  {
    using type = std::variant<
      SemicolonTag,
      TildeTag,
      HyphenTag,
      DecrementTag,
      Parenthesis,
      Brace,
      Identifier,
      LiteralConstant,
      Keyword>;
    type token{};

    public:
    constexpr Token() = default;
    explicit constexpr Token(type const &token): token{ token } {}
    constexpr Token(Token const &)                    = default;
    constexpr auto   operator<=>(Token const &) const = default;
    constexpr Token &operator=(Token const &)         = default;

    // clang-format off
    [[nodiscard]] constexpr std::string toString() const noexcept(false)
    {
      using namespace std::literals::string_literals;
      return std::visit(
        Overloaded(
          [](SemicolonTag) { return "semicolon"s; },
          [](TildeTag) { return "tilde"s; },
          [](HyphenTag) { return "hyphen"s; },
          [](DecrementTag) { return "decrement"s; },
          [](Identifier const &identifier) {
            return std::format("Identifier: {}", identifier.getName());
          },
          [](LiteralConstant const &literalConstant) {
            return std::format("Literal constant: {}", literalConstant.getValue());
          },
          [](Keyword keyword) {
            return std::format("Keyword: {}", [keyword]() {
              switch (keyword) {
                case Keyword::INT: return "int"s; break;
                case Keyword::RETURN: return "return"s; break;
                case Keyword::VOID: return "void"s; break;
                default: std::unreachable();
              }
            }());
          },
          [](Parenthesis parenthesis) {
            switch (parenthesis) {
              case Parenthesis::LEFT_PARENTHESIS:
                return "left parenthesis"s;
                break;
              case Parenthesis::RIGHT_PARENTHESIS:
                return "right parenthesis"s;
                break;
              default: std::unreachable();
            }
          },
          [](Brace brace) {
            switch (brace) {
              case Brace::LEFT_BRACE: return "left curly brace"s; break;
              case Brace::RIGHT_BRACE: return "right curly brace"s; break;
              default: std::unreachable();
            }
          }
        ),
        token
      );
    } // clang-format on

    [[nodiscard]] constexpr bool isLiteralConstant() const noexcept
    {
      return std::holds_alternative<LiteralConstant>(token);
    }

    [[nodiscard]] constexpr bool isTilde() const noexcept
    {
      return std::holds_alternative<TildeTag>(token);
    }

    [[nodiscard]] constexpr bool isHyphen() const noexcept
    {
      return std::holds_alternative<HyphenTag>(token);
    }

    [[nodiscard]] constexpr bool isDecrement() const noexcept
    {
      return std::holds_alternative<DecrementTag>(token);
    }

    [[nodiscard]] constexpr bool isParenthesis() const noexcept
    {
      return std::holds_alternative<Parenthesis>(token);
    }

    [[nodiscard]] Identifier      getIdentifier() const noexcept(false);
    [[nodiscard]] LiteralConstant getLiteralConstant() const;
    [[nodiscard]] Keyword         getKeyword() const noexcept(false);
    [[nodiscard]] Parenthesis     getParenthesis() const noexcept(false);
    [[nodiscard]] Brace           getBrace() const noexcept(false);
    [[nodiscard]] SemicolonTag    getSemicolon() const noexcept(false);
    [[nodiscard]] TildeTag        getTilde() const noexcept(false);
    [[nodiscard]] HyphenTag       getHyphen() const noexcept(false);
    [[nodiscard]] DecrementTag    getDecrement() const noexcept(false);
  };

  class TokenConversionError: public CompilerError
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
} // namespace SC2

#endif
