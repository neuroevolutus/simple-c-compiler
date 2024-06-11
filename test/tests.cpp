#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <sc2/lexer.hpp>
#include <string_view>

#include <array>
#include <iostream>
#include <iterator>
#include <ranges>
#include <variant>
#include <vector>

TEST_CASE("lexer behaves correctly")
{
  SECTION("whitespace is ignored")
  {
    SC2::Lexer lexer{ " \n\r\t" };
    for (auto const &token: lexer) { REQUIRE(false); }
  }
  SECTION("lexer throws exceptions after end of file")
  {
    SC2::Lexer empty_lexer{ "" };
    auto      &empty_iterator{ empty_lexer.begin() };
    for (; empty_iterator != empty_lexer.end(); ++empty_iterator) {
      REQUIRE(false);
    }
    REQUIRE_THROWS_MATCHES(
      *empty_iterator,
      SC2::LexerEOFError,
      Catch::Matchers::Message("Lexer error: reached end of file")
    );
    REQUIRE_THROWS_MATCHES(
      ++empty_iterator,
      SC2::LexerEOFError,
      Catch::Matchers::Message("Lexer error: reached end of file")
    );
    SC2::Lexer keyword_lexer{ "int return void" };
    auto      &keyword_iterator{ keyword_lexer.begin() };
    for (; keyword_iterator != keyword_lexer.end(); ++keyword_iterator);
    REQUIRE_THROWS_MATCHES(
      *keyword_iterator,
      SC2::LexerEOFError,
      Catch::Matchers::Message("Lexer error: reached end of file")
    );
    REQUIRE_THROWS_MATCHES(
      ++keyword_iterator,
      SC2::LexerEOFError,
      Catch::Matchers::Message("Lexer error: reached end of file")
    );
  }
  SECTION("keywords are correctly lexed")
  {
    SC2::Lexer              lexer{ "int return void" };
    std::vector<SC2::Token> tokens{};
    for (auto const &token: lexer) { tokens.push_back(token); }
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].getKeyword() == SC2::Keyword::INT);
    REQUIRE(tokens[1].getKeyword() == SC2::Keyword::RETURN);
    REQUIRE(tokens[2].getKeyword() == SC2::Keyword::VOID);
  }
  SECTION(
    "identifiers are correctly lexed and not confused with keyword prefixes"
  )
  {
    SC2::Lexer              lexer{ "int9_ER return9_ER void9_ER" };
    std::vector<SC2::Token> tokens{};
    for (auto const &token: lexer) { tokens.push_back(token); }
    REQUIRE(tokens[0].getIdentifier().getName() == "int9_ER");
    REQUIRE(tokens[1].getIdentifier().getName() == "return9_ER");
    REQUIRE(tokens[2].getIdentifier().getName() == "void9_ER");
  }
  SECTION("literal constants are correctly lexed")
  {
    SC2::Lexer              lexer{ "123456789 123 12341" };
    std::vector<SC2::Token> tokens{};
    for (auto const &token: lexer) { tokens.push_back(token); }
    REQUIRE(tokens[0].getLiteralConstant().getValue() == 123456789);
    REQUIRE(tokens[1].getLiteralConstant().getValue() == 123);
    REQUIRE(tokens[2].getLiteralConstant().getValue() == 12341);
  }
  SECTION("literal constants are only lexed when followed by EOF or spaces")
  {
    SC2::Lexer invalid_lexer_zero{ "1234a" };
    REQUIRE_THROWS_MATCHES(
      invalid_lexer_zero.begin(),
      SC2::InvalidTokenError,
      Catch::Matchers::Message("Lexer error: Invalid token: 1234a")
    );
    SC2::Lexer invalid_lexer_one{ "1234A" };
    REQUIRE_THROWS_MATCHES(
      invalid_lexer_one.begin(),
      SC2::InvalidTokenError,
      Catch::Matchers::Message("Lexer error: Invalid token: 1234A")
    );
    SC2::Lexer invalid_lexer_two{ "1234_" };
    REQUIRE_THROWS_MATCHES(
      invalid_lexer_two.begin(),
      SC2::InvalidTokenError,
      Catch::Matchers::Message("Lexer error: Invalid token: 1234_")
    );
  }
  SECTION("parentheses are correctly lexed")
  {
    SC2::Lexer              lexer{ "( )" };
    std::vector<SC2::Token> tokens{};
    for (auto const &token: lexer) { tokens.push_back(token); }
    REQUIRE(tokens[0].getParenthesis() == SC2::Parenthesis::LEFT_PARENTHESIS);
    REQUIRE(tokens[1].getParenthesis() == SC2::Parenthesis::RIGHT_PARENTHESIS);
  }
  SECTION("braces are correctly lexed")
  {
    SC2::Lexer              lexer{ "{ }" };
    std::vector<SC2::Token> tokens{};
    for (auto const &token: lexer) { tokens.push_back(token); }
    REQUIRE(tokens[0].getBrace() == SC2::Brace::LEFT_BRACE);
    REQUIRE(tokens[1].getBrace() == SC2::Brace::RIGHT_BRACE);
  }
  SECTION("semicolons are correctly lexed")
  {
    SC2::Lexer              lexer{ ";;;" };
    std::vector<SC2::Token> tokens{};
    for (auto const &token: lexer) { tokens.push_back(token); }
    REQUIRE(tokens[0].getSemicolon() == SC2::Semicolon);
    REQUIRE(tokens[1].getSemicolon() == SC2::Semicolon);
    REQUIRE(tokens[2].getSemicolon() == SC2::Semicolon);
  }
  SECTION("a full program is correctly lexed")
  {
    char const * const program{
      "int main(void) {\n"
      "  return 2;\n"
      "}"
    };
    SC2::Lexer              lexer{ program };
    std::vector<SC2::Token> tokens{};
    for (auto const &token: lexer) { tokens.push_back(token); }
    REQUIRE(tokens[0].getKeyword() == SC2::Keyword::INT);
    REQUIRE(tokens[1].getIdentifier().getName() == "main");
    REQUIRE(tokens[2].getParenthesis() == SC2::Parenthesis::LEFT_PARENTHESIS);
    REQUIRE(tokens[3].getKeyword() == SC2::Keyword::VOID);
    REQUIRE(tokens[4].getParenthesis() == SC2::Parenthesis::RIGHT_PARENTHESIS);
    REQUIRE(tokens[5].getBrace() == SC2::Brace::LEFT_BRACE);
    REQUIRE(tokens[6].getKeyword() == SC2::Keyword::RETURN);
    REQUIRE(tokens[7].getLiteralConstant().getValue() == 2);
    REQUIRE(tokens[8].getSemicolon() == SC2::Semicolon);
    REQUIRE(tokens[9].getBrace() == SC2::Brace::RIGHT_BRACE);
  }
}
