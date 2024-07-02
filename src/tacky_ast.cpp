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
        std::make_shared<AXRegisterAssemblyASTNode>()
      ),
      std::make_shared<ReturnAssemblyASTNode>()
    };
  }

  [[nodiscard]] std::shared_ptr<UnaryOperatorAssemblyASTNode>
  ComplementTACKYASTNode::emitAssembly() const
  {
    return std::make_shared<ComplementAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<UnaryOperatorAssemblyASTNode>
  NegateTACKYASTNode::emitAssembly() const
  {
    return std::make_shared<NegateAssemblyASTNode>();
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  UnaryTACKYASTNode::emitAssembly() const
  {
    auto const &destinationAssembly{ getDestination()->emitAssembly() };
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<MovlAssemblyASTNode>(
        getSource()->emitAssembly(),
        destinationAssembly
      ),
      std::make_shared<UnaryAssemblyASTNode>(
        getUnaryOperator()->emitAssembly(),
        destinationAssembly
      )
    };
  }

  [[nodiscard]] static std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  emitDefaultAssemblyForBinaryOperator(
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
    return emitDefaultAssemblyForBinaryOperator(
      std::make_shared<AddAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  SubtractTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperator(
      std::make_shared<SubtractAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  MultiplyTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperator(
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
        std::make_shared<AXRegisterAssemblyASTNode>()
      ),
      std::make_shared<CdqAssemblyASTNode>(),
      std::make_shared<IdivAssemblyASTNode>(right_operand->emitAssembly()),
      std::make_shared<MovlAssemblyASTNode>(
        std::make_shared<AXRegisterAssemblyASTNode>(),
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
        std::make_shared<AXRegisterAssemblyASTNode>()
      ),
      std::make_shared<CdqAssemblyASTNode>(),
      std::make_shared<IdivAssemblyASTNode>(right_operand->emitAssembly()),
      std::make_shared<MovlAssemblyASTNode>(
        std::make_shared<DXRegisterAssemblyASTNode>(),
        destination->emitAssembly()
      )
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  BitwiseAndTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperator(
      std::make_shared<BitwiseAndAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  BitwiseOrTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperator(
      std::make_shared<BitwiseOrAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  BitwiseXorTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperator(
      std::make_shared<BitwiseXorAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  LeftShiftTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperator(
      std::make_shared<LeftShiftAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  RightShiftTACKYASTNode::emitAssembly(
    BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
  ) const
  {
    return emitDefaultAssemblyForBinaryOperator(
      std::make_shared<RightShiftAssemblyASTNode>(),
      std::move(input)
    );
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  BinaryTACKYASTNode::emitAssembly() const
  {
    return getBinaryOperator()->emitAssembly(
      { getLeftOperand(), getRightOperand(), getDestination() }
    );
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
