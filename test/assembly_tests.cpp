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
      "    Movl (ImmediateValue: 2), (PseudoRegister: main.0)\n"
      "    Unary (Complement (PseudoRegister: main.0))\n"
      "    Movl (PseudoRegister: main.0), (PseudoRegister: main.1)\n"
      "    Unary (Negate (PseudoRegister: main.1))\n"
      "    Movl (PseudoRegister: main.1), (Register: %eax)\n"
      "    Ret\n"
    );
    auto [last_offset, _, cleaned_assembly]{ assembly->replacePseudoRegisters(
    ) };
    REQUIRE(cleaned_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    Movl (ImmediateValue: 2), (StackOffset: -4)\n"
      "    Unary (Complement (StackOffset: -4))\n"
      "    Movl (StackOffset: -4), (StackOffset: -8)\n"
      "    Unary (Negate (StackOffset: -8))\n"
      "    Movl (StackOffset: -8), (Register: %eax)\n"
      "    Ret\n"
    );
    auto fixed_assembly{ cleaned_assembly->fixUp(-last_offset) };
    REQUIRE(fixed_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    AllocateStack(8)\n"
      "    Movl (ImmediateValue: 2), (StackOffset: -4)\n"
      "    Unary (Complement (StackOffset: -4))\n"
      "    Movl (StackOffset: -4), (Register: %r10d)\n"
      "    Movl (Register: %r10d), (StackOffset: -8)\n"
      "    Unary (Negate (StackOffset: -8))\n"
      "    Movl (StackOffset: -8), (Register: %eax)\n"
      "    Ret\n"
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
      "    Movl (ImmediateValue: 2), (PseudoRegister: main.2)\n"
      "    Binary (Multiply (ImmediateValue: 10), (PseudoRegister: main.2))\n"
      "    Movl (PseudoRegister: main.2), (Register: %eax)\n"
      "    Cdq\n"
      "    Idiv (ImmediateValue: 7)\n"
      "    Movl (Register: %edx), (PseudoRegister: main.3)\n"
      "    Movl (ImmediateValue: 5), (Register: %eax)\n"
      "    Cdq\n"
      "    Idiv (ImmediateValue: 2)\n"
      "    Movl (Register: %eax), (PseudoRegister: main.4)\n"
      "    Movl (PseudoRegister: main.4), (PseudoRegister: main.5)\n"
      "    Binary (RightShift (ImmediateValue: 3), (PseudoRegister: main.5))\n"
      "    Movl (PseudoRegister: main.3), (PseudoRegister: main.6)\n"
      "    Binary (BitwiseAnd (PseudoRegister: main.5), (PseudoRegister: main.6))\n"
      "    Movl (PseudoRegister: main.6), (Register: %eax)\n"
      "    Ret\n"
    );
    auto [last_offset, _, cleaned_assembly]{ assembly->replacePseudoRegisters(
    ) };
    REQUIRE(cleaned_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    Movl (ImmediateValue: 2), (StackOffset: -4)\n"
      "    Binary (Multiply (ImmediateValue: 10), (StackOffset: -4))\n"
      "    Movl (StackOffset: -4), (Register: %eax)\n"
      "    Cdq\n"
      "    Idiv (ImmediateValue: 7)\n"
      "    Movl (Register: %edx), (StackOffset: -8)\n"
      "    Movl (ImmediateValue: 5), (Register: %eax)\n"
      "    Cdq\n"
      "    Idiv (ImmediateValue: 2)\n"
      "    Movl (Register: %eax), (StackOffset: -12)\n"
      "    Movl (StackOffset: -12), (StackOffset: -16)\n"
      "    Binary (RightShift (ImmediateValue: 3), (StackOffset: -16))\n"
      "    Movl (StackOffset: -8), (StackOffset: -20)\n"
      "    Binary (BitwiseAnd (StackOffset: -16), (StackOffset: -20))\n"
      "    Movl (StackOffset: -20), (Register: %eax)\n"
      "    Ret\n"
    );
    auto fixed_assembly{ cleaned_assembly->fixUp(-last_offset) };
    REQUIRE(fixed_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    AllocateStack(20)\n"
      "    Movl (ImmediateValue: 2), (StackOffset: -4)\n"
      "    Movl (StackOffset: -4), (Register: %r11d)\n"
      "    Binary (Multiply (ImmediateValue: 10), (Register: %r11d))\n"
      "    Movl (Register: %r11d), (StackOffset: -4)\n"
      "    Movl (StackOffset: -4), (Register: %eax)\n"
      "    Cdq\n"
      "    Movl (ImmediateValue: 7), (Register: %r10d)\n"
      "    Idiv (Register: %r10d)\n"
      "    Movl (Register: %edx), (StackOffset: -8)\n"
      "    Movl (ImmediateValue: 5), (Register: %eax)\n"
      "    Cdq\n"
      "    Movl (ImmediateValue: 2), (Register: %r10d)\n"
      "    Idiv (Register: %r10d)\n"
      "    Movl (Register: %eax), (StackOffset: -12)\n"
      "    Movl (StackOffset: -12), (Register: %r10d)\n"
      "    Movl (Register: %r10d), (StackOffset: -16)\n"
      "    Movb (ImmediateValue: 3), (Register: %r11b)\n"
      "    Movb (Register: %r11b), (Register: %cl)\n"
      "    Binary (RightShift (Register: %cl), (StackOffset: -16))\n"
      "    Movl (StackOffset: -8), (Register: %r10d)\n"
      "    Movl (Register: %r10d), (StackOffset: -20)\n"
      "    Movl (StackOffset: -16), (Register: %r10d)\n"
      "    Binary (BitwiseAnd (Register: %r10d), (StackOffset: -20))\n"
      "    Movl (StackOffset: -20), (Register: %eax)\n"
      "    Ret\n"
    );
  }
  SECTION(
    "Chapter 4: a program with comparison and relational operators is "
    "assembled correctly"
  )
  {
    constexpr char const * const program_text{
      "int main(void) {\n"
      "  return ((((1 < 2) != (3 > 4)) && ((5 <= 6) == (7 >= 8))) || !(2));\n"
      "}"
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
      "    Cmp (ImmediateValue: 2), (ImmediateValue: 1)\n"
      "    Movl (ImmediateValue: 0), (PseudoRegister: main.7)\n"
      "    SetCC L, (PseudoRegister: main.7)\n"
      "    Cmp (ImmediateValue: 4), (ImmediateValue: 3)\n"
      "    Movl (ImmediateValue: 0), (PseudoRegister: main.8)\n"
      "    SetCC G, (PseudoRegister: main.8)\n"
      "    Cmp (PseudoRegister: main.8), (PseudoRegister: main.7)\n"
      "    Movl (ImmediateValue: 0), (PseudoRegister: main.9)\n"
      "    SetCC NE, (PseudoRegister: main.9)\n"
      "    Cmp (ImmediateValue: 0), (PseudoRegister: main.9)\n"
      "    JmpCC E, main.10_false_label\n"
      "    Cmp (ImmediateValue: 6), (ImmediateValue: 5)\n"
      "    Movl (ImmediateValue: 0), (PseudoRegister: main.11)\n"
      "    SetCC LE, (PseudoRegister: main.11)\n"
      "    Cmp (ImmediateValue: 8), (ImmediateValue: 7)\n"
      "    Movl (ImmediateValue: 0), (PseudoRegister: main.12)\n"
      "    SetCC GE, (PseudoRegister: main.12)\n"
      "    Cmp (PseudoRegister: main.12), (PseudoRegister: main.11)\n"
      "    Movl (ImmediateValue: 0), (PseudoRegister: main.13)\n"
      "    SetCC E, (PseudoRegister: main.13)\n"
      "    Cmp (ImmediateValue: 0), (PseudoRegister: main.13)\n"
      "    JmpCC E, main.10_false_label\n"
      "    Movl (ImmediateValue: 1), (PseudoRegister: main.14)\n"
      "    Jmp main.15\n"
      "    Label main.10_false_label\n"
      "    Movl (ImmediateValue: 0), (PseudoRegister: main.14)\n"
      "    Label main.15\n"
      "    Cmp (ImmediateValue: 0), (PseudoRegister: main.14)\n"
      "    JmpCC NE, main.16_false_label\n"
      "    Cmp (ImmediateValue: 0), (ImmediateValue: 2)\n"
      "    Movl (ImmediateValue: 0), (PseudoRegister: main.17)\n"
      "    SetCC E, (PseudoRegister: main.17)\n"
      "    Cmp (ImmediateValue: 0), (PseudoRegister: main.17)\n"
      "    JmpCC NE, main.16_false_label\n"
      "    Movl (ImmediateValue: 0), (PseudoRegister: main.18)\n"
      "    Jmp main.19\n"
      "    Label main.16_false_label\n"
      "    Movl (ImmediateValue: 1), (PseudoRegister: main.18)\n"
      "    Label main.19\n"
      "    Movl (PseudoRegister: main.18), (Register: %eax)\n"
      "    Ret\n"
    );
    auto [last_offset, _, cleaned_assembly]{ assembly->replacePseudoRegisters(
    ) };
    REQUIRE(cleaned_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    Cmp (ImmediateValue: 2), (ImmediateValue: 1)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -4)\n"
      "    SetCC L, (StackOffset: -4)\n"
      "    Cmp (ImmediateValue: 4), (ImmediateValue: 3)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -8)\n"
      "    SetCC G, (StackOffset: -8)\n"
      "    Cmp (StackOffset: -8), (StackOffset: -4)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -12)\n"
      "    SetCC NE, (StackOffset: -12)\n"
      "    Cmp (ImmediateValue: 0), (StackOffset: -12)\n"
      "    JmpCC E, main.10_false_label\n"
      "    Cmp (ImmediateValue: 6), (ImmediateValue: 5)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -16)\n"
      "    SetCC LE, (StackOffset: -16)\n"
      "    Cmp (ImmediateValue: 8), (ImmediateValue: 7)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -20)\n"
      "    SetCC GE, (StackOffset: -20)\n"
      "    Cmp (StackOffset: -20), (StackOffset: -16)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -24)\n"
      "    SetCC E, (StackOffset: -24)\n"
      "    Cmp (ImmediateValue: 0), (StackOffset: -24)\n"
      "    JmpCC E, main.10_false_label\n"
      "    Movl (ImmediateValue: 1), (StackOffset: -28)\n"
      "    Jmp main.15\n"
      "    Label main.10_false_label\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -28)\n"
      "    Label main.15\n"
      "    Cmp (ImmediateValue: 0), (StackOffset: -28)\n"
      "    JmpCC NE, main.16_false_label\n"
      "    Cmp (ImmediateValue: 0), (ImmediateValue: 2)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -32)\n"
      "    SetCC E, (StackOffset: -32)\n"
      "    Cmp (ImmediateValue: 0), (StackOffset: -32)\n"
      "    JmpCC NE, main.16_false_label\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -36)\n"
      "    Jmp main.19\n"
      "    Label main.16_false_label\n"
      "    Movl (ImmediateValue: 1), (StackOffset: -36)\n"
      "    Label main.19\n"
      "    Movl (StackOffset: -36), (Register: %eax)\n"
      "    Ret\n"
    );
    auto fixed_assembly{ cleaned_assembly->fixUp(-last_offset) };
    REQUIRE(fixed_assembly->prettyPrint() ==
      "Program:\n"
      "  Function: main\n"
      "    AllocateStack(36)\n"
      "    Movl (ImmediateValue: 1), (Register: %r11d)\n"
      "    Cmp (ImmediateValue: 2), (Register: %r11d)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -4)\n"
      "    SetCC L, (StackOffset: -4)\n"
      "    Movl (ImmediateValue: 3), (Register: %r11d)\n"
      "    Cmp (ImmediateValue: 4), (Register: %r11d)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -8)\n"
      "    SetCC G, (StackOffset: -8)\n"
      "    Movl (StackOffset: -8), (Register: %r10d)\n"
      "    Cmp (Register: %r10d), (StackOffset: -4)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -12)\n"
      "    SetCC NE, (StackOffset: -12)\n"
      "    Cmp (ImmediateValue: 0), (StackOffset: -12)\n"
      "    JmpCC E, main.10_false_label\n"
      "    Movl (ImmediateValue: 5), (Register: %r11d)\n"
      "    Cmp (ImmediateValue: 6), (Register: %r11d)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -16)\n"
      "    SetCC LE, (StackOffset: -16)\n"
      "    Movl (ImmediateValue: 7), (Register: %r11d)\n"
      "    Cmp (ImmediateValue: 8), (Register: %r11d)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -20)\n"
      "    SetCC GE, (StackOffset: -20)\n"
      "    Movl (StackOffset: -20), (Register: %r10d)\n"
      "    Cmp (Register: %r10d), (StackOffset: -16)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -24)\n"
      "    SetCC E, (StackOffset: -24)\n"
      "    Cmp (ImmediateValue: 0), (StackOffset: -24)\n"
      "    JmpCC E, main.10_false_label\n"
      "    Movl (ImmediateValue: 1), (StackOffset: -28)\n"
      "    Jmp main.15\n"
      "    Label main.10_false_label\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -28)\n"
      "    Label main.15\n"
      "    Cmp (ImmediateValue: 0), (StackOffset: -28)\n"
      "    JmpCC NE, main.16_false_label\n"
      "    Movl (ImmediateValue: 2), (Register: %r11d)\n"
      "    Cmp (ImmediateValue: 0), (Register: %r11d)\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -32)\n"
      "    SetCC E, (StackOffset: -32)\n"
      "    Cmp (ImmediateValue: 0), (StackOffset: -32)\n"
      "    JmpCC NE, main.16_false_label\n"
      "    Movl (ImmediateValue: 0), (StackOffset: -36)\n"
      "    Jmp main.19\n"
      "    Label main.16_false_label\n"
      "    Movl (ImmediateValue: 1), (StackOffset: -36)\n"
      "    Label main.19\n"
      "    Movl (StackOffset: -36), (Register: %eax)\n"
      "    Ret\n"
    );
  }
}
