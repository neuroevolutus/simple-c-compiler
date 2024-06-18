#include <catch2/catch_test_macros.hpp>
#include <sc2/assembly_generator.hpp>
#include <sc2/lexer.hpp>
#include <sc2/parser.hpp>
#include <sc2/test_fixtures.hpp>

TEST_CASE("code generator behaves correctly")
{
  SC2::Lexer  lexer{ basic_program_text };
  SC2::Parser parser{ lexer };
  REQUIRE(SC2::AssemblyGenerator::generateProgramAssembly(parser.parseProgram())->prettyPrint() ==
          "Program:\n"
          "  Function: main\n"
          "    Instruction: Mov (ImmediateValue 2), (Register %eax)\n"
          "    Instruction: Ret\n"
  );
}
