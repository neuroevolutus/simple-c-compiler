#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <sc2/ast.hpp>
#include <sc2/lexer.hpp>
#include <sc2/parser.hpp>
#include <sc2/test_fixtures.hpp>
#include <string_view>

#include <array>
#include <iostream>
#include <iterator>
#include <memory>
#include <ranges>
#include <variant>
#include <vector>

TEST_CASE("lexer behaves correctly")
{
  SECTION("Chapter 1")
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
      REQUIRE(*tokens[0].getIntKeyword() == SC2::IntKeywordToken{});
      REQUIRE(*tokens[1].getReturnKeyword() == SC2::ReturnKeywordToken{});
      REQUIRE(*tokens[2].getVoidKeyword() == SC2::VoidKeywordToken{});
    }
    SECTION(
      "identifiers are correctly lexed and not confused with keyword prefixes"
    )
    {
      SC2::Lexer              lexer{ "int9_ER return9_ER void9_ER" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(tokens[0].getIdentifier()->getName() == "int9_ER");
      REQUIRE(tokens[1].getIdentifier()->getName() == "return9_ER");
      REQUIRE(tokens[2].getIdentifier()->getName() == "void9_ER");
    }
    SECTION("literal constants are correctly lexed")
    {
      SC2::Lexer              lexer{ "123456789 123 12341" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(
        *tokens[0].getLiteralConstant() == SC2::LiteralConstantToken(123456789)
      );
      REQUIRE(
        *tokens[1].getLiteralConstant() == SC2::LiteralConstantToken(123)
      );
      REQUIRE(
        *tokens[2].getLiteralConstant() == SC2::LiteralConstantToken(12341)
      );
    }
    SECTION("literal constants are only lexed when followed by EOF or spaces")
    {
      REQUIRE_THROWS_MATCHES(
        SC2::Lexer{ "1234a" },
        SC2::LexerInvalidTokenError,
        Catch::Matchers::Message("Lexer error: invalid token: 1234a")
      );
      REQUIRE_THROWS_MATCHES(
        SC2::Lexer{ "1234A" },
        SC2::LexerInvalidTokenError,
        Catch::Matchers::Message("Lexer error: invalid token: 1234A")
      );
      REQUIRE_THROWS_MATCHES(
        SC2::Lexer{ "1234_" },
        SC2::LexerInvalidTokenError,
        Catch::Matchers::Message("Lexer error: invalid token: 1234_")
      );
    }
    SECTION("parentheses are correctly lexed")
    {
      SC2::Lexer              lexer{ "( )" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getLeftParenthesis() == SC2::LeftParenthesisToken{});
      REQUIRE(*tokens[1].getRightParenthesis() == SC2::RightParenthesisToken{});
    }
    SECTION("braces are correctly lexed")
    {
      SC2::Lexer              lexer{ "{ }" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getLeftCurlyBrace() == SC2::LeftCurlyBraceToken{});
      REQUIRE(*tokens[1].getRightCurlyBrace() == SC2::RightCurlyBraceToken{});
    }
    SECTION("semicolons are correctly lexed")
    {
      SC2::Lexer              lexer{ ";;;" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getSemicolon() == SC2::SemicolonToken{});
      REQUIRE(*tokens[1].getSemicolon() == SC2::SemicolonToken{});
      REQUIRE(*tokens[2].getSemicolon() == SC2::SemicolonToken{});
    }
    SECTION("a basic program is correctly lexed")
    {
      SC2::Lexer              lexer{ basic_program_text };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getIntKeyword() == SC2::IntKeywordToken{});
      REQUIRE(tokens[1].getIdentifier()->getName() == "main");
      REQUIRE(*tokens[2].getLeftParenthesis() == SC2::LeftParenthesisToken{});
      REQUIRE(*tokens[3].getVoidKeyword() == SC2::VoidKeywordToken{});
      REQUIRE(*tokens[4].getRightParenthesis() == SC2::RightParenthesisToken{});
      REQUIRE(*tokens[5].getLeftCurlyBrace() == SC2::LeftCurlyBraceToken{});
      REQUIRE(*tokens[6].getReturnKeyword() == SC2::ReturnKeywordToken{});
      REQUIRE(*tokens[7].getLiteralConstant() == SC2::LiteralConstantToken(2));
      REQUIRE(*tokens[8].getSemicolon() == SC2::SemicolonToken{});
      REQUIRE(*tokens[9].getRightCurlyBrace() == SC2::RightCurlyBraceToken{});
    }
  }
  SECTION("Chapter 2")
  {
    SECTION("tildes are correctly lexed")
    {
      SC2::Lexer              lexer{ "~~~" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getTilde() == SC2::TildeToken{});
      REQUIRE(*tokens[1].getTilde() == SC2::TildeToken{});
      REQUIRE(*tokens[2].getTilde() == SC2::TildeToken{});
    }
    SECTION("hyphens are correctly lexed")
    {
      SC2::Lexer              lexer{ "-abc123-hello" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getHyphen() == SC2::HyphenToken{});
      REQUIRE(*tokens[1].getIdentifier() == SC2::IdentifierToken("abc123"));
      REQUIRE(*tokens[2].getHyphen() == SC2::HyphenToken{});
      REQUIRE(*tokens[3].getIdentifier() == SC2::IdentifierToken("hello"));
    }
    SECTION("decrements are correctly lexed")
    {
      SC2::Lexer              lexer{ "---abc123--hello" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getDecrement() == SC2::DecrementToken{});
      REQUIRE(*tokens[1].getHyphen() == SC2::HyphenToken{});
      REQUIRE(*tokens[2].getIdentifier() == SC2::IdentifierToken("abc123"));
      REQUIRE(*tokens[3].getDecrement() == SC2::DecrementToken{});
      REQUIRE(*tokens[4].getIdentifier() == SC2::IdentifierToken("hello"));
    }
  }
  SECTION("Chapter 3")
  {
    SECTION("basic arithmetic operator tokens are correctly lexed")
    {
      SC2::Lexer              lexer{ "+*/%" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getPlusSign() == SC2::PlusSignToken{});
      REQUIRE(*tokens[1].getAsterisk() == SC2::AsteriskToken{});
      REQUIRE(*tokens[2].getForwardSlash() == SC2::ForwardSlashToken{});
      REQUIRE(*tokens[3].getPercentSign() == SC2::PercentSignToken{});
    }
    SECTION("bitwise operator tokens are correctly lexed")
    {
      SC2::Lexer              lexer{ "&|^" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getBitwiseAnd() == SC2::BitwiseAndToken{});
      REQUIRE(*tokens[1].getBitwiseOr() == SC2::BitwiseOrToken{});
      REQUIRE(*tokens[2].getBitwiseXor() == SC2::BitwiseXorToken{});
    }
    SECTION("arithmetic shift operator tokens are correctly lexed")
    {
      SC2::Lexer              lexer{ "<<>>" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getLeftShift() == SC2::LeftShiftToken{});
      REQUIRE(*tokens[1].getRightShift() == SC2::RightShiftToken{});
    }
  }
  SECTION("Chapter 4")
  {
    SECTION("Logical operator tokens are correctly lexed")
    {
      SC2::Lexer              lexer{ "!&&||" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getExclamationPoint() == SC2::ExclamationPointToken{});
      REQUIRE(*tokens[1].getDoubleAmpersand() == SC2::DoubleAmpersandToken{});
      REQUIRE(*tokens[2].getDoublePipe() == SC2::DoublePipeToken{});
    }
    SECTION("Comparison operator tokens are correctly lexed")
    {
      SC2::Lexer              lexer{ "!===" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getNotEqualTo() == SC2::NotEqualToToken{});
      REQUIRE(*tokens[1].getEqualTo() == SC2::EqualToToken{});
    }
    SECTION("Relational operator tokens are correctly lexed")
    {
      SC2::Lexer              lexer{ "< <= > >=" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getLessThan() == SC2::LessThanToken{});
      REQUIRE(
        *tokens[1].getLessThanOrEqualTo() == SC2::LessThanOrEqualToToken{}
      );
      REQUIRE(*tokens[2].getGreaterThan() == SC2::GreaterThanToken{});
      REQUIRE(
        *tokens[3].getGreaterThanOrEqualTo() == SC2::GreaterThanOrEqualToToken{}
      );
    }
  }
  SECTION("Chapter 5")
  {
    SECTION("Assignment operators are correctly lexed")
    {
      SC2::Lexer              lexer{ "= += -= *= /= %= &= |= ^= <<= >>=" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getAssignment() == SC2::AssignmentToken{});
      REQUIRE(*tokens[1].getAddAssignment() == SC2::AddAssignmentToken{});
      REQUIRE(
        *tokens[2].getSubtractAssignment() == SC2::SubtractAssignmentToken{}
      );
      REQUIRE(
        *tokens[3].getMultiplyAssignment() == SC2::MultiplyAssignmentToken{}
      );
      REQUIRE(*tokens[4].getDivideAssignment() == SC2::DivideAssignmentToken{});
      REQUIRE(*tokens[5].getModuloAssignment() == SC2::ModuloAssignmentToken{});
      REQUIRE(
        *tokens[6].getBitwiseAndAssignment() == SC2::BitwiseAndAssignmentToken{}
      );
      REQUIRE(
        *tokens[7].getBitwiseOrAssignment() == SC2::BitwiseOrAssignmentToken{}
      );
      REQUIRE(
        *tokens[8].getBitwiseXorAssignment() == SC2::BitwiseXorAssignmentToken{}
      );
      REQUIRE(
        *tokens[9].getLeftShiftAssignment() == SC2::LeftShiftAssignmentToken{}
      );
      REQUIRE(
        *tokens[10].getRightShiftAssignment()
        == SC2::RightShiftAssignmentToken{}
      );
    }
    SECTION("Increment is correctly lexed")
    {
      SC2::Lexer lexer{ "++" };
      REQUIRE(*(*lexer).getIncrement() == SC2::IncrementToken{});
    }
    SECTION("typedef keyword is correctly lexed")
    {
      SC2::Lexer lexer{ "typedef" };
      REQUIRE(*(*lexer).getTypedefKeyword() == SC2::TypedefKeywordToken{});
    }
    SECTION("comma is correctly lexed")
    {
      SC2::Lexer lexer{ "," };
      REQUIRE(*(*lexer).getComma() == SC2::CommaToken{});
    }
  }
}
