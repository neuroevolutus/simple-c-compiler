#include <catch2/catch_test_macros.hpp>
#include <sc2/assembly_ast.hpp>
#include <sc2/ast.hpp>
#include <sc2/lexer.hpp>
#include <sc2/parser.hpp>
#include <sc2/tacky_ast.hpp>

#include <memory>

TEST_CASE("assembly generator works correctly")
{
  SECTION("Chapter 2: a basic program is assembled correctly")
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
      "    Instruction: Movl (ImmediateValue: 2), (PseudoRegister: main.0)\n"
      "    Instruction: Unary (Complement (PseudoRegister: main.0))\n"
      "    Instruction: Movl (PseudoRegister: main.0), (PseudoRegister: main.1)\n"
      "    Instruction: Unary (Negate (PseudoRegister: main.1))\n"
      "    Instruction: Movl (PseudoRegister: main.1), (Register: %ax)\n"
      "    Instruction: Ret\n"
    );
    auto [last_offset, _, cleaned_assembly]{ assembly->replacePseudoRegisters(
    ) };
    REQUIRE(cleaned_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    Instruction: Movl (ImmediateValue: 2), (StackOffset: -4)\n"
      "    Instruction: Unary (Complement (StackOffset: -4))\n"
      "    Instruction: Movl (StackOffset: -4), (StackOffset: -8)\n"
      "    Instruction: Unary (Negate (StackOffset: -8))\n"
      "    Instruction: Movl (StackOffset: -8), (Register: %ax)\n"
      "    Instruction: Ret\n"
    );
    auto fixed_assembly{ cleaned_assembly->fixUp(-last_offset) };
    REQUIRE(fixed_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    Instruction: AllocateStack(8)\n"
      "    Instruction: Movl (ImmediateValue: 2), (StackOffset: -4)\n"
      "    Instruction: Unary (Complement (StackOffset: -4))\n"
      "    Instruction: Movl (StackOffset: -4), (Register: %r10d)\n"
      "    Instruction: Movl (Register: %r10d), (StackOffset: -8)\n"
      "    Instruction: Unary (Negate (StackOffset: -8))\n"
      "    Instruction: Movl (StackOffset: -8), (Register: %ax)\n"
      "    Instruction: Ret\n"
    );
  }
  SECTION("Chapter 3: a program with binary operators is assembled correctly")
  {
    constexpr char const * const program_text{
      "int main(void) {\n"
      "  return 2 * 10 % 7 & 5 / 2 >> 3;\n"
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
      "    Instruction: Movl (ImmediateValue: 2), (PseudoRegister: main.2)\n"
      "    Instruction: Binary (Multiply (ImmediateValue: 10), (PseudoRegister: main.2))\n"
      "    Instruction: Movl (PseudoRegister: main.2), (Register: %ax)\n"
      "    Instruction: Cdq\n"
      "    Instruction: Idiv (ImmediateValue: 7)\n"
      "    Instruction: Movl (Register: %dx), (PseudoRegister: main.3)\n"
      "    Instruction: Movl (ImmediateValue: 5), (Register: %ax)\n"
      "    Instruction: Cdq\n"
      "    Instruction: Idiv (ImmediateValue: 2)\n"
      "    Instruction: Movl (Register: %ax), (PseudoRegister: main.4)\n"
      "    Instruction: Movl (PseudoRegister: main.4), (PseudoRegister: main.5)\n"
      "    Instruction: Binary (RightShift (ImmediateValue: 3), (PseudoRegister: main.5))\n"
      "    Instruction: Movl (PseudoRegister: main.3), (PseudoRegister: main.6)\n"
      "    Instruction: Binary (BitwiseAnd (PseudoRegister: main.5), (PseudoRegister: main.6))\n"
      "    Instruction: Movl (PseudoRegister: main.6), (Register: %ax)\n"
      "    Instruction: Ret\n"
    );
    auto [last_offset, _, cleaned_assembly]{ assembly->replacePseudoRegisters(
    ) };
    REQUIRE(cleaned_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    Instruction: Movl (ImmediateValue: 2), (StackOffset: -4)\n"
      "    Instruction: Binary (Multiply (ImmediateValue: 10), (StackOffset: -4))\n"
      "    Instruction: Movl (StackOffset: -4), (Register: %ax)\n"
      "    Instruction: Cdq\n"
      "    Instruction: Idiv (ImmediateValue: 7)\n"
      "    Instruction: Movl (Register: %dx), (StackOffset: -8)\n"
      "    Instruction: Movl (ImmediateValue: 5), (Register: %ax)\n"
      "    Instruction: Cdq\n"
      "    Instruction: Idiv (ImmediateValue: 2)\n"
      "    Instruction: Movl (Register: %ax), (StackOffset: -12)\n"
      "    Instruction: Movl (StackOffset: -12), (StackOffset: -16)\n"
      "    Instruction: Binary (RightShift (ImmediateValue: 3), (StackOffset: -16))\n"
      "    Instruction: Movl (StackOffset: -8), (StackOffset: -20)\n"
      "    Instruction: Binary (BitwiseAnd (StackOffset: -16), (StackOffset: -20))\n"
      "    Instruction: Movl (StackOffset: -20), (Register: %ax)\n"
      "    Instruction: Ret\n"
    );
    auto fixed_assembly{ cleaned_assembly->fixUp(-last_offset) };
    REQUIRE(fixed_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    Instruction: AllocateStack(20)\n"
      "    Instruction: Movl (ImmediateValue: 2), (StackOffset: -4)\n"
      "    Instruction: Movl (StackOffset: -4), (Register: %r11d)\n"
      "    Instruction: Binary (Multiply (ImmediateValue: 10), (Register: %r11d))\n"
      "    Instruction: Movl (Register: %r11d), (StackOffset: -4)\n"
      "    Instruction: Movl (StackOffset: -4), (Register: %ax)\n"
      "    Instruction: Cdq\n"
      "    Instruction: Movl (ImmediateValue: 7), (Register: %r10d)\n"
      "    Instruction: Idiv (Register: %r10d)\n"
      "    Instruction: Movl (Register: %dx), (StackOffset: -8)\n"
      "    Instruction: Movl (ImmediateValue: 5), (Register: %ax)\n"
      "    Instruction: Cdq\n"
      "    Instruction: Movl (ImmediateValue: 2), (Register: %r10d)\n"
      "    Instruction: Idiv (Register: %r10d)\n"
      "    Instruction: Movl (Register: %ax), (StackOffset: -12)\n"
      "    Instruction: Movl (StackOffset: -12), (Register: %r10d)\n"
      "    Instruction: Movl (Register: %r10d), (StackOffset: -16)\n"
      "    Instruction: Movb (ImmediateValue: 3), (Register: %r11b)\n"
      "    Instruction: Movb (Register: %r11b), (Register: %cl)\n"
      "    Instruction: Binary (RightShift (Register: %cl), (StackOffset: -16))\n"
      "    Instruction: Movl (StackOffset: -8), (Register: %r10d)\n"
      "    Instruction: Movl (Register: %r10d), (StackOffset: -20)\n"
      "    Instruction: Movl (StackOffset: -16), (Register: %r10d)\n"
      "    Instruction: Binary (BitwiseAnd (Register: %r10d), (StackOffset: -20))\n"
      "    Instruction: Movl (StackOffset: -20), (Register: %ax)\n"
      "    Instruction: Ret\n"
    );
  }
}
