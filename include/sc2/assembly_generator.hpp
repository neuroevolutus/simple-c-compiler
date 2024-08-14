#ifndef SC2_ASSEMBLY_GENERATOR_HPP_INCLUDED
#define SC2_ASSEMBLY_GENERATOR_HPP_INCLUDED

#include <sc2/assembly_ast.hpp>

#include <memory>
#include <vector>

namespace SC2 {
  struct AssemblyGenerator
  {
    static constexpr std::shared_ptr<ProgramAssemblyASTNode>
    generateProgramAssembly(std::shared_ptr<ProgramASTNode> program)
    {
      return std::make_shared<ProgramAssemblyASTNode>(
        generateFunctionAssembly(program->getFunction())
      );
    }

    static constexpr std::shared_ptr<FunctionAssemblyASTNode>
    generateFunctionAssembly(std::shared_ptr<FunctionASTNode> function)
    {
      return std::make_shared<FunctionAssemblyASTNode>(
        function->getIdentifier(),
        generateInstructionsAssembly(function->getStatement())
      );
    }

    static constexpr std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
    generateInstructionsAssembly(std::shared_ptr<StatementASTNode> statement)
    {
      return { std::make_shared<MovAssemblyASTNode>(
                 generateExpressionAssembly(statement->getExpression()),
                 std::make_shared<RegisterAssemblyASTNode>()
               ),
               std::make_shared<ReturnAssemblyASTNode>() };
    }

    static constexpr std::shared_ptr<ImmediateValueAssemblyASTNode>
    generateExpressionAssembly(std::shared_ptr<ExpressionASTNode> expression)
    {
      return generateLiteralConstantAssembly(
        std::dynamic_pointer_cast<LiteralConstantASTNode>(expression)
          ->getLiteralConstant()
      );
    }

    static constexpr std::shared_ptr<ImmediateValueAssemblyASTNode>
    generateLiteralConstantAssembly(LiteralConstant const &value)
    {
      return std::make_shared<ImmediateValueAssemblyASTNode>(value);
    }
  };

} // namespace SC2

#endif
