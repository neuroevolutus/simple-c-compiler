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
          "Parser error: invalid non-terminal <statement>:\n"
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
          "Lexer error: invalid token: <"
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
          "Parser error: invalid non-terminal <statement>:\n"
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
          "Parser error: invalid non-terminal <statement>:\n"
          "Parser error: invalid non-terminal <expression>:\n"
          "Parser error: invalid non-terminal <expression>:\n"
          "Parser error: invalid non-terminal <factor>:\n"
          "Parser error: expected (left parenthesis) but got (plus sign)"
        )
      );
    }
  }
}
