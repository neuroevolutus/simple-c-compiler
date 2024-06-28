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
      std::make_shared<MovAssemblyASTNode>(
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
      std::make_shared<MovAssemblyASTNode>(
        getSource()->emitAssembly(),
        destinationAssembly
      ),
      std::make_shared<UnaryAssemblyASTNode>(
        getUnaryOperator()->emitAssembly(),
        destinationAssembly
      )
    };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  BinaryTACKYASTNode::emitAssembly() const
  {
    throw std::runtime_error("Unimplemented");
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
