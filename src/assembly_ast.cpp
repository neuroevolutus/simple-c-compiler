#include <sc2/assembly_ast.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace SC2 {
  [[nodiscard]] std::shared_ptr<ByteRegisterAssemblyASTNode>
  EAXRegisterAssemblyASTNode::toByteRegister()
  {
    return std::make_shared<ALRegisterAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<LongWordRegisterAssemblyASTNode>
  ALRegisterAssemblyASTNode::toLongWordRegister()
  {
    return std::make_shared<EAXRegisterAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<ByteRegisterAssemblyASTNode>
  EDXRegisterAssemblyASTNode::toByteRegister()
  {
    return std::make_shared<DLRegisterAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<LongWordRegisterAssemblyASTNode>
  DLRegisterAssemblyASTNode::toLongWordRegister()
  {
    return std::make_shared<EDXRegisterAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<ByteRegisterAssemblyASTNode>
  R10DRegisterAssemblyASTNode::toByteRegister()
  {
    return std::make_shared<R10BRegisterAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<LongWordRegisterAssemblyASTNode>
  R10BRegisterAssemblyASTNode::toLongWordRegister()
  {
    return std::make_shared<R10DRegisterAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<ByteRegisterAssemblyASTNode>
  R11DRegisterAssemblyASTNode::toByteRegister()
  {
    return std::make_shared<R11BRegisterAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<LongWordRegisterAssemblyASTNode>
  R11BRegisterAssemblyASTNode::toLongWordRegister()
  {
    return std::make_shared<R11DRegisterAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<ByteRegisterAssemblyASTNode>
  ECXRegisterAssemblyASTNode::toByteRegister()
  {
    return std::make_shared<CLRegisterAssemblyASTNode>();
  }

  [[nodiscard]] std::shared_ptr<LongWordRegisterAssemblyASTNode>
  CLRegisterAssemblyASTNode::toLongWordRegister()
  {
    return std::make_shared<ECXRegisterAssemblyASTNode>();
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  BinaryOperatorAssemblyASTNode::fixUp(BinaryAssemblyASTNodeFixUpInput &&input)
  {
    auto const &[source, destination]{ std::move(input) };
    auto const &stack_source{
      std::dynamic_pointer_cast<StackOffsetAssemblyASTNode>(source)
    };
    auto const &stack_destination{
      std::dynamic_pointer_cast<StackOffsetAssemblyASTNode>(destination)
    };
    return (stack_source && stack_destination) ?
             std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
               std::make_shared<MovlAssemblyASTNode>(
                 std::move(source),
                 std::make_shared<R10DRegisterAssemblyASTNode>()
               ),
               std::make_shared<BinaryAssemblyASTNode>(
                 std::dynamic_pointer_cast<BinaryOperatorAssemblyASTNode>(
                   shared_from_this()
                 ),
                 std::make_shared<R10DRegisterAssemblyASTNode>(),
                 std::move(destination)
               )
             } :
             std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
               std::make_shared<BinaryAssemblyASTNode>(
                 std::dynamic_pointer_cast<BinaryOperatorAssemblyASTNode>(
                   shared_from_this()
                 ),
                 std::move(source),
                 std::move(destination)
               )
             };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  CmpAssemblyASTNode::fixUp()
  {
    auto const &stack_source{
      std::dynamic_pointer_cast<StackOffsetAssemblyASTNode>(getLeftOperand())
    };
    auto const &stack_destination{
      std::dynamic_pointer_cast<StackOffsetAssemblyASTNode>(getRightOperand())
    };
    if (stack_source && stack_destination) {
      return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
        std::make_shared<MovlAssemblyASTNode>(
          std::move(stack_source),
          std::make_shared<R10DRegisterAssemblyASTNode>()
        ),
        std::make_shared<CmpAssemblyASTNode>(
          std::make_shared<R10DRegisterAssemblyASTNode>(),
          std::move(stack_destination)
        )
      };
    } else if (auto const &literal_constant_destination{
                 std::dynamic_pointer_cast<ImmediateValueAssemblyASTNode>(
                   getRightOperand()
                 ) }) {
      return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
        std::make_shared<MovlAssemblyASTNode>(
          std::move(literal_constant_destination),
          std::make_shared<R11DRegisterAssemblyASTNode>()
        ),
        std::make_shared<CmpAssemblyASTNode>(
          getLeftOperand(),
          std::make_shared<R11DRegisterAssemblyASTNode>()
        )
      };
    } else
      return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
        std::dynamic_pointer_cast<InstructionAssemblyASTNode>(shared_from_this()
        )
      };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  SetCCAssemblyASTNode::fixUp()
  {
    if (auto const &register_destination{
          std::dynamic_pointer_cast<RegisterAssemblyASTNode>(getDestination()
          ) }) {
      return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
        std::make_shared<SetCCAssemblyASTNode>(
          getConditionCode(),
          register_destination->toByteRegister()
        )
      };
    } else {
      return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
        std::dynamic_pointer_cast<InstructionAssemblyASTNode>(shared_from_this()
        )
      };
    }
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  MultiplyAssemblyASTNode::fixUp(BinaryAssemblyASTNodeFixUpInput &&input)
  {
    auto const &[source, destination]{ std::move(input) };
    auto const &stack_destination{
      std::dynamic_pointer_cast<StackOffsetAssemblyASTNode>(destination)
    };
    return stack_destination ?
             std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
               std::make_shared<MovlAssemblyASTNode>(
                 destination,
                 std::make_shared<R11DRegisterAssemblyASTNode>()
               ),
               std::make_shared<BinaryAssemblyASTNode>(
                 std::make_shared<MultiplyAssemblyASTNode>(),
                 std::move(source),
                 std::make_shared<R11DRegisterAssemblyASTNode>()
               ),
               std::make_shared<MovlAssemblyASTNode>(
                 std::make_shared<R11DRegisterAssemblyASTNode>(),
                 destination
               )
             } :
             std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
               std::make_shared<BinaryAssemblyASTNode>(
                 std::make_shared<MultiplyAssemblyASTNode>(),
                 std::move(source),
                 std::move(destination)
               )
             };
  }

  [[nodiscard]] std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
  ShiftOperatorAssemblyASTNode::fixUp(BinaryAssemblyASTNodeFixUpInput &&input)
  {
    auto const &[source, destination]{ std::move(input) };
    return std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
      std::make_shared<MovbAssemblyASTNode>(
        std::move(source),
        std::make_shared<R11BRegisterAssemblyASTNode>()
      ),
      std::make_shared<MovbAssemblyASTNode>(
        std::make_shared<R11BRegisterAssemblyASTNode>(),
        std::make_shared<CLRegisterAssemblyASTNode>()
      ),
      std::make_shared<BinaryAssemblyASTNode>(
        std::dynamic_pointer_cast<BinaryOperatorAssemblyASTNode>(
          std::move(shared_from_this())
        ),
        std::make_shared<CLRegisterAssemblyASTNode>(),
        std::move(destination)
      )
    };
  }
} // namespace SC2
