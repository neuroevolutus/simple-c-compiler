#include <sc2/assembly_ast.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace SC2 {
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
                 std::make_shared<R10dRegisterAssemblyASTNode>()
               ),
               std::make_shared<BinaryAssemblyASTNode>(
                 std::dynamic_pointer_cast<BinaryOperatorAssemblyASTNode>(
                   shared_from_this()
                 ),
                 std::make_shared<R10dRegisterAssemblyASTNode>(),
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
                 std::make_shared<R11dRegisterAssemblyASTNode>()
               ),
               std::make_shared<BinaryAssemblyASTNode>(
                 std::make_shared<MultiplyAssemblyASTNode>(),
                 std::move(source),
                 std::make_shared<R11dRegisterAssemblyASTNode>()
               ),
               std::make_shared<MovlAssemblyASTNode>(
                 std::make_shared<R11dRegisterAssemblyASTNode>(),
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
        std::make_shared<R11bRegisterAssemblyASTNode>()
      ),
      std::make_shared<MovbAssemblyASTNode>(
        std::make_shared<R11bRegisterAssemblyASTNode>(),
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
