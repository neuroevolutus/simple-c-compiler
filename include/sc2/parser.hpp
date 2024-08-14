#ifndef SC2_PARSER_HPP_INCLUDED
#define SC2_PARSER_HPP_INCLUDED

#include <sc2/ast.hpp>
#include <sc2/compiler_error.hpp>
#include <sc2/lexer.hpp>
#include <sc2/tokens.hpp>

#include <format>
#include <stdexcept>
#include <utility>
#include <vector>

namespace SC2 {

  struct ParserError: public CompilerError
  {
    virtual ~ParserError() = default;
  };

  class ParserNonTerminalError final: public ParserError
  {
    std::string const message{};

    public:
    ParserNonTerminalError(
      std::string_view   non_terminal,
      ParserError const &childError
    )
      : message{ std::format(
          "Parser error: invalid non-terminal <{}>:\n{}",
          non_terminal,
          childError.what()
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~ParserNonTerminalError() final override = default;
  };

  class ParserTokenCreationError final: public ParserError
  {
    std::string const message{};

    public:
    ParserTokenCreationError(TokenConversionError const &error)
      : message{ std::format(
          "Parser error: Cannot create ({}) from ({})",
          error.getDestinationTokenType(),
          error.getSourceToken().toString()
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~ParserTokenCreationError() final override = default;
  };

  class ParserTokenExpectationError final: public ParserError
  {
    std::string const message{};

    public:
    ParserTokenExpectationError(Token expected_token, Token actual_token)
      : message{ std::format(
          "Parser error: Expected ({}) but got ({})",
          expected_token.toString(),
          actual_token.toString()
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~ParserTokenExpectationError() final override = default;
  };

  struct ParserUnmatchedParenthesesError final: public ParserError
  {
    virtual constexpr char const *what() const noexcept final override
    {
      return "Parser error: unmatched parentheses";
    }

    virtual ~ParserUnmatchedParenthesesError() final override = default;
  };

  struct ParserEOFError final: public ParserError
  {
    static constexpr char const * const message{
      "Parser error: reached end of file"
    };

    virtual constexpr char const *what() const noexcept final override
    {
      return message;
    }

    virtual ~ParserEOFError() final override = default;
  };

  struct ParserExtraneousTokenError final: public ParserError
  {
    std::string const message{};

    public:
    ParserExtraneousTokenError(Token token)
      : message{
        std::format("Parser error: Extraneous token: ({})", token.toString())
      }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~ParserExtraneousTokenError() final override = default;
  };

  class ParserInvalidTokenError final: public ParserError
  {
    std::string const message{};

    public:
    ParserInvalidTokenError(LexerInvalidTokenError const &error)
      : message{ std::format("{}", error.what()) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~ParserInvalidTokenError() final override = default;
  };

  class Parser
  {
    std::exception_ptr current_exception{};
    std::vector<Token> tokens{};
    Lexer             &lexer;

    [[nodiscard]] Token peekNextToken()
    try {
      return *lexer;
    } catch (LexerEOFError const &) {
      throw ParserEOFError{};
    }

    // We have to do some gymnastics here so that we don't get confusing error
    // messages. We want a non-terminal to parse correctly even if the *next*
    // token following the non-terminal is invalid.
    [[nodiscard]] Token parseNextToken()
    try {
      if (current_exception) {
        try {
          std::rethrow_exception(current_exception);
        } catch (LexerInvalidTokenError const &error) {
          throw ParserInvalidTokenError(error);
        }
      }
      Token next_token{ *lexer };
      tokens.push_back(next_token);
      try {
        ++lexer;
      } catch (...) {
        current_exception = std::current_exception();
      }
      return next_token;
    } catch (LexerEOFError const &) {
      throw ParserEOFError{};
    }

    [[nodiscard]] std::shared_ptr<IdentifierToken> parseIdentifierToken()
    try {
      return parseNextToken().getIdentifier();
    } catch (TokenConversionError const &error) {
      throw ParserTokenCreationError(error);
    }

    [[nodiscard]] std::shared_ptr<LiteralConstantToken>
    parseLiteralConstantToken()
    try {
      return parseNextToken().getLiteralConstant();
    } catch (TokenConversionError const &error) {
      throw ParserTokenCreationError(error);
    }

    void expect(Token const &expected_token)
    {
      if (auto const actual_token{ parseNextToken() };
          expected_token != actual_token)
        throw ParserTokenExpectationError(expected_token, actual_token);
    }

    constexpr void expectFinished()
    {
      if (lexer != lexer.end()) throw ParserExtraneousTokenError(*lexer);
    }

    [[nodiscard]] std::shared_ptr<LiteralConstantASTNode>
    parseLiteralConstantExpression()
    {
      return std::make_shared<LiteralConstantASTNode>(
        parseLiteralConstantToken()->getValue()
      );
    }

    [[nodiscard]] std::shared_ptr<UnaryExpressionASTNode> parseUnaryExpression()
    {
      Token                                 next_token{ parseNextToken() };
      std::shared_ptr<UnaryOperatorASTNode> unary_operator{
        [&]() -> std::shared_ptr<UnaryOperatorASTNode> {
        if (next_token.isTilde())
          return std::make_shared<ComplementASTNode>();
        else
          return std::make_shared<NegateASTNode>();
      }()
      };
      std::shared_ptr<ExpressionASTNode> expression{ parseExpression() };
      return std::make_shared<UnaryExpressionASTNode>(
        unary_operator,
        expression
      );
    }

    [[nodiscard]] std::shared_ptr<ExpressionASTNode> parseExpression()
    try {
      Token nextToken{ peekNextToken() };
      if (nextToken.isLiteralConstant()) {
        return parseLiteralConstantExpression();
      } else if (nextToken.isTilde() || nextToken.isHyphen()) {
        return parseUnaryExpression();
      } else {
        expect(Token(std::make_shared<LeftParenthesisToken>()));
        std::shared_ptr<ExpressionASTNode> expression{ parseExpression() };
        try {
          expect(Token(std::make_shared<RightParenthesisToken>()));
        } catch (...) {
          throw ParserUnmatchedParenthesesError{};
        }
        return expression;
      }
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("expression", error);
    }

    [[nodiscard]] std::shared_ptr<StatementASTNode> parseStatement()
    try {
      expect(Token(std::make_shared<ReturnKeywordToken>()));
      auto const &expression{ parseExpression() };
      expect(Token(std::make_shared<SemicolonToken>()));
      return std::make_shared<StatementASTNode>(expression);
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("statement", error);
    }

    [[nodiscard]] std::shared_ptr<FunctionASTNode> parseFunction()
    try {
      expect(Token(std::make_shared<IntKeywordToken>()));
      std::shared_ptr<IdentifierToken> function_name{ parseIdentifierToken() };
      expect(Token(std::make_shared<LeftParenthesisToken>()));
      expect(Token(std::make_shared<VoidKeywordToken>()));
      expect(Token(std::make_shared<RightParenthesisToken>()));
      expect(Token(std::make_shared<LeftCurlyBraceToken>()));
      auto const &statement{ parseStatement() };
      expect(Token(std::make_shared<RightCurlyBraceToken>()));
      return std::make_shared<FunctionASTNode>(
        function_name->getName(),
        statement
      );
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("function", error);
    }

    public:
    Parser(Lexer &lexer): lexer{ lexer } {}

    [[nodiscard]] std::shared_ptr<ProgramASTNode> parseProgram()
    try {
      auto const program{ std::make_shared<ProgramASTNode>(parseFunction()) };
      expectFinished();
      return program;
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("program", error);
    }
  };
} // namespace SC2

#endif
