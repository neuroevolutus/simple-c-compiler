#ifndef SC2_TOKENS_HPP_INCLUDED
#define SC2_TOKENS_HPP_INCLUDED

#include <sc2/compiler_error.hpp>
#include <string_view>

#include <compare>
#include <format>
#include <memory>
#include <string>
#include <utility>

namespace SC2 {
  struct BasicToken: public std::enable_shared_from_this<BasicToken>
  {
    [[nodiscard]] virtual constexpr bool operator==(BasicToken const &other
    ) const noexcept
    {
      return typeid(*this) == typeid(other);
    }

    [[nodiscard]] virtual bool isUnaryOperatorToken() const noexcept
    {
      return false;
    }

    [[nodiscard]] virtual bool isBinaryOperatorToken() const noexcept
    {
      return false;
    }

    [[nodiscard]] virtual constexpr std::string toString() const = 0;

    virtual ~BasicToken() = default;
  };

  class IdentifierToken final: public BasicToken
  {
    std::string name{};

    public:
    constexpr IdentifierToken() = default;

    explicit constexpr IdentifierToken(std::string_view const name)
      : name{ name }
    {}

    IdentifierToken(IdentifierToken const &) = default;

    [[nodiscard]] virtual constexpr bool operator==(BasicToken const &other
    ) const noexcept final override
    {
      return typeid(*this) == typeid(other) ?
               static_cast<IdentifierToken const &>(other).operator==(*this) :
               false;
    }

    [[nodiscard]] constexpr std::string_view getName() const noexcept
    {
      return name;
    }

    [[nodiscard]] constexpr bool operator==(IdentifierToken const &other
    ) const noexcept
    {
      return getName() == other.getName();
    }

    [[nodiscard]] virtual constexpr std::string toString() const final override
    {
      return std::format("Identifier: {}", getName());
    }

    virtual ~IdentifierToken() final override = default;
  };

  class LiteralConstantToken final: public BasicToken
  {
    int value{};

    public:
    constexpr LiteralConstantToken() = default;

    explicit constexpr LiteralConstantToken(int const value): value{ value } {}

    LiteralConstantToken(LiteralConstantToken const &) = default;

    [[nodiscard]] virtual constexpr bool operator==(BasicToken const &other
    ) const noexcept final override
    {
      return typeid(*this) == typeid(other) ?
               static_cast<LiteralConstantToken const &>(other).operator==(*this
               ) :
               false;
    }

    [[nodiscard]] constexpr int getValue() const noexcept { return value; }

    [[nodiscard]] virtual constexpr bool
    operator==(LiteralConstantToken const &other) const noexcept
    {
      return getValue() == other.getValue();
    }

    [[nodiscard]] virtual constexpr std::string toString() const final override
    {
      return std::format("Literal constant: {}", value);
    }

    virtual ~LiteralConstantToken() final override = default;
  };

  class KeywordToken: public BasicToken
  {
    protected:
    [[nodiscard]] virtual constexpr std::string getKeyword() const = 0;

    public:
    [[nodiscard]] virtual constexpr std::string toString() const final override
    {
      return std::format("Keyword: {}", getKeyword());
    }
  };

  class IntKeywordToken final: public KeywordToken
  {
    protected:
    [[nodiscard]] virtual constexpr std::string
    getKeyword() const final override
    {
      return "int";
    }

    public:
    virtual ~IntKeywordToken() final override = default;
  };

  class ReturnKeywordToken final: public KeywordToken
  {
    protected:
    [[nodiscard]] virtual constexpr std::string
    getKeyword() const final override
    {
      return "return";
    }

    public:
    virtual ~ReturnKeywordToken() final override = default;
  };

  class VoidKeywordToken final: public KeywordToken
  {
    protected:
    [[nodiscard]] virtual constexpr std::string
    getKeyword() const final override
    {
      return "void";
    }

    public:
    virtual ~VoidKeywordToken() final override = default;
  };

  struct ParenthesisToken: public BasicToken
  {
    virtual ~ParenthesisToken() override = default;
  };

  struct LeftParenthesisToken final: public ParenthesisToken
  {
    [[nodiscard]] virtual constexpr std::string toString() const final override
    {
      return "left parenthesis";
    }

    virtual ~LeftParenthesisToken() final override = default;
  };

  struct RightParenthesisToken final: public ParenthesisToken
  {
    [[nodiscard]] virtual constexpr std::string toString() const final override
    {
      return "right parenthesis";
    }

