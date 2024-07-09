#include <sc2/assembly_ast.hpp>
#include <sc2/tacky_ast.hpp>

#include <memory>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace SC2 {
  [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode>
  LiteralConstantTACKYASTNode::emitAssembly() const
  {
    return std::make_shared<ImmediateValueAssemblyASTNode>(getValue());
  }

  [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode>
  VariableTACKYASTNode::emitAssembly() const
  {
    return std::make_shared<PseudoRegisterAssemblyASTNode>(getIdentifier());
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  ReturnTACKYASTNode::emitAssembly() const
  {
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<MovlAssemblyASTNode>(
        getValue()->emitAssembly(),
        std::make_shared<EAXRegisterAssemblyASTNode>()
      ),
      std::make_shared<ReturnAssemblyASTNode>()
    };
  }

  [[nodiscard]] static std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  emitDefaultAssemblyForUnaryOperatorTACKYASTNode(
    std::shared_ptr<UnaryOperatorAssemblyASTNode> unary_operator,
    UnaryOperatorTACKYASTNodeEmitAssemblyInput  &&input
  )
  {
    auto const &[source, destination]{ std::move(input) };
    auto const &destination_assembly{ destination->emitAssembly() };
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<MovlAssemblyASTNode>(
        source->emitAssembly(),
        destination_assembly
      ),
      std::make_shared<UnaryAssemblyASTNode>(
        unary_operator,
        destination_assembly
      )
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  ComplementTACKYASTNode::emitAssembly(
    UnaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForUnaryOperatorTACKYASTNode(
      std::make_shared<ComplementAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  NegateTACKYASTNode::emitAssembly(
    UnaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForUnaryOperatorTACKYASTNode(
      std::make_shared<NegateAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  NotTACKYASTNode::emitAssembly(
    UnaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    auto const &[source, destination]{ std::move(input) };
    auto const &destination_assembly{ destination->emitAssembly() };
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<CmpAssemblyASTNode>(
        std::make_shared<ImmediateValueAssemblyASTNode>(0),
        source->emitAssembly()
      ),
      std::make_shared<MovlAssemblyASTNode>(
        std::make_shared<ImmediateValueAssemblyASTNode>(0),
        destination_assembly
      ),
      std::make_shared<SetCCAssemblyASTNode>(
        std::make_shared<ECondCodeAssemblyASTNode>(),
        destination_assembly
      )
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  UnaryTACKYASTNode::emitAssembly() const
  {
    return getUnaryOperator()->emitAssembly({ getSource(), getDestination() });
  }

  [[nodiscard]] static std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  emitDefaultAssemblyForBinaryOperatorTACKYASTNode(
    std::shared_ptr<BinaryOperatorAssemblyASTNode> binary_operator,
    BinaryOperatorTACKYASTNodeEmitAssemblyInput  &&input
  )
  {
    auto const &[left_operand, right_operand, destination]{ std::move(input) };
    std::shared_ptr<OperandAssemblyASTNode> const destination_assembly{
      destination->emitAssembly()
    };
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<MovlAssemblyASTNode>(
        left_operand->emitAssembly(),
        destination_assembly
      ),
      std::make_shared<BinaryAssemblyASTNode>(
        binary_operator,
        right_operand->emitAssembly(),
        destination_assembly
      )
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  AddTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperatorTACKYASTNode(
      std::make_shared<AddAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  SubtractTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperatorTACKYASTNode(
      std::make_shared<SubtractAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  MultiplyTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperatorTACKYASTNode(
      std::make_shared<MultiplyAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  DivideTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    auto const &[left_operand, right_operand, destination]{ std::move(input) };
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<MovlAssemblyASTNode>(
        left_operand->emitAssembly(),
        std::make_shared<EAXRegisterAssemblyASTNode>()
      ),
      std::make_shared<CdqAssemblyASTNode>(),
      std::make_shared<IdivAssemblyASTNode>(right_operand->emitAssembly()),
      std::make_shared<MovlAssemblyASTNode>(
        std::make_shared<EAXRegisterAssemblyASTNode>(),
        destination->emitAssembly()
      )
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  ModuloTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    auto const &[left_operand, right_operand, destination]{ std::move(input) };
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<MovlAssemblyASTNode>(
        left_operand->emitAssembly(),
        std::make_shared<EAXRegisterAssemblyASTNode>()
      ),
      std::make_shared<CdqAssemblyASTNode>(),
      std::make_shared<IdivAssemblyASTNode>(right_operand->emitAssembly()),
      std::make_shared<MovlAssemblyASTNode>(
        std::make_shared<EDXRegisterAssemblyASTNode>(),
        destination->emitAssembly()
      )
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  BitwiseAndTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperatorTACKYASTNode(
      std::make_shared<BitwiseAndAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  BitwiseOrTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperatorTACKYASTNode(
      std::make_shared<BitwiseOrAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  BitwiseXorTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperatorTACKYASTNode(
      std::make_shared<BitwiseXorAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  LeftShiftTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperatorTACKYASTNode(
      std::make_shared<LeftShiftAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  RightShiftTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperatorTACKYASTNode(
      std::make_shared<RightShiftAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  OrderingOperatorTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    auto const &[left_operand, right_operand, destination]{ std::move(input) };
    auto const &destination_assembly{ destination->emitAssembly() };
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<CmpAssemblyASTNode>(
        right_operand->emitAssembly(),
        left_operand->emitAssembly()
      ),
      std::make_shared<MovlAssemblyASTNode>(
        std::make_shared<ImmediateValueAssemblyASTNode>(0),
        destination_assembly
      ),
      std::make_shared<SetCCAssemblyASTNode>(
        emitConditionCode(),
        destination_assembly
      )
    };
  }

  [[nodiscard]] std::shared_ptr<CondCodeAssemblyASTNode>
  EqualsTACKYASTNode::emitConditionCode() const
  {
    return std::make_shared<ECondCodeAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<CondCodeAssemblyASTNode>
  NotEqualsTACKYASTNode::emitConditionCode() const
  {
    return std::make_shared<NECondCodeAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<CondCodeAssemblyASTNode>
  LessThanTACKYASTNode::emitConditionCode() const
  {
    return std::make_shared<LCondCodeAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<CondCodeAssemblyASTNode>
  GreaterThanTACKYASTNode::emitConditionCode() const
  {
    return std::make_shared<GCondCodeAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<CondCodeAssemblyASTNode>
  LessThanOrEqualToTACKYASTNode::emitConditionCode() const
  {
    return std::make_shared<LECondCodeAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<CondCodeAssemblyASTNode>
  GreaterThanOrEqualToTACKYASTNode::emitConditionCode() const
  {
    return std::make_shared<GECondCodeAssemblyASTNode>();
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  BinaryTACKYASTNode::emitAssembly() const
  {
    return getBinaryOperator()->emitAssembly(
      { getLeftOperand(), getRightOperand(), getDestination() }
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  CopyTACKYASTNode::emitAssembly() const
  {
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<MovlAssemblyASTNode>(
        getSource()->emitAssembly(),
        getDestination()->emitAssembly()
      )
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  JumpTACKYASTNode::emitAssembly() const
  {
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<JmpAssemblyASTNode>(getIdentifier())
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  JumpIfZeroTACKYASTNode::emitAssembly() const
  {
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<CmpAssemblyASTNode>(
        std::make_shared<ImmediateValueAssemblyASTNode>(0),
        getCondition()->emitAssembly()
      ),
      std::make_shared<JmpCCAssemblyASTNode>(
        std::make_shared<ECondCodeAssemblyASTNode>(),
        getIdentifier()
      )
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  JumpIfNotZeroTACKYASTNode::emitAssembly() const
  {
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<CmpAssemblyASTNode>(
        std::make_shared<ImmediateValueAssemblyASTNode>(0),
        getCondition()->emitAssembly()
      ),
      std::make_shared<JmpCCAssemblyASTNode>(
        std::make_shared<NECondCodeAssemblyASTNode>(),
        getIdentifier()
      )
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  LabelTACKYASTNode::emitAssembly() const
  {
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<LabelAssemblyASTNode>(getIdentifier())
    };
  }

  [[nodiscard]] std::shared_ptr<FunctionAssemblyASTNode>
  FunctionTACKYASTNode::emitAssembly() const
  {
    return std::make_shared<FunctionAssemblyASTNode>(
      getIdentifier(),
      getInstructions() | std::views::transform([](auto const instruction) {
      return instruction->emitAssembly();
    }) | std::views::join
        | std::ranges::to<std::vector>()
    );
  }

  [[nodiscard]] std::shared_ptr<ProgramAssemblyASTNode>
  ProgramTACKYASTNode::emitAssembly() const
  {
    return std::make_shared<ProgramAssemblyASTNode>(getFunction()->emitAssembly(
    ));
  }
} // namespace SC2
