#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <sc2/lexer.hpp>
#include <sc2/parser.hpp>
#include <sc2/tacky_ast.hpp>
#include <sc2/test_fixtures.hpp>

#include <memory>

TEST_CASE("tacky emitter behaves correctly")
{
  SECTION("Chapter 2: a basic program is correctly translated")
  {
    constexpr char const * const program_text_zero{
      "int main(void) {\n"
      "  return -(~2);\n"
      "}"
    };
    SC2::Lexer                           lexer_zero{ program_text_zero };
    SC2::Parser                          parser_zero{ lexer_zero };
    std::shared_ptr<SC2::ProgramASTNode> program_ast_zero{
      parser_zero.parseProgram()
    };
    std::shared_ptr<SC2::TACKYASTNode> tacky_ast_zero{
      program_ast_zero->emitTACKY()
    };
    REQUIRE(tacky_ast_zero->prettyPrint() ==
      "Function: main\n"
      "  Unary(Complement, LiteralConstant(2), Variable(\"main.0\"))\n"
      "  Unary(Negate, Variable(\"main.0\"), Variable(\"main.1\"))\n"
      "  Return(Variable(\"main.1\"))\n"
    );
    constexpr char const * const program_text_one{
      "int main(void) {\n"
      "  return ~12;\n"
      "}"
    };
    SC2::Lexer                           lexer_one{ program_text_one };
    SC2::Parser                          parser_one{ lexer_one };
    std::shared_ptr<SC2::ProgramASTNode> program_ast_one{
      parser_one.parseProgram()
    };
    std::shared_ptr<SC2::TACKYASTNode> tacky_ast_one{
      program_ast_one->emitTACKY()
    };
    REQUIRE(tacky_ast_one->prettyPrint() ==
      "Function: main\n"
      "  Unary(Complement, LiteralConstant(12), Variable(\"main.2\"))\n"
      "  Return(Variable(\"main.2\"))\n"
    );
  }
  SECTION("Chapter 3: a program with binary expressions is correctly translated"
  )
  {
    constexpr char const * const program_text_zero{
      "int main(void) {\n"
      "  return ~12 * 4;\n"
      "}"
    };
    SC2::Lexer                           lexer_zero{ program_text_zero };
    SC2::Parser                          parser_zero{ lexer_zero };
    std::shared_ptr<SC2::ProgramASTNode> program_ast_zero{
      parser_zero.parseProgram()
    };
    std::shared_ptr<SC2::TACKYASTNode> tacky_ast_zero{
      program_ast_zero->emitTACKY()
    };
    REQUIRE(tacky_ast_zero->prettyPrint() ==
      "Function: main\n"
      "  Unary(Complement, LiteralConstant(12), Variable(\"main.3\"))\n"
      "  Binary(Multiply, Variable(\"main.3\"), LiteralConstant(4), Variable(\"main.4\"))\n"
      "  Return(Variable(\"main.4\"))\n"
    );
  }
  SECTION(
    "Chapter 4: a program with logical and relational operators is correctly "
    "translated"
  )
  {
    constexpr char const * const program_text_zero{
      "int main(void) {\n"
      "  return ((((1 < 2) != (3 > 4)) && ((5 <= 6) == (7 >= 8))) || !(2));\n"
      "}"
    };
    SC2::Lexer                           lexer_zero{ program_text_zero };
    SC2::Parser                          parser_zero{ lexer_zero };
    std::shared_ptr<SC2::ProgramASTNode> program_ast_zero{
      parser_zero.parseProgram()
    };
    std::shared_ptr<SC2::TACKYASTNode> tacky_ast_zero{
      program_ast_zero->emitTACKY()
    };
    REQUIRE(tacky_ast_zero->prettyPrint() ==
      "Function: main\n"
      "  Binary(LessThan, LiteralConstant(1), LiteralConstant(2), Variable(\"main.5\"))\n"
      "  Binary(GreaterThan, LiteralConstant(3), LiteralConstant(4), Variable(\"main.6\"))\n"
      "  Binary(NotEquals, Variable(\"main.5\"), Variable(\"main.6\"), Variable(\"main.7\"))\n"
      "  JumpIfZero(Variable(\"main.7\"), main.8_false_label)\n"
      "  Binary(LessThanOrEqualTo, LiteralConstant(5), LiteralConstant(6), Variable(\"main.9\"))\n"
      "  Binary(GreaterThanOrEqualTo, LiteralConstant(7), LiteralConstant(8), Variable(\"main.10\"))\n"
      "  Binary(Equals, Variable(\"main.9\"), Variable(\"main.10\"), Variable(\"main.11\"))\n"
      "  JumpIfZero(Variable(\"main.11\"), main.8_false_label)\n"
      "  Copy(LiteralConstant(1), Variable(\"main.12\"))\n"
      "  Jump(main.13)\n"
      "  Label(main.8_false_label)\n"
      "  Copy(LiteralConstant(0), Variable(\"main.12\"))\n"
      "  Label(main.13)\n"
      "  JumpIfNotZero(Variable(\"main.12\"), main.14_false_label)\n"
      "  Unary(Not, LiteralConstant(2), Variable(\"main.15\"))\n"
      "  JumpIfNotZero(Variable(\"main.15\"), main.14_false_label)\n"
      "  Copy(LiteralConstant(0), Variable(\"main.16\"))\n"
      "  Jump(main.17)\n"
      "  Label(main.14_false_label)\n"
      "  Copy(LiteralConstant(1), Variable(\"main.16\"))\n"
      "  Label(main.17)\n"
      "  Return(Variable(\"main.16\"))\n"
    );
  }
}
