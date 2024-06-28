#include <sc2/ast.hpp>
#include <sc2/tacky_ast.hpp>
#include <string_view>

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace SC2 {
  std::shared_ptr<UnaryOperatorTACKYASTNode>
  ComplementASTNode::emitTACKY() const
  {
    return std::make_shared<ComplementTACKYASTNode>();
  }

  std::shared_ptr<UnaryOperatorTACKYASTNode> NegateASTNode::emitTACKY() const
  {
    return std::make_shared<NegateTACKYASTNode>();
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  LiteralConstantASTNode::emitTACKY(ExpressionASTNodeEmitTACKYInput input) const
  {
    auto const &[_, instructions]{ std::move(input) };
    return ExpressionASTNodeEmitTACKYOutput{
      std::make_shared<LiteralConstantTACKYASTNode>(getValue()),
      std::move(instructions)
    };
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  UnaryExpressionASTNode::emitTACKY(ExpressionASTNodeEmitTACKYInput input) const
  {
    auto const &[identifier, instructions]{ std::move(input) };
    auto [source, new_instructions]{ getExpression()->emitTACKY(
      ExpressionASTNodeEmitTACKYInput{ identifier, std::move(instructions) }
    ) };
    auto destination{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifier(identifier)
    ) };
    new_instructions.push_back(std::make_shared<UnaryTACKYASTNode>(
      getUnaryOperator()->emitTACKY(),
      source,
      destination
    ));
    return ExpressionASTNodeEmitTACKYOutput{ destination,
                                             std::move(new_instructions) };
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  BinaryExpressionASTNode::emitTACKY(ExpressionASTNodeEmitTACKYInput) const
  {
    throw std::runtime_error("Not yet implemented");
  }

  std::vector<std::shared_ptr<InstructionTACKYASTNode>>
  StatementASTNode::emitTACKY(std::string_view identifier) const
  {
    auto [value, instructions]{
      getExpression()->emitTACKY(ExpressionASTNodeEmitTACKYInput{
        identifier,
        std::vector<std::shared_ptr<InstructionTACKYASTNode>>{} })
    };
    instructions.push_back(std::make_shared<ReturnTACKYASTNode>(value));
    return instructions;
  }

  std::shared_ptr<FunctionTACKYASTNode> FunctionASTNode::emitTACKY() const
  {
    return std::make_shared<FunctionTACKYASTNode>(
      getIdentifier(),
      getStatement()->emitTACKY(getIdentifier())
    );
  }

  std::shared_ptr<ProgramTACKYASTNode> ProgramASTNode::emitTACKY() const
  {
    return std::make_shared<ProgramTACKYASTNode>(getFunction()->emitTACKY());
  }
} // namespace SC2
