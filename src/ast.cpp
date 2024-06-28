#include <sc2/ast.hpp>
#include <sc2/tacky_ast.hpp>
#include <string_view>

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace SC2 {
  [[nodiscard]] std::shared_ptr<UnaryOperatorTACKYASTNode>
  ComplementASTNode::emitTACKY() const
  {
    return std::make_shared<ComplementTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<UnaryOperatorTACKYASTNode> NegateASTNode::emitTACKY() const
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

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode> AddASTNode::emitTACKY() const
  {
    return std::make_shared<AddTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode> SubtractASTNode::emitTACKY() const
  {
    return std::make_shared<SubtractTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode> MultiplyASTNode::emitTACKY() const
  {
    return std::make_shared<MultiplyTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode> DivideASTNode::emitTACKY() const
  {
    return std::make_shared<DivideTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode> ModuloASTNode::emitTACKY() const
  {
    return std::make_shared<ModuloTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode> BitwiseAndASTNode::emitTACKY() const
  {
    return std::make_shared<BitwiseAndTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode> BitwiseOrASTNode::emitTACKY() const
  {
    return std::make_shared<BitwiseOrTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode> BitwiseXorASTNode::emitTACKY() const
  {
    return std::make_shared<BitwiseXorTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode> LeftShiftASTNode::emitTACKY() const
  {
    return std::make_shared<LeftShiftTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode> RightShiftASTNode::emitTACKY() const
  {
    return std::make_shared<RightShiftTACKYASTNode>();
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  BinaryExpressionASTNode::emitTACKY(ExpressionASTNodeEmitTACKYInput input
  ) const
  {
    auto const &[identifier, instructions]{ std::move(input) };
    auto [left_operand, left_operand_instructions]{ getLeftOperand()->emitTACKY(
      ExpressionASTNodeEmitTACKYInput{ identifier, std::move(instructions) }
    ) };
    auto [right_operand, right_operand_instructions]{
      getRightOperand()->emitTACKY(ExpressionASTNodeEmitTACKYInput{
        identifier,
        std::move(left_operand_instructions) })
    };
    auto destination{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifier(identifier)
    ) };
    right_operand_instructions.push_back(std::make_shared<BinaryTACKYASTNode>(
      getBinaryOperator()->emitTACKY(),
      left_operand,
      right_operand,
      destination
    ));
    return ExpressionASTNodeEmitTACKYOutput{
      destination,
      std::move(right_operand_instructions)
    };
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
