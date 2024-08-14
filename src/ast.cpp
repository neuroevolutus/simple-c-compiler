#include <sc2/ast.hpp>
#include <sc2/tacky_ast.hpp>
#include <string_view>

#include <memory>
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

  std::pair<
    std::shared_ptr<ValueTACKYASTNode>,
    std::vector<std::shared_ptr<InstructionTACKYASTNode>>>
  LiteralConstantASTNode::emitTACKY(
    std::string_view,
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions
  ) const
  {
    return std::make_pair<
      std::shared_ptr<ValueTACKYASTNode>,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>>>(
      std::make_shared<LiteralConstantTACKYASTNode>(getValue()),
      std::move(instructions)
    );
  }

  std::pair<
    std::shared_ptr<ValueTACKYASTNode>,
    std::vector<std::shared_ptr<InstructionTACKYASTNode>>>
  UnaryExpressionASTNode::emitTACKY(
    std::string_view                                      identifier,
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions
  ) const
  {
    auto [source, new_instructions]{
      getExpression()->emitTACKY(identifier, std::move(instructions))
    };
    auto destination{ std::make_shared<VariableTACKYASTNode>(
      Utility::generateFreshIdentifier(identifier)
    ) };
    new_instructions.push_back(std::make_shared<UnaryTACKYASTNode>(
      getUnaryOperator()->emitTACKY(),
      source,
      destination
    ));
    return std::make_pair<
      std::shared_ptr<ValueTACKYASTNode>,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>>>(
      destination,
      std::move(new_instructions)
    );
  }

  std::vector<std::shared_ptr<InstructionTACKYASTNode>>
  StatementASTNode::emitTACKY(std::string_view identifier) const
  {
    auto [value, instructions]{ getExpression()->emitTACKY(
      identifier,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>>{}
    ) };
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
