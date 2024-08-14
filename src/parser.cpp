#include <sc2/parser.hpp>

namespace SC2 {
  [[nodiscard]] std::shared_ptr<ExpressionASTNode>
  Parser::parseFactor(VariableToTypeAndUniqueIdentifierMap &map)
  try {
    std::shared_ptr<ExpressionASTNode> expression{
      [this, &map]() -> std::shared_ptr<ExpressionASTNode> {
      Token const next_token{ peekNextToken() };
      if (next_token.isLiteralConstant()) {
        return parseLiteralConstantExpression();
      } else if (next_token.isPrefixUnaryOperatorToken()) {
        std::shared_ptr<PrefixUnaryOperatorASTNode> const prefix_unary_operator{
          parsePrefixUnaryOperator()
        };
        std::shared_ptr<ExpressionASTNode> const factor{ parseFactor(map) };
        if ((std::dynamic_pointer_cast<PrefixDecrementASTNode>(
               prefix_unary_operator
             )
             || std::dynamic_pointer_cast<PrefixIncrementASTNode>(
               prefix_unary_operator
             ))
            && !std::dynamic_pointer_cast<VariableASTNode>(factor))
          throw InvalidLValueError(factor);
        else
          return std::make_shared<UnaryExpressionASTNode>(
            prefix_unary_operator,
            factor
          );
      } else if (next_token.isIdentifier()) {
        return parseVariable(map);
      } else {
        expect(Token(std::make_shared<LeftParenthesisToken>()));
        std::shared_ptr<ExpressionASTNode> const expression{
          parseExpression(0, map)
        };
        try {
          expect(Token(std::make_shared<RightParenthesisToken>()));
        } catch (...) {
          throw ParserUnmatchedParenthesesError{};
        }
        return expression;
      }
    }()
    };
    if (Token next_token{ peekNextToken() };
        next_token.isPostfixUnaryOperatorToken()) {
      if (!std::dynamic_pointer_cast<VariableASTNode>(expression))
        throw InvalidLValueError(expression);
      expression = std::make_shared<UnaryExpressionASTNode>(
        parsePostfixUnaryOperator(),
        expression
      );
    }
    return expression;
  } catch (ParserError const &error) {
    throw ParserNonTerminalError("factor", error);
  }
} // namespace SC2