    virtual ~RightParenthesisToken() final override = default;
  };

  struct CurlyBraceToken: public BasicToken
  {
    virtual ~CurlyBraceToken() override = default;
  };

  struct LeftCurlyBraceToken final: public CurlyBraceToken
  {
    [[nodiscard]] virtual constexpr std::string toString() const final override
    {
      return "left curly brace";
    }

    virtual ~LeftCurlyBraceToken() final override = default;
  };

  struct RightCurlyBraceToken final: public CurlyBraceToken
  {
    [[nodiscard]] virtual constexpr std::string toString() const final override
    {
      return "right curly brace";
    }

    virtual ~RightCurlyBraceToken() final override = default;
  };

  struct SemicolonToken final: public BasicToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "semicolon";
    }

    constexpr auto operator<=>(SemicolonToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~SemicolonToken() final override = default;
  };

  struct UnaryOperatorToken: virtual public BasicToken
  {
    [[nodiscard]] virtual constexpr bool
    isUnaryOperatorToken() const noexcept final override
    {
      return true;
    };

    virtual ~UnaryOperatorToken() override = default;
  };

  struct TildeToken final: public UnaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "tilde";
    }

    constexpr auto operator<=>(TildeToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~TildeToken() final override = default;
  };

  struct BinaryOperatorToken: virtual public BasicToken
  {
    [[nodiscard]] virtual constexpr bool
    isBinaryOperatorToken() const noexcept final override
    {
      return true;
    }

    [[nodiscard]] virtual std::size_t getPrecedence() const = 0;

    virtual ~BinaryOperatorToken() override = default;
  };

  struct HyphenToken final
    : public UnaryOperatorToken
    , public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "hyphen";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 12;
    }

    constexpr auto operator<=>(HyphenToken const &) const noexcept
    {
      return std::strong_ordering::equal;
    }

    virtual ~HyphenToken() final override = default;
  };

  struct DecrementToken final: public BasicToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "decrement";
    }

    constexpr auto operator<=>(DecrementToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~DecrementToken() final override = default;
  };

  struct PlusSignToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "plus sign";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 12;
    }

    constexpr auto operator<=>(PlusSignToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~PlusSignToken() final override = default;
  };

  struct AsteriskToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "asterisk";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 13;
    }

    constexpr auto operator<=>(AsteriskToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~AsteriskToken() final override = default;
  };

  struct ForwardSlashToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "forward slash";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 13;
    }

    constexpr auto operator<=>(ForwardSlashToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~ForwardSlashToken() final override = default;
  };

  struct PercentSignToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "percent sign";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 13;
    }

    constexpr auto operator<=>(PercentSignToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~PercentSignToken() final override = default;
  };

  struct BitwiseToken: public BinaryOperatorToken
  {
    virtual ~BitwiseToken() override = default;
  };

  struct BitwiseAndToken final: public BitwiseToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "bitwise and";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 8;
    }

    constexpr auto operator<=>(BitwiseAndToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~BitwiseAndToken() final override = default;
  };

  struct BitwiseOrToken final: public BitwiseToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "bitwise or";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 6;
    }

    constexpr auto operator<=>(BitwiseOrToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~BitwiseOrToken() final override = default;
  };

  struct BitwiseXorToken final: public BitwiseToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "bitwise xor";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 7;
    }

    constexpr auto operator<=>(BitwiseXorToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~BitwiseXorToken() final override = default;
  };

  struct ShiftToken: public BinaryOperatorToken
  {
    virtual ~ShiftToken() override = default;
  };

  struct LeftShiftToken final: public ShiftToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "left shift";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 11;
    }

    constexpr auto operator<=>(LeftShiftToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~LeftShiftToken() final override = default;
  };

  struct RightShiftToken final: public ShiftToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "right shift";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 11;
    }

    constexpr auto operator<=>(RightShiftToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~RightShiftToken() final override = default;
  };

  class Token
  {
    std::shared_ptr<BasicToken> token{};

    public:
    constexpr Token() = default;

    explicit Token(std::shared_ptr<BasicToken> token): token{ token } {}

    Token(Token const &) = default;

    [[nodiscard]] constexpr bool operator==(Token const &other) const noexcept
    {
      return *token == *other.token;
    }

    Token &operator=(Token const &) = default;

    [[nodiscard]] constexpr std::string toString() const
    {
      return token->toString();
    }

    [[nodiscard]] constexpr bool isUnaryOperatorToken() const
    {
      return token->isUnaryOperatorToken();
    }

    [[nodiscard]] constexpr bool isBinaryOperatorToken() const
    {
      return token->isBinaryOperatorToken();
    }

    [[nodiscard]] constexpr std::size_t getPrecedence() const
    {
      return std::dynamic_pointer_cast<BinaryOperatorToken>(token)
        ->getPrecedence();
    }

    [[nodiscard]] bool isLiteralConstant() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<LiteralConstantToken>(token)
      );
    }

    [[nodiscard]] bool isTilde() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<TildeToken>(token));
    }

    [[nodiscard]] bool isHyphen() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<HyphenToken>(token));
    }

    [[nodiscard]] bool isDecrement() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<DecrementToken>(token)
      );
    }

    [[nodiscard]] bool isParenthesis() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<ParenthesisToken>(token
      ));
    }

    [[nodiscard]] bool isPlusSign() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<PlusSignToken>(token));
    }

    [[nodiscard]] bool isAsterisk() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<AsteriskToken>(token));
    }

    [[nodiscard]] bool isForwardSlash() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<ForwardSlashToken>(token)
      );
    }


    [[nodiscard]] bool isPercentSign() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<PercentSignToken>(token
      ));
    }

    [[nodiscard]] bool isBitwiseAnd() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<BitwiseAndToken>(token)
      );
    }

    [[nodiscard]] bool isBitwiseOr() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<BitwiseOrToken>(token)
      );
    }

    [[nodiscard]] bool isBitwiseXor() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<BitwiseXorToken>(token)
      );
    }

    [[nodiscard]] bool isLeftShift() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<LeftShiftToken>(token)
      );
    }

    [[nodiscard]] bool isRightShift() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<RightShiftToken>(token)
      );
    }

    [[nodiscard]] std::shared_ptr<IdentifierToken> getIdentifier() const;
    [[nodiscard]] std::shared_ptr<LiteralConstantToken>
                                                   getLiteralConstant() const;
    [[nodiscard]] std::shared_ptr<IntKeywordToken> getIntKeyword() const;
    [[nodiscard]] std::shared_ptr<ReturnKeywordToken> getReturnKeyword() const;
    [[nodiscard]] std::shared_ptr<VoidKeywordToken>   getVoidKeyword() const;
    [[nodiscard]] std::shared_ptr<ParenthesisToken> getLeftParenthesis() const;
    [[nodiscard]] std::shared_ptr<ParenthesisToken> getRightParenthesis() const;
    [[nodiscard]] std::shared_ptr<CurlyBraceToken>  getLeftCurlyBrace() const;
    [[nodiscard]] std::shared_ptr<CurlyBraceToken>  getRightCurlyBrace() const;
    [[nodiscard]] std::shared_ptr<SemicolonToken>   getSemicolon() const;
    [[nodiscard]] std::shared_ptr<TildeToken>       getTilde() const;
    [[nodiscard]] std::shared_ptr<HyphenToken>      getHyphen() const;
    [[nodiscard]] std::shared_ptr<DecrementToken>   getDecrement() const;
    [[nodiscard]] std::shared_ptr<PlusSignToken>    getPlusSign() const;
    [[nodiscard]] std::shared_ptr<AsteriskToken>    getAsterisk() const;
    [[nodiscard]] std::shared_ptr<ForwardSlashToken> getForwardSlash() const;
    [[nodiscard]] std::shared_ptr<PercentSignToken>  getPercentSign() const;
    [[nodiscard]] std::shared_ptr<BitwiseAndToken>   getBitwiseAnd() const;
    [[nodiscard]] std::shared_ptr<BitwiseOrToken>    getBitwiseOr() const;
    [[nodiscard]] std::shared_ptr<BitwiseXorToken>   getBitwiseXor() const;
    [[nodiscard]] std::shared_ptr<LeftShiftToken>    getLeftShift() const;
    [[nodiscard]] std::shared_ptr<RightShiftToken>   getRightShift() const;
  };

  class TokenConversionError final: public CompilerError
  {
    std::string const message{};
    Token const       source_token{};
    std::string const destination_token_type{};

    public:
    TokenConversionError(
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

    [[nodiscard]] Token getSourceToken() const noexcept { return source_token; }

    [[nodiscard]] constexpr std::string_view
    getDestinationTokenType() const noexcept
    {
      return destination_token_type;
    }

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~TokenConversionError() final override = default;
  };
} // namespace SC2

#endif
