#include <sc2/ast.hpp>
#include <sc2/tacky_ast.hpp>
#include <string_view>

#include <format>
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

  [[nodiscard]] std::shared_ptr<UnaryOperatorTACKYASTNode>
  NegateASTNode::emitTACKY() const
  {
    return std::make_shared<NegateTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<UnaryOperatorTACKYASTNode>
  NotASTNode::emitTACKY() const
  {
    return std::make_shared<NotTACKYASTNode>();
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  LiteralConstantASTNode::emitTACKY(ExpressionASTNodeEmitTACKYInput &&input
  ) const
  {
    auto const &[_, instructions]{ std::move(input) };
    return ExpressionASTNodeEmitTACKYOutput{
      std::make_shared<LiteralConstantTACKYASTNode>(getValue()),
      std::move(instructions)
    };
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  UnaryExpressionASTNode::emitTACKY(ExpressionASTNodeEmitTACKYInput &&input
  ) const
  {
    auto const &[identifier, instructions]{ std::move(input) };
    auto [source, new_instructions]{ getExpression()->emitTACKY(
      ExpressionASTNodeEmitTACKYInput{ identifier, std::move(instructions) }
    ) };
    auto destination{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifierWithPrefix(identifier)
    ) };
    new_instructions.push_back(std::make_shared<UnaryTACKYASTNode>(
      getUnaryOperator()->emitTACKY(),
      source,
      destination
    ));
    return ExpressionASTNodeEmitTACKYOutput{ destination,
                                             std::move(new_instructions) };
  }

  [[nodiscard]] static ExpressionASTNodeEmitTACKYOutput
  emitDefaultTACKYForBinaryOperatorASTNode(
    std::shared_ptr<BinaryOperatorTACKYASTNode> binary_operator,
    BinaryOperatorASTNodeEmitTACKYInput       &&input
  )
  {
    auto const &[identifier, left_operand_ast, right_operand_ast, instructions]{
      std::move(input)
    };
    auto [left_operand, left_operand_instructions]{ left_operand_ast->emitTACKY(
      ExpressionASTNodeEmitTACKYInput{ identifier, std::move(instructions) }
    ) };
    auto [right_operand, right_operand_instructions]{
      right_operand_ast->emitTACKY(ExpressionASTNodeEmitTACKYInput{
        identifier,
        std::move(left_operand_instructions) })
    };
    auto destination{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifierWithPrefix(identifier)
    ) };
    right_operand_instructions.push_back(std::make_shared<BinaryTACKYASTNode>(
      binary_operator,
      left_operand,
      right_operand,
      destination
    ));
    return ExpressionASTNodeEmitTACKYOutput{
      destination,
      std::move(right_operand_instructions)
    };
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  AddASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<AddTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  SubtractASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<SubtractTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  MultiplyASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<MultiplyTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  DivideASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<DivideTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  ModuloASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<ModuloTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  BitwiseAndASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<BitwiseAndTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  BitwiseOrASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<BitwiseOrTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  BitwiseXorASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<BitwiseXorTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  LeftShiftASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<LeftShiftTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  RightShiftASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<RightShiftTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  AndASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    auto const &[identifier, left_operand_ast, right_operand_ast, instructions]{
      std::move(input)
    };
    auto [left_operand, left_operand_instructions]{ left_operand_ast->emitTACKY(
      ExpressionASTNodeEmitTACKYInput{ identifier, std::move(instructions) }
    ) };
    auto const &false_label{ std::format(
      "{}_{}",
      Utility::generateFreshIdentifierWithPrefix(identifier),
      "false_label"
    ) };
    left_operand_instructions.push_back(
      std::make_shared<JumpIfZeroTACKYASTNode>(left_operand, false_label)
    );
    auto [right_operand, right_operand_instructions]{
      right_operand_ast->emitTACKY(ExpressionASTNodeEmitTACKYInput{
        identifier,
        std::move(left_operand_instructions) })
    };
    auto destination{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifierWithPrefix(identifier)
    ) };
    right_operand_instructions.push_back(
      std::make_shared<JumpIfZeroTACKYASTNode>(right_operand, false_label)
    );
    right_operand_instructions.push_back(std::make_shared<CopyTACKYASTNode>(
      std::make_shared<LiteralConstantTACKYASTNode>(1),
      destination
    ));
    auto const &end_label{ Utility::generateFreshIdentifierWithPrefix(identifier
    ) };
    right_operand_instructions.push_back(
      std::make_shared<JumpTACKYASTNode>(end_label)
    );
    right_operand_instructions.push_back(
      std::make_shared<LabelTACKYASTNode>(false_label)
    );
    right_operand_instructions.push_back(std::make_shared<CopyTACKYASTNode>(
      std::make_shared<LiteralConstantTACKYASTNode>(0),
      destination
    ));
    right_operand_instructions.push_back(
      std::make_shared<LabelTACKYASTNode>(end_label)
    );
    return ExpressionASTNodeEmitTACKYOutput{
      destination,
      std::move(right_operand_instructions)
    };
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  OrASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    auto const &[identifier, left_operand_ast, right_operand_ast, instructions]{
      std::move(input)
    };
    auto [left_operand, left_operand_instructions]{ left_operand_ast->emitTACKY(
      ExpressionASTNodeEmitTACKYInput{ identifier, std::move(instructions) }
    ) };
    auto const &false_label{ std::format(
      "{}_{}",
      Utility::generateFreshIdentifierWithPrefix(identifier),
      "false_label"
    ) };
    left_operand_instructions.push_back(
      std::make_shared<JumpIfNotZeroTACKYASTNode>(left_operand, false_label)
    );
    auto [right_operand, right_operand_instructions]{
      right_operand_ast->emitTACKY(ExpressionASTNodeEmitTACKYInput{
        identifier,
        std::move(left_operand_instructions) })
    };
    auto destination{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifierWithPrefix(identifier)
    ) };
    right_operand_instructions.push_back(
      std::make_shared<JumpIfNotZeroTACKYASTNode>(right_operand, false_label)
    );
    right_operand_instructions.push_back(std::make_shared<CopyTACKYASTNode>(
      std::make_shared<LiteralConstantTACKYASTNode>(0),
      destination
    ));
    auto const &end_label{ Utility::generateFreshIdentifierWithPrefix(identifier
    ) };
    right_operand_instructions.push_back(
      std::make_shared<JumpTACKYASTNode>(end_label)
    );
    right_operand_instructions.push_back(
      std::make_shared<LabelTACKYASTNode>(false_label)
    );
    right_operand_instructions.push_back(std::make_shared<CopyTACKYASTNode>(
      std::make_shared<LiteralConstantTACKYASTNode>(1),
      destination
    ));
    right_operand_instructions.push_back(
      std::make_shared<LabelTACKYASTNode>(end_label)
    );
    return ExpressionASTNodeEmitTACKYOutput{
      destination,
      std::move(right_operand_instructions)
    };
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  EqualsASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<EqualsTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  NotEqualsASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<NotEqualsTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  LessThanASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<LessThanTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  GreaterThanASTNode::emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<GreaterThanTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  LessThanOrEqualToASTNode::emitTACKY(
    BinaryOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<LessThanOrEqualToTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  GreaterThanOrEqualToASTNode::emitTACKY(
    BinaryOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForBinaryOperatorASTNode(
      std::make_shared<GreaterThanOrEqualToTACKYASTNode>(),
      std::move(input)
    );
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
