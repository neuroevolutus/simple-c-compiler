#include <sc2/ast.hpp>
#include <sc2/tacky_ast.hpp>
#include <string_view>

#include <format>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <utility>
#include <vector>

namespace SC2 {
  [[nodiscard]] bool TypeASTNode::operator==(TypeASTNode const &other) const
  {
    if (dynamic_cast<IntTypeASTNode const *>(this)
        && dynamic_cast<IntTypeASTNode const *>(&other)) {
      return true;
    } else if (dynamic_cast<VoidTypeASTNode const *>(this)
               && dynamic_cast<VoidTypeASTNode const *>(&other)) {
      return true;
    } else
      return false;
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
  VariableASTNode::emitTACKY(ExpressionASTNodeEmitTACKYInput &&input) const
  {
    auto const &[_, instructions]{ std::move(input) };
    return ExpressionASTNodeEmitTACKYOutput{
      std::make_shared<VariableTACKYASTNode>(getIdentifier()),
      std::move(instructions)
    };
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  emitDefaultTACKYForUnaryOperatorASTNode(
    std::shared_ptr<UnaryOperatorTACKYASTNode> unary_operator,
    UnaryOperatorASTNodeEmitTACKYInput       &&input
  )
  {
    auto const &[identifier, expression, instructions]{ std::move(input) };
    auto [source, new_instructions]{ expression->emitTACKY(
      ExpressionASTNodeEmitTACKYInput{ identifier, std::move(instructions) }
    ) };
    auto destination{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifierWithPrefix(identifier)
    ) };
    new_instructions.push_back(
      std::make_shared<UnaryTACKYASTNode>(unary_operator, source, destination)
    );
    return ExpressionASTNodeEmitTACKYOutput{ destination,
                                             std::move(new_instructions) };
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  ComplementASTNode::emitTACKY(UnaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForUnaryOperatorASTNode(
      std::make_shared<ComplementTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  NegateASTNode::emitTACKY(UnaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForUnaryOperatorASTNode(
      std::make_shared<NegateTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  NotASTNode::emitTACKY(UnaryOperatorASTNodeEmitTACKYInput &&input) const
  {
    return emitDefaultTACKYForUnaryOperatorASTNode(
      std::make_shared<NotTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  PrefixAddSubtractASTNode::emitTACKY(UnaryOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    auto const &[identifier, source_ast, instructions]{ std::move(input) };
    auto [source, new_instructions]{ source_ast->emitTACKY(
      ExpressionASTNodeEmitTACKYInput{ identifier, std::move(instructions) }
    ) };
    auto const &source_tacky{
      std::dynamic_pointer_cast<VariableTACKYASTNode>(source)
    };
    auto destination{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifierWithPrefix(identifier)
    ) };
    new_instructions.push_back(std::make_shared<BinaryTACKYASTNode>(
      emitBinaryOperatorTACKYASTNode(),
      source_tacky,
      std::make_shared<LiteralConstantTACKYASTNode>(1),
      source_tacky
    ));
    return ExpressionASTNodeEmitTACKYOutput{ source_tacky,
                                             std::move(new_instructions) };
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode>
  PrefixIncrementASTNode::emitBinaryOperatorTACKYASTNode() const
  {
    return std::make_shared<AddTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode>
  PrefixDecrementASTNode::emitBinaryOperatorTACKYASTNode() const
  {
    return std::make_shared<SubtractTACKYASTNode>();
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  PostfixAddSubtractASTNode::emitTACKY(
    UnaryOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    auto const &[identifier, expression, instructions]{ std::move(input) };
    auto [source, new_instructions]{ expression->emitTACKY(
      ExpressionASTNodeEmitTACKYInput{ identifier, std::move(instructions) }
    ) };
    auto const &source_tacky{
      std::dynamic_pointer_cast<VariableTACKYASTNode>(source)
    };
    auto temporary{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifierWithPrefix(identifier)
    ) };
    new_instructions.push_back(
      std::make_shared<CopyTACKYASTNode>(source_tacky, temporary)
    );
    new_instructions.push_back(std::make_shared<BinaryTACKYASTNode>(
      emitBinaryOperatorTACKYASTNode(),
      source_tacky,
      std::make_shared<LiteralConstantTACKYASTNode>(1),
      source_tacky
    ));
    return ExpressionASTNodeEmitTACKYOutput{ temporary,
                                             std::move(new_instructions) };
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode>
  PostfixIncrementASTNode::emitBinaryOperatorTACKYASTNode() const
  {
    return std::make_shared<AddTACKYASTNode>();
  }

  [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode>
  PostfixDecrementASTNode::emitBinaryOperatorTACKYASTNode() const
  {
    return std::make_shared<SubtractTACKYASTNode>();
  }

  struct EmitDefaultBinaryOperatorTACKYInput
  {
    std::string                                           identifier{};
    std::shared_ptr<BinaryOperatorTACKYASTNode>           binary_operator;
    std::shared_ptr<ValueTACKYASTNode>                    left_operand{};
    std::shared_ptr<ValueTACKYASTNode>                    right_operand{};
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions{};
  };

  [[nodiscard]] static ExpressionASTNodeEmitTACKYOutput
  emitDefaultBinaryOperatorTACKY(EmitDefaultBinaryOperatorTACKYInput &&input)
  {
    auto
      [identifier, binary_operator, left_operand, right_operand, instructions]{
        std::move(input)
      };
    auto destination{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifierWithPrefix(identifier)
    ) };
    instructions.push_back(std::make_shared<BinaryTACKYASTNode>(
      binary_operator,
      left_operand,
      right_operand,
      destination
    ));
    return ExpressionASTNodeEmitTACKYOutput{ destination,
                                             std::move(instructions) };
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
    return emitDefaultBinaryOperatorTACKY(EmitDefaultBinaryOperatorTACKYInput{
      identifier,
      std::move(binary_operator),
      std::move(left_operand),
      std::move(right_operand),
      std::move(right_operand_instructions) });
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

  [[nodiscard]] static ExpressionASTNodeEmitTACKYOutput
  emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
    std::shared_ptr<BinaryOperatorTACKYASTNode>    binary_operator,
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  )
  {
    auto const &[identifier, variable, expression, instructions]{
      std::move(input)
    };
    auto const &[source_tacky, new_instructions_zero]{
      expression->emitTACKY({ identifier, std::move(instructions) })
    };
    auto [destination_tacky, new_instructions_one]{
      variable->emitTACKY({ identifier, std::move(new_instructions_zero) })
    };
    auto [temp_destination_tacky, final_instructions]{
      emitDefaultBinaryOperatorTACKY(EmitDefaultBinaryOperatorTACKYInput{
        identifier,
        std::move(binary_operator),
        destination_tacky,
        std::move(source_tacky),
        std::move(new_instructions_one) })
    };
    final_instructions.push_back(std::make_shared<CopyTACKYASTNode>(
      temp_destination_tacky,
      std::dynamic_pointer_cast<VariableTACKYASTNode>(destination_tacky)
    ));
    return { destination_tacky, std::move(final_instructions) };
  }

  [[nodiscard]] static ExpressionASTNodeEmitTACKYOutput
  emitTACKYForAssignment(BasicAssignmentOperatorASTNodeEmitTACKYInput &&input)
  {
    auto const &[identifier, variable, expression, instructions]{
      std::move(input)
    };
    auto const &[source_tacky, new_instructions_zero]{
      expression->emitTACKY({ identifier, std::move(instructions) })
    };
    auto [destination_tacky, new_instructions_one]{
      variable->emitTACKY({ identifier, std::move(new_instructions_zero) })
    };
    new_instructions_one.push_back(std::make_shared<CopyTACKYASTNode>(
      source_tacky,
      std::dynamic_pointer_cast<VariableTACKYASTNode>(destination_tacky)
    ));
    return { destination_tacky, std::move(new_instructions_one) };
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  AssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitTACKYForAssignment(std::move(input));
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  AddAssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
      std::make_shared<AddTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  DivideAssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
      std::make_shared<DivideTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  ModuloAssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
      std::make_shared<ModuloTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  MultiplyAssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
      std::make_shared<MultiplyTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  SubtractAssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
      std::make_shared<SubtractTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  BitwiseOrAssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
      std::make_shared<BitwiseOrTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  BitwiseAndAssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
      std::make_shared<BitwiseAndTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  BitwiseXorAssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
      std::make_shared<BitwiseXorTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  LeftShiftAssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
      std::make_shared<LeftShiftTACKYASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
  RightShiftAssignmentOperatorASTNode::emitTACKY(
    BasicAssignmentOperatorASTNodeEmitTACKYInput &&input
  ) const
  {
    return emitDefaultTACKYForCompoundAssignmentOperatorASTNode(
      std::make_shared<RightShiftTACKYASTNode>(),
      std::move(input)
    );
  }

  std::vector<std::shared_ptr<InstructionTACKYASTNode>>
  ReturnStatementASTNode::emitTACKY(std::string_view identifier) const
  {
    auto [value, instructions]{
      getExpression()->emitTACKY(ExpressionASTNodeEmitTACKYInput{
        std::string{ identifier },
        std::vector<std::shared_ptr<InstructionTACKYASTNode>>{} })
    };
    instructions.push_back(std::make_shared<ReturnTACKYASTNode>(value));
    return instructions;
  }

  std::vector<std::shared_ptr<InstructionTACKYASTNode>>
  ExpressionStatementASTNode::emitTACKY(std::string_view identifier) const
  {
    auto [_, instructions]{
      getExpression()->emitTACKY(ExpressionASTNodeEmitTACKYInput{
        std::string{ identifier },
        std::vector<std::shared_ptr<InstructionTACKYASTNode>>{} })
    };
    return instructions;
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionTACKYASTNode>>
  DeclarationASTNode::emitTACKY(std::string_view identifier) const
  {
    if (initializer) {
      auto [_, instructions]{
        emitTACKYForAssignment(BasicAssignmentOperatorASTNodeEmitTACKYInput{
          std::string{ identifier },
          std::make_shared<VariableASTNode>(getIdentifier()),
          getInitializer(),
          std::vector<std::shared_ptr<InstructionTACKYASTNode>>() })
      };
      return instructions;
    } else {
      return std::vector<std::shared_ptr<InstructionTACKYASTNode>>();
    }
  }

  std::shared_ptr<FunctionTACKYASTNode> FunctionASTNode::emitTACKY()
  {
    auto tacky_block_items{
      getBlockItems()
      | std::views::transform(
        [this](std::shared_ptr<BlockItemASTNode> block_item) {
      return block_item->emitTACKY(getIdentifier());
    }
      )
      | std::views::join | std::ranges::to<std::vector>()
    };
    tacky_block_items.push_back(std::make_shared<ReturnTACKYASTNode>(
      std::make_shared<LiteralConstantTACKYASTNode>(0)
    ));
    return std::make_shared<FunctionTACKYASTNode>(
      getIdentifier(),
      tacky_block_items
    );
  }

  std::shared_ptr<ProgramTACKYASTNode> ProgramASTNode::emitTACKY() const
  {
    return std::make_shared<ProgramTACKYASTNode>(getFunction()->emitTACKY());
  }
} // namespace SC2
