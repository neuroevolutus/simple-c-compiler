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

    [[nodiscard]] virtual bool isPrefixUnaryOperatorToken() const noexcept
    {
      return false;
    }

    [[nodiscard]] virtual bool isPostfixUnaryOperatorToken() const noexcept
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

    virtual ~KeywordToken() override = default;
  };

  struct TypeToken: virtual public KeywordToken
  {
    virtual ~TypeToken() override = default;
  };

  class IntKeywordToken final
    : virtual public KeywordToken
    , public TypeToken
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

  class VoidKeywordToken final
    : virtual public KeywordToken
    , public TypeToken
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

  class TypedefKeywordToken final: public KeywordToken
  {
    protected:
    [[nodiscard]] virtual constexpr std::string
    getKeyword() const final override
    {
      return "typedef";
    }

    public:
    virtual ~TypedefKeywordToken() final override = default;
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
    virtual ~UnaryOperatorToken() override = default;
  };

  struct PrefixUnaryOperatorToken: public UnaryOperatorToken
  {
    [[nodiscard]] virtual constexpr bool
    isPrefixUnaryOperatorToken() const noexcept final override
    {
      return true;
    };

    virtual ~PrefixUnaryOperatorToken() = default;
  };

  struct TildeToken final: public PrefixUnaryOperatorToken
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
    : public PrefixUnaryOperatorToken
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

  struct PostfixUnaryOperatorToken: public UnaryOperatorToken
  {
    [[nodiscard]] virtual constexpr bool
    isPostfixUnaryOperatorToken() const noexcept final override
    {
      return true;
    };

    virtual ~PostfixUnaryOperatorToken() = default;
  };

  struct IncrementToken final
    : public PrefixUnaryOperatorToken
    , public PostfixUnaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "increment";
    }

    constexpr auto operator<=>(IncrementToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~IncrementToken() final override = default;
  };

  struct CommaToken final: public BasicToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "comma";
    }

    constexpr auto operator<=>(CommaToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~CommaToken() final override = default;
  };

  struct DecrementToken final
    : public PrefixUnaryOperatorToken
    , public PostfixUnaryOperatorToken
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

  struct ExclamationPointToken final: public PrefixUnaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "exclamation point";
    }

    constexpr auto operator<=>(ExclamationPointToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~ExclamationPointToken() final override = default;
  };

  struct DoubleAmpersandToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "double ampersand";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 5;
    }

    constexpr auto operator<=>(DoubleAmpersandToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~DoubleAmpersandToken() final override = default;
  };

  struct DoublePipeToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "double pipe";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 4;
    }

    constexpr auto operator<=>(DoublePipeToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~DoublePipeToken() final override = default;
  };

  struct EqualToToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "equal to";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 9;
    }

    constexpr auto operator<=>(EqualToToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~EqualToToken() final override = default;
  };

  struct NotEqualToToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "not equal to";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 9;
    }

    constexpr auto operator<=>(NotEqualToToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~NotEqualToToken() final override = default;
  };

  struct LessThanToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "less than";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 10;
    }

    constexpr auto operator<=>(LessThanToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~LessThanToken() final override = default;
  };

  struct GreaterThanToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "greater than";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 10;
    }

    constexpr auto operator<=>(GreaterThanToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~GreaterThanToken() final override = default;
  };

  struct LessThanOrEqualToToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "less than or equal to";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 10;
    }

    constexpr auto operator<=>(LessThanOrEqualToToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~LessThanOrEqualToToken() final override = default;
  };

  struct GreaterThanOrEqualToToken final: public BinaryOperatorToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "greater than or equal to";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 10;
    }

    constexpr auto operator<=>(GreaterThanOrEqualToToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~GreaterThanOrEqualToToken() final override = default;
  };

  struct BasicAssignmentToken: public BinaryOperatorToken
  {
    virtual ~BasicAssignmentToken() override = default;
  };

  struct AssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(AssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~AssignmentToken() final override = default;
  };

  struct AddAssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "add assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(AddAssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~AddAssignmentToken() final override = default;
  };

  struct SubtractAssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "subtract assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(SubtractAssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~SubtractAssignmentToken() final override = default;
  };

  struct MultiplyAssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "multiply assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(MultiplyAssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~MultiplyAssignmentToken() final override = default;
  };

  struct DivideAssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "divide assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(DivideAssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~DivideAssignmentToken() final override = default;
  };

  struct ModuloAssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "modulo assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(ModuloAssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~ModuloAssignmentToken() final override = default;
  };

  struct BitwiseAndAssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "bitwise and assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(BitwiseAndAssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~BitwiseAndAssignmentToken() final override = default;
  };

  struct BitwiseOrAssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "bitwise or assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(BitwiseOrAssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~BitwiseOrAssignmentToken() final override = default;
  };

  struct BitwiseXorAssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "bitwise xor assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(BitwiseXorAssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~BitwiseXorAssignmentToken() final override = default;
  };

  struct LeftShiftAssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "left shift assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(LeftShiftAssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~LeftShiftAssignmentToken() final override = default;
  };

  struct RightShiftAssignmentToken final: public BasicAssignmentToken
  {
    [[nodiscard]] virtual constexpr std::string
    toString() const noexcept final override
    {
      return "right shift assignment";
    }

    [[nodiscard]] virtual constexpr std::size_t
    getPrecedence() const noexcept final override
    {
      return 2;
    }

    constexpr auto operator<=>(RightShiftAssignmentToken const &) const
    {
      return std::strong_ordering::equal;
    }

    virtual ~RightShiftAssignmentToken() final override = default;
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

    [[nodiscard]] constexpr bool isPrefixUnaryOperatorToken() const
    {
      return token->isPrefixUnaryOperatorToken();
    }

    [[nodiscard]] constexpr bool isPostfixUnaryOperatorToken() const
    {
      return token->isPostfixUnaryOperatorToken();
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

    [[nodiscard]] bool isSemicolon() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<SemicolonToken>(token)
      );
    }

    [[nodiscard]] bool isRightCurlyBrace() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<RightCurlyBraceToken>(token)
      );
    }

    [[nodiscard]] bool isIdentifier() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<IdentifierToken>(token)
      );
    }

    [[nodiscard]] bool isType() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<TypeToken>(token));
    }

    [[nodiscard]] bool isKeyword() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<KeywordToken>(token));
    }

    [[nodiscard]] bool isIntKeyword() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<IntKeywordToken>(token)
      );
    }

    [[nodiscard]] bool isVoidKeyword() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<VoidKeywordToken>(token
      ));
    }

    [[nodiscard]] bool isReturnKeyword() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<ReturnKeywordToken>(token)
      );
    }

    [[nodiscard]] bool isTypedefKeyword() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<TypedefKeywordToken>(token)
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

    [[nodiscard]] bool isIncrement() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<IncrementToken>(token)
      );
    }

    [[nodiscard]] bool isParenthesis() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<ParenthesisToken>(token
      ));
    }

    [[nodiscard]] bool isRightParenthesis() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<RightParenthesisToken>(token)
      );
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

    [[nodiscard]] bool isExclamationPoint() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<ExclamationPointToken>(token)
      );
    }

    [[nodiscard]] bool isDoubleAmpersand() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<DoubleAmpersandToken>(token)
      );
    }

    [[nodiscard]] bool isDoublePipe() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<DoublePipeToken>(token)
      );
    }

    [[nodiscard]] bool isEqualTo() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<EqualToToken>(token));
    }

    [[nodiscard]] bool isNotEqualTo() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<NotEqualToToken>(token)
      );
    }

    [[nodiscard]] bool isLessThan() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<LessThanToken>(token));
    }

    [[nodiscard]] bool isGreaterThan() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<GreaterThanToken>(token
      ));
    }

    [[nodiscard]] bool isLessThanOrEqualTo() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<LessThanOrEqualToToken>(token)
      );
    }

    [[nodiscard]] bool isGreaterThanOrEqualTo() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<GreaterThanOrEqualToToken>(token)
      );
    }

    [[nodiscard]] bool isBasicAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<BasicAssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isAssignment() const noexcept
    {
      return static_cast<bool>(std::dynamic_pointer_cast<AssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isAddAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<AddAssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isSubtractAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<SubtractAssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isMultiplyAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<MultiplyAssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isDivideAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<DivideAssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isModuloAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<ModuloAssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isBitwiseAndAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<BitwiseAndAssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isBitwiseOrAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<BitwiseOrAssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isBitwiseXorAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<BitwiseXorAssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isLeftShiftAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<LeftShiftAssignmentToken>(token)
      );
    }

    [[nodiscard]] bool isRightShiftAssignment() const noexcept
    {
      return static_cast<bool>(
        std::dynamic_pointer_cast<RightShiftAssignmentToken>(token)
      );
    }

    [[nodiscard]] std::shared_ptr<IdentifierToken> getIdentifier() const;
    [[nodiscard]] std::shared_ptr<LiteralConstantToken>
                                                   getLiteralConstant() const;
    [[nodiscard]] std::shared_ptr<KeywordToken>    getKeyword() const;
    [[nodiscard]] std::shared_ptr<IntKeywordToken> getIntKeyword() const;
    [[nodiscard]] std::shared_ptr<ReturnKeywordToken> getReturnKeyword() const;
    [[nodiscard]] std::shared_ptr<VoidKeywordToken>   getVoidKeyword() const;
    [[nodiscard]] std::shared_ptr<TypedefKeywordToken>
                                                    getTypedefKeyword() const;
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
    [[nodiscard]] std::shared_ptr<ExclamationPointToken>
    getExclamationPoint() const;
    [[nodiscard]] std::shared_ptr<DoubleAmpersandToken>
                                                    getDoubleAmpersand() const;
    [[nodiscard]] std::shared_ptr<DoublePipeToken>  getDoublePipe() const;
    [[nodiscard]] std::shared_ptr<EqualToToken>     getEqualTo() const;
    [[nodiscard]] std::shared_ptr<NotEqualToToken>  getNotEqualTo() const;
    [[nodiscard]] std::shared_ptr<LessThanToken>    getLessThan() const;
    [[nodiscard]] std::shared_ptr<GreaterThanToken> getGreaterThan() const;
    [[nodiscard]] std::shared_ptr<LessThanOrEqualToToken>
    getLessThanOrEqualTo() const;
    [[nodiscard]] std::shared_ptr<GreaterThanOrEqualToToken>
    getGreaterThanOrEqualTo() const;
    [[nodiscard]] std::shared_ptr<AssignmentToken>    getAssignment() const;
    [[nodiscard]] std::shared_ptr<AddAssignmentToken> getAddAssignment() const;
    [[nodiscard]] std::shared_ptr<SubtractAssignmentToken>
    getSubtractAssignment() const;
    [[nodiscard]] std::shared_ptr<MultiplyAssignmentToken>
    getMultiplyAssignment() const;
    [[nodiscard]] std::shared_ptr<DivideAssignmentToken>
    getDivideAssignment() const;
    [[nodiscard]] std::shared_ptr<ModuloAssignmentToken>
    getModuloAssignment() const;
    [[nodiscard]] std::shared_ptr<BitwiseAndAssignmentToken>
    getBitwiseAndAssignment() const;
    [[nodiscard]] std::shared_ptr<BitwiseOrAssignmentToken>
    getBitwiseOrAssignment() const;
    [[nodiscard]] std::shared_ptr<BitwiseXorAssignmentToken>
    getBitwiseXorAssignment() const;
    [[nodiscard]] std::shared_ptr<LeftShiftAssignmentToken>
    getLeftShiftAssignment() const;
    [[nodiscard]] std::shared_ptr<RightShiftAssignmentToken>
    getRightShiftAssignment() const;
    [[nodiscard]] std::shared_ptr<IncrementToken> getIncrement() const;
    [[nodiscard]] std::shared_ptr<CommaToken>     getComma() const;
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
