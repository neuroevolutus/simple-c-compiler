#include <sc2/parser.hpp>

namespace SC2 {
  [[nodiscard]] std::shared_ptr<ExpressionASTNode> Parser::parseFactor()
  try {
    Token const next_token{ peekNextToken() };
    if (next_token.isLiteralConstant()) {
      return parseLiteralConstantExpression();
    } else if (next_token.isUnaryOperatorToken()) {
      return parseUnaryExpression();
    } else {
      expect(Token(std::make_shared<LeftParenthesisToken>()));
      std::shared_ptr<ExpressionASTNode> const expression{ parseExpression(0) };
      try {
        expect(Token(std::make_shared<RightParenthesisToken>()));
      } catch (...) {
        throw ParserUnmatchedParenthesesError{};
      }
      return expression;
    }
  } catch (ParserError const &error) {
    throw ParserNonTerminalError("factor", error);
  }
} // namespace SC2
