#ifndef SC2_PARSER_HPP_INCLUDED
#define SC2_PARSER_HPP_INCLUDED

#include <sc2/ast.hpp>
#include <sc2/lexer.hpp>
#include <sc2/tokens.hpp>

#include <format>
#include <stdexcept>
#include <utility>
#include <vector>

namespace SC2 {

  struct ParserError: public std::exception
  {
    virtual ~ParserError() = default;
  };

  class ParserNonTerminalError: public ParserError
  {
    std::string const message{};

    public:
    // TODO: Make sure only single argument constructors are explicit
    constexpr ParserNonTerminalError(
      std::string_view   non_terminal,
      ParserError const &childError
    )
      : message{ std::format(
          "Parser error: invalid non-terminal <{}>:\n{}",
          non_terminal,
          childError.what()
        ) }
    {}
    virtual constexpr char const *what() const noexcept override
    {
      return message.c_str();
    }
    virtual ~ParserNonTerminalError() override = default;
  };

  class ParserTokenCreationError: public ParserError
  {
    std::string const message{};

    public:
    constexpr ParserTokenCreationError(TokenConversionError const &error)
      : message{ std::format(
          "Parser error: Cannot create ({}) from ({})",
          error.getDestinationTokenType(),
          error.getSourceToken().toString()
        ) }
    {}
    virtual constexpr char const *what() const noexcept override
    {
      return message.c_str();
    }
    virtual ~ParserTokenCreationError() override = default;
  };

  class ParserTokenExpectationError: public ParserError
  {
    std::string const message{};

    public:
    constexpr ParserTokenExpectationError(
      Token expected_token,
      Token actual_token
    )
      : message{ std::format(
          "Parser error: Expected ({}) but got ({})",
          expected_token.toString(),
          actual_token.toString()
        ) }
    {}
    virtual constexpr char const *what() const noexcept override
    {
      return message.c_str();
    }
    virtual ~ParserTokenExpectationError() override = default;
  };

  struct ParserEOFError: public ParserError
  {
    static constexpr char const * const message{
      "Parser error: reached end of file"
    };
    virtual constexpr char const *what() const noexcept { return message; }
    virtual ~ParserEOFError() override = default;
  };

  struct ParserExtraneousTokenError: public ParserError
  {
    std::string const message{};

    public:
    constexpr ParserExtraneousTokenError(Token token)
      : message{
        std::format("Parser error: Extraneous token: ({})", token.toString())
      }
    {}
    virtual constexpr char const *what() const noexcept
    {
      return message.c_str();
    }
    virtual ~ParserExtraneousTokenError() override = default;
  };

  class ParserInvalidTokenError: public ParserError
  {
    std::string const message{};

    public:
    constexpr ParserInvalidTokenError(LexerInvalidTokenError const &error)
      : message{ std::format("{}", error.what()) }
    {}
    virtual constexpr char const *what() const noexcept
    {
      return message.c_str();
    }
    virtual ~ParserInvalidTokenError() = default;
  };

  class Parser
  {
    std::exception_ptr current_exception{};
    std::vector<Token> tokens{};
    Lexer             &lexer;

    // We have to do some gymnastics here so that we don't get confusing error
    // messages. We want a non-terminal to parse correctly even if the *next*
    // token following the non-terminal is invalid.
    constexpr void parseNextToken() noexcept(false)
    try {
      if (current_exception) {
        try {
          std::rethrow_exception(current_exception);
        } catch (LexerInvalidTokenError const &error) {
          throw ParserInvalidTokenError(error);
        }
      }
      tokens.push_back(*lexer);
      try {
        ++lexer;
      } catch (...) {
        current_exception = std::current_exception();
      }
    } catch (LexerEOFError const &) {
      throw ParserEOFError{};
    }

    constexpr Identifier parseIdentifier() noexcept(false)
    try {
      parseNextToken();
      return tokens.back().getIdentifier();
    } catch (TokenConversionError const &error) {
      throw ParserTokenCreationError(error);
    }

    constexpr LiteralConstant parseLiteralConstant() noexcept(false)
    try {
      parseNextToken();
      return tokens.back().getLiteralConstant();
    } catch (TokenConversionError const &error) {
      throw ParserTokenCreationError(error);
    }

    constexpr void expect(Token const &expected_token) noexcept(false)
    {
      parseNextToken();
      if (auto const actual_token{ tokens.back() };
          expected_token != actual_token)
        throw ParserTokenExpectationError(expected_token, actual_token);
    }

    constexpr void expectFinished() noexcept(false)
    {
      if (lexer != lexer.end()) throw ParserExtraneousTokenError(*lexer);
    }

    [[nodiscard]] constexpr std::shared_ptr<Expression>
    parseExpression() noexcept(false)
    try {
      LiteralConstant const literalConstant{ parseLiteralConstant() };
      return std::make_shared<Expression>(literalConstant);
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("expression", error);
    }

    [[nodiscard]] constexpr std::shared_ptr<Statement>
    parseStatement() noexcept(false)
    try {
      expect(Token(Keyword::RETURN));
      auto const &expression{ parseExpression() };
      expect(Token(Semicolon));
      return std::make_shared<Statement>(expression);
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("statement", error);
    }

    [[nodiscard]] constexpr std::shared_ptr<Function>
    parseFunction() noexcept(false)
    try {
      expect(Token(Keyword::INT));
      Identifier const function_name{ parseIdentifier() };
      expect(Token(Parenthesis::LEFT_PARENTHESIS));
      expect(Token(Keyword::VOID));
      expect(Token(Parenthesis::RIGHT_PARENTHESIS));
      expect(Token(Brace::LEFT_BRACE));
      auto const &statement{ parseStatement() };
      expect(Token(Brace::RIGHT_BRACE));
      return std::make_shared<Function>(function_name, statement);
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("function", error);
    }

    public:
    constexpr Parser(Lexer &lexer): lexer{ lexer } {}

    [[nodiscard]] constexpr std::shared_ptr<Program>
    parseProgram() noexcept(false)
    try {
      auto const program{ std::make_shared<Program>(parseFunction()) };
      expectFinished();
      return program;
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("program", error);
    }
  };
} // namespace SC2

#endif
