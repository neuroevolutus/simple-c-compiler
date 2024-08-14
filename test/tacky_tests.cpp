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
  SECTION("a basic program is correctly translated")
  {
    constexpr char const * const program_text_zero{
      "int main(void) {\n"
      " return -(~2);\n"
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
      " return ~12;\n"
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
}
