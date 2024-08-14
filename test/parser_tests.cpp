#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <sc2/lexer.hpp>
#include <sc2/parser.hpp>
#include <sc2/test_fixtures.hpp>

#include <memory>

TEST_CASE("parser behaves correctly")
{
  SECTION("Chapter 1")
  {
    SECTION("a basic program is correctly parsed")
    {
      SC2::Lexer                           lexer{ basic_program_text };
      SC2::Parser                          parser{ lexer };
      std::shared_ptr<SC2::ProgramASTNode> program_ast{ parser.parseProgram() };
      REQUIRE(program_ast->prettyPrint() == basic_program_text);
    }
    SECTION("extraneous tokens at the end of a program cause an error")
    {
      constexpr char const * const invalid_program_text{
        "int main(void) {\n"
        "  return 2;\n"
        "}d"
      };
      SC2::Lexer  lexer{ invalid_program_text };
      SC2::Parser parser{ lexer };
      REQUIRE_THROWS_MATCHES(
        parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: extraneous token: (Identifier: d)"
        )
      );
    }
    SECTION("end of file is handled correctly")
    {
      constexpr char const * const invalid_program_text{
        "int main(void) {\n"
        "  return 2;\n"
        ""
      };
      SC2::Lexer  lexer{ invalid_program_text };
      SC2::Parser parser{ lexer };
      REQUIRE_THROWS_MATCHES(
        parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: reached end of file"
        )
      );
    }
    SECTION("invalid tokens are handled correctly")
    {
      constexpr char const * const invalid_program_text_zero{
        "int return(void) {\n"
        "  return 2;\n"
        "}"
      };
      SC2::Lexer  lexer_zero{ invalid_program_text_zero };
      SC2::Parser parser_zero{ lexer_zero };
      REQUIRE_THROWS_MATCHES(
        parser_zero.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: cannot create (identifier) from (Keyword: return)"
        )
      );
      constexpr char const * const invalid_program_text_one{
        "int main(void) {\n"
        "  return 2)\n"
        "}"
      };
      SC2::Lexer  lexer_one{ invalid_program_text_one };
      SC2::Parser parser_one{ lexer_one };
      REQUIRE_THROWS_MATCHES(
        parser_one.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <return statement>:\n"
          "Parser error: expected (semicolon) but got (right parenthesis)"
        )
      );
      constexpr char const * const invalid_program_text_two{
        "int main(void) {\n"
        "  return 2;\n"
        "<"
      };
      SC2::Lexer  lexer_two{ invalid_program_text_two };
      SC2::Parser parser_two{ lexer_two };
      REQUIRE_THROWS_MATCHES(
        parser_two.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <expression statement>:\n"
          "Parser error: invalid non-terminal <expression>:\n"
          "Parser error: invalid non-terminal <factor>:\n"
          "Parser error: expected (left parenthesis) but got (less than)"
        )
      );
    }
  }
  SECTION("Chapter 2")
  {
    SECTION("Unary operators are handled correctly")
    {
      constexpr char const * const valid_program_text{
        "int main(void) {\n"
        "  return -(-2);\n"
        "}\n"
      };
      SC2::Lexer  valid_lexer{ valid_program_text };
      SC2::Parser valid_parser{ valid_lexer };
      REQUIRE(valid_parser.parseProgram()->prettyPrint() ==
        "int main(void) {\n"
        "  return -(-(2));\n"
        "}\n"
      );
      constexpr char const * const invalid_program_text{
        "int main(void) {\n"
        "  return --2;\n"
        "}\n"
      };
      SC2::Lexer  invalid_lexer{ invalid_program_text };
      SC2::Parser invalid_parser{ invalid_lexer };
      REQUIRE_THROWS_AS(
        invalid_parser.parseProgram(),
        SC2::ParserNonTerminalError
      );
    }
    SECTION("Unmatched parentheses are handled correctly")
    {
      constexpr char const * const invalid_program_text{
        "int main(void) {\n"
        "  return -((2);\n"
        "}\n"
      };
      SC2::Lexer  invalid_lexer{ invalid_program_text };
      SC2::Parser invalid_parser{ invalid_lexer };
      REQUIRE_THROWS_MATCHES(
        invalid_parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <return statement>:\n"
          "Parser error: invalid non-terminal <expression>:\n"
          "Parser error: invalid non-terminal <factor>:\n"
          "Parser error: invalid non-terminal <factor>:\n"
          "Parser error: unmatched parentheses"
        )
      );
    }
  }
  SECTION("Chapter 3")
  {
    SECTION("a program with binary expressions is correctly parsed")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  return 2 * 3 / 5 % 7 + 4 << 3 & 7 | 8 ^ 9 | 5 & 3 >> 6 + 8 * 2 * (1 "
        "+ 2 % 2 + 3 & 2) + (3 * 5) - 1;\n"
        "}\n"
      };
      constexpr char const * const prettified_program_text{
        "int main(void) {\n"
        "  return ((((((((2 * 3) / 5) % 7) + 4) << 3) & 7) | (8 ^ 9)) | (5 & "
        "(3 >> (((6 + ((8 * 2) * (((1 + (2 % 2)) + 3) & 2))) + (3 * 5)) - "
        "1))));\n"
        "}\n"
      };
      SC2::Lexer                           lexer{ program_text };
      SC2::Parser                          parser{ lexer };
      std::shared_ptr<SC2::ProgramASTNode> program_ast{ parser.parseProgram() };
      REQUIRE(program_ast->prettyPrint() == prettified_program_text);
    }
    SECTION("a program with with invalid binary expressions is not parsed")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  return 3 * + 4;"
        "}\n"
      };
      SC2::Lexer  lexer{ program_text };
      SC2::Parser parser{ lexer };
      REQUIRE_THROWS_MATCHES(
        parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <return statement>:\n"
          "Parser error: invalid non-terminal <expression>:\n"
          "Parser error: invalid non-terminal <expression>:\n"
          "Parser error: invalid non-terminal <factor>:\n"
          "Parser error: expected (left parenthesis) but got (plus sign)"
        )
      );
    }
  }
  SECTION("Chapter 4")
  {
    constexpr char const * const program_text{
      "int main(void) {\n"
      "  return 1 < 2 != 3 > 4 && 5 <= 6 == 7 >= 8 || !2;\n"
      "}\n"
    };
    constexpr char const * const prettified_program_text{
      "int main(void) {\n"
      "  return ((((1 < 2) != (3 > 4)) && ((5 <= 6) == (7 >= 8))) || !(2));\n"
      "}\n"
    };
    SC2::Lexer                           lexer{ program_text };
    SC2::Parser                          parser{ lexer };
    std::shared_ptr<SC2::ProgramASTNode> program_ast{ parser.parseProgram() };
    REQUIRE(program_ast->prettyPrint() == prettified_program_text);
  }
  SECTION("Chapter 5")
  {
    SECTION("Variable declarations are parsed correctly")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  int a;\n"
        "  int b = 5;\n"
        "}\n"
      };
      constexpr char const * const prettified_program_text{
        "int main(void) {\n"
        "  int main.0.a;\n"
        "  int main.1.b = 5;\n"
        "}\n"
      };
      SC2::Lexer                           lexer{ program_text };
      SC2::Parser                          parser{ lexer };
      std::shared_ptr<SC2::ProgramASTNode> program_ast{ parser.parseProgram() };
      REQUIRE(program_ast->prettyPrint() == prettified_program_text);
    }
    SECTION("Null statements are parsed correctly")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  int a;;\n"
        "  ;;\n"
        "}\n"
      };
      constexpr char const * const prettified_program_text{
        "int main(void) {\n"
        "  int main.2.a;\n"
        "  ;\n"
        "  ;\n"
        "  ;\n"
        "}\n"
      };
      SC2::Lexer                           lexer{ program_text };
      SC2::Parser                          parser{ lexer };
      std::shared_ptr<SC2::ProgramASTNode> program_ast{ parser.parseProgram() };
      REQUIRE(program_ast->prettyPrint() == prettified_program_text);
    }
    SECTION("Expression statements are parsed correctly")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  1 + 2;\n"
        "}\n"
      };
      constexpr char const * const prettified_program_text{
        "int main(void) {\n"
        "  (1 + 2);\n"
        "}\n"
      };
      SC2::Lexer                           lexer{ program_text };
      SC2::Parser                          parser{ lexer };
      std::shared_ptr<SC2::ProgramASTNode> program_ast{ parser.parseProgram() };
      REQUIRE(program_ast->prettyPrint() == prettified_program_text);
    }
    SECTION("Prefix and postfix increment and decrement are parsed correctly")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  int a = 2;\n"
        "  ++a;\n"
        "  --a;\n"
        "  a++;\n"
        "  a--;\n"
        "  ++(a);\n"
        "  --(a);\n"
        "  (a)++;\n"
        "  (a)--;\n"
        "}\n"
      };
      constexpr char const * const prettified_program_text{
        "int main(void) {\n"
        "  int main.3.a = 2;\n"
        "  ++main.3.a;\n"
        "  --main.3.a;\n"
        "  main.3.a++;\n"
        "  main.3.a--;\n"
        "  ++main.3.a;\n"
        "  --main.3.a;\n"
        "  main.3.a++;\n"
        "  main.3.a--;\n"
        "}\n"
      };
      SC2::Lexer                           lexer{ program_text };
      SC2::Parser                          parser{ lexer };
      std::shared_ptr<SC2::ProgramASTNode> program_ast{ parser.parseProgram() };
      REQUIRE(program_ast->prettyPrint() == prettified_program_text);
    }
    SECTION("Compound assignments are parsed correctly")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  int a = 5;\n"
        "  int b = 5;\n"
        "  int c = 5;\n"
        "  int d = 5;\n"
        "  int e = 5;\n"
        "  int f = 5;\n"
        "  int g = 5;\n"
        "  int h = 5;\n"
        "  int i = 5;\n"
        "  int j = 5;\n"
        "  a += b -= c *= d /= e %= f &= g |= h ^= i <<= j >>= 2;\n"
        "}\n"
      };
      constexpr char const * const prettified_program_text{
        "int main(void) {\n"
        "  int main.4.a = 5;\n"
        "  int main.5.b = 5;\n"
        "  int main.6.c = 5;\n"
        "  int main.7.d = 5;\n"
        "  int main.8.e = 5;\n"
        "  int main.9.f = 5;\n"
        "  int main.10.g = 5;\n"
        "  int main.11.h = 5;\n"
        "  int main.12.i = 5;\n"
        "  int main.13.j = 5;\n"
        "  (main.4.a += (main.5.b -= (main.6.c *= (main.7.d /= (main.8.e %= "
        "(main.9.f &= (main.10.g |= (main.11.h ^= (main.12.i <<= (main.13.j "
        ">>= "
        "2))))))))));\n"
        "}\n"
      };
      SC2::Lexer                           lexer{ program_text };
      SC2::Parser                          parser{ lexer };
      std::shared_ptr<SC2::ProgramASTNode> program_ast{ parser.parseProgram() };
      REQUIRE(program_ast->prettyPrint() == prettified_program_text);
    }
    SECTION("Typedefs are parsed correctly")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  typedef int a;\n"
        "  typedef int b, c;\n"
        "  typedef a b;\n"
        "  typedef b a;\n"
        "  a d = 5;\n"
        "  b e = 5;\n"
        "  c f = 5;\n"
        "}\n"
      };
      constexpr char const * const prettified_program_text{
        "int main(void) {\n"
        "  int main.14.d = 5;\n"
        "  int main.15.e = 5;\n"
        "  int main.16.f = 5;\n"
        "}\n"
      };
      SC2::Lexer                           lexer{ program_text };
      SC2::Parser                          parser{ lexer };
      std::shared_ptr<SC2::ProgramASTNode> program_ast{ parser.parseProgram() };
      REQUIRE(program_ast->prettyPrint() == prettified_program_text);
    }
    SECTION("rvalues cannot be used as lvalues")
    {
      constexpr char const * const invalid_program_text_zero{
        "int main(void) {\n"
        "  int a = 5;\n"
        "  int b = 5;\n"
        "  int c = 5;\n"
        "  (a += b) -= c *= 5;\n"
        "}\n"
      };
      SC2::Lexer  lexer_zero{ invalid_program_text_zero };
      SC2::Parser parser_zero{ lexer_zero };
      REQUIRE_THROWS_MATCHES(
        parser_zero.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <expression statement>:\n"
          "Parser error: invalid non-terminal <expression>:\n"
          "Semantic analysis error: invalid lvalue: ((main.17.a += main.18.b))"
          // TODO show original name of variable
        )
      );
      constexpr char const * const invalid_program_text_one{
        "int main(void) {\n"
        "  int a = 5;\n"
        "  int b = 5;\n"
        "  (a += b)++;\n"
        "}\n"
      };
      SC2::Lexer  lexer_one{ invalid_program_text_one };
      SC2::Parser parser_one{ lexer_one };
      REQUIRE_THROWS_MATCHES(
        parser_one.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <expression statement>:\n"
          "Parser error: invalid non-terminal <expression>:\n"
          "Parser error: invalid non-terminal <factor>:\n"
          "Semantic analysis error: invalid lvalue: ((main.20.a += main.21.b))"
          // TODO show original name of variable
        )
      );
      constexpr char const * const invalid_program_text_two{
        "int main(void) {\n"
        "  int a = 5;\n"
        "  int b = 5;\n"
        "  ++(a += b);\n"
        "}\n"
      };
      SC2::Lexer  lexer_two{ invalid_program_text_two };
      SC2::Parser parser_two{ lexer_two };
      REQUIRE_THROWS_MATCHES(
        parser_two.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <expression statement>:\n"
          "Parser error: invalid non-terminal <expression>:\n"
          "Parser error: invalid non-terminal <factor>:\n"
          "Semantic analysis error: invalid lvalue: ((main.22.a += main.23.b))"
          // TODO show original name of variable
        )
      );
    }
    SECTION("variables cannot be redeclared")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  int a = 5;\n"
        "  int a;\n"
        "}\n"
      };
      SC2::Lexer  lexer{ program_text };
      SC2::Parser parser{ lexer };
      REQUIRE_THROWS_MATCHES(
        parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <declaration>:\n"
          "Semantic analysis error: cannot redeclare variable a"
        )
      );
    }
    SECTION("variables must be declared before use")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  a += 4;\n"
        "  int a = 5;\n"
        "}\n"
      };
      SC2::Lexer  lexer{ program_text };
      SC2::Parser parser{ lexer };
      REQUIRE_THROWS_MATCHES(
        parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <expression statement>:\n"
          "Parser error: invalid non-terminal <expression>:\n"
          "Parser error: invalid non-terminal <factor>:\n"
          "Semantic analysis error: undefined variable a"
        )
      );
    }
    SECTION("inconsistent typedefs are not parsed")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  typedef int a;\n"
        "  typedef void b;\n"
        "  typedef a c, b;\n"
        "}\n"
      };
      SC2::Lexer  lexer{ program_text };
      SC2::Parser parser{ lexer };
      REQUIRE_THROWS_MATCHES(
        parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <typedef>:\n"
          "Semantic analysis error: cannot redefine previously defined alias "
          "(b) with type void as an alias to type int"
        )
      );
    }
    SECTION("unknown types in typedefs are rejected")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  typedef int a;\n"
        "  typedef b c;\n"
        "}\n"
      };
      SC2::Lexer  lexer{ program_text };
      SC2::Parser parser{ lexer };
      REQUIRE_THROWS_MATCHES(
        parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <typedef>:\n"
          "Semantic analysis error: unknown type b"
        )
      );
    }
    SECTION("type alias cannot be a primitive type")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  typedef int int;\n"
        "}\n"
      };
      SC2::Lexer  lexer{ program_text };
      SC2::Parser parser{ lexer };
      REQUIRE_THROWS_MATCHES(
        parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <typedef>:\n"
          "Semantic analysis error: cannot use (Keyword: int) as a type alias"
        )
      );
    }
    SECTION("type cannot be redefined as variable")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  typedef int a;\n"
        "  int a = 5;\n"
        "}\n"
      };
      SC2::Lexer  lexer{ program_text };
      SC2::Parser parser{ lexer };
      REQUIRE_THROWS_MATCHES(
        parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <declaration>:\n"
          "Semantic analysis error: cannot redefine name (a) with symbol type "
          "<type> as having symbol type <variable>"
        )
      );
    }
    SECTION("variable cannot be redefined as type")
    {
      constexpr char const * const program_text{
        "int main(void) {\n"
        "  int a = 5;\n"
        "  typedef int a;\n"
        "}\n"
      };
      SC2::Lexer  lexer{ program_text };
      SC2::Parser parser{ lexer };
      REQUIRE_THROWS_MATCHES(
        parser.parseProgram(),
        SC2::ParserNonTerminalError,
        Catch::Matchers::Message(
          "Parser error: invalid non-terminal <program>:\n"
          "Parser error: invalid non-terminal <function>:\n"
          "Parser error: invalid non-terminal <block item>:\n"
          "Parser error: invalid non-terminal <typedef>:\n"
          "Semantic analysis error: cannot redefine name (a) with symbol type "
          "<variable> as having symbol type <type>"
        )
      );
    }
  }
}
