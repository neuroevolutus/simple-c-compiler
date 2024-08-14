#include <catch2/catch_test_macros.hpp>
#include <sc2/assembly_ast.hpp>
#include <sc2/ast.hpp>
#include <sc2/lexer.hpp>
#include <sc2/parser.hpp>
#include <sc2/tacky_ast.hpp>

#include <memory>

TEST_CASE("assembly generator works correctly")
{
  SECTION("Chapter 2: a basic program is correctly assembled")
  {
    constexpr char const * const program_text{
      "int main(void) {\n"
      "  return -(~2);\n"
      "}\n"
    };
    SC2::Lexer                                   lexer{ program_text };
    SC2::Parser                                  parser{ lexer };
    std::shared_ptr<SC2::ProgramASTNode>         ast{ parser.parseProgram() };
    std::shared_ptr<SC2::ProgramTACKYASTNode>    tacky{ ast->emitTACKY() };
    std::shared_ptr<SC2::ProgramAssemblyASTNode> assembly{ tacky->emitAssembly(
    ) };
    REQUIRE(assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    Instruction: Mov (ImmediateValue: 2), (PseudoRegister: main.0)\n"
      "    Instruction: Unary (Complement (PseudoRegister: main.0)\n"
      "    Instruction: Mov (PseudoRegister: main.0), (PseudoRegister: main.1)\n"
      "    Instruction: Unary (Negate (PseudoRegister: main.1)\n"
      "    Instruction: Mov (PseudoRegister: main.1), (Register: %ax)\n"
      "    Instruction: Ret\n"
    );
    auto [last_offset, _, cleaned_assembly]{ assembly->replacePseudoRegisters(
    ) };
    REQUIRE(cleaned_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    Instruction: Mov (ImmediateValue: 2), (StackOffset: -4)\n"
      "    Instruction: Unary (Complement (StackOffset: -4)\n"
      "    Instruction: Mov (StackOffset: -4), (StackOffset: -8)\n"
      "    Instruction: Unary (Negate (StackOffset: -8)\n"
      "    Instruction: Mov (StackOffset: -8), (Register: %ax)\n"
      "    Instruction: Ret\n"
    );
    auto fixed_assembly{ cleaned_assembly->fixUpInstructions(-last_offset) };
    REQUIRE(fixed_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    Instruction: AllocateStack(8)\n"
      "    Instruction: Mov (ImmediateValue: 2), (StackOffset: -4)\n"
      "    Instruction: Unary (Complement (StackOffset: -4)\n"
      "    Instruction: Mov (StackOffset: -4), (Register: %r10)\n"
      "    Instruction: Mov (Register: %r10), (StackOffset: -8)\n"
      "    Instruction: Unary (Negate (StackOffset: -8)\n"
      "    Instruction: Mov (StackOffset: -8), (Register: %ax)\n"
      "    Instruction: Ret\n"
    );
  }
}
