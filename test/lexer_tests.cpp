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
        Catch::Matchers::Message("Lexer error: Invalid token: 1234a")
      );
      REQUIRE_THROWS_MATCHES(
        SC2::Lexer{ "1234A" },
        SC2::LexerInvalidTokenError,
        Catch::Matchers::Message("Lexer error: Invalid token: 1234A")
      );
      REQUIRE_THROWS_MATCHES(
        SC2::Lexer{ "1234_" },
        SC2::LexerInvalidTokenError,
        Catch::Matchers::Message("Lexer error: Invalid token: 1234_")
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
    SECTION("plus signs are correctly lexed")
    {
      SC2::Lexer              lexer{ "+++" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getPlusSign() == SC2::PlusSignToken{});
      REQUIRE(*tokens[1].getPlusSign() == SC2::PlusSignToken{});
      REQUIRE(*tokens[2].getPlusSign() == SC2::PlusSignToken{});
    }
    SECTION("asterisks are correctly lexed")
    {
      SC2::Lexer              lexer{ "***" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getAsterisk() == SC2::AsteriskToken{});
      REQUIRE(*tokens[1].getAsterisk() == SC2::AsteriskToken{});
      REQUIRE(*tokens[2].getAsterisk() == SC2::AsteriskToken{});
    }
    SECTION("forward slashes are correctly lexed")
    {
      SC2::Lexer              lexer{ "///" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getForwardSlash() == SC2::ForwardSlashToken{});
      REQUIRE(*tokens[1].getForwardSlash() == SC2::ForwardSlashToken{});
      REQUIRE(*tokens[2].getForwardSlash() == SC2::ForwardSlashToken{});
    }
    SECTION("percent signs are correctly lexed")
    {
      SC2::Lexer              lexer{ "%%%" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getPercentSign() == SC2::PercentSignToken{});
      REQUIRE(*tokens[1].getPercentSign() == SC2::PercentSignToken{});
      REQUIRE(*tokens[2].getPercentSign() == SC2::PercentSignToken{});
    }
    SECTION("bitwise and is correctly lexed")
    {
      SC2::Lexer              lexer{ "&&&" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getBitwiseAnd() == SC2::BitwiseAndToken{});
      REQUIRE(*tokens[1].getBitwiseAnd() == SC2::BitwiseAndToken{});
      REQUIRE(*tokens[2].getBitwiseAnd() == SC2::BitwiseAndToken{});
    }
    SECTION("bitwise or is correctly lexed")
    {
      SC2::Lexer              lexer{ "|||" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getBitwiseOr() == SC2::BitwiseOrToken{});
      REQUIRE(*tokens[1].getBitwiseOr() == SC2::BitwiseOrToken{});
      REQUIRE(*tokens[2].getBitwiseOr() == SC2::BitwiseOrToken{});
    }
    SECTION("bitwise xor is correctly lexed")
    {
      SC2::Lexer              lexer{ "^^^" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getBitwiseXor() == SC2::BitwiseXorToken{});
      REQUIRE(*tokens[1].getBitwiseXor() == SC2::BitwiseXorToken{});
      REQUIRE(*tokens[2].getBitwiseXor() == SC2::BitwiseXorToken{});
    }
    SECTION("left shifts are correctly lexed")
    {
      SC2::Lexer              lexer{ "<< << <<" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getLeftShift() == SC2::LeftShiftToken{});
      REQUIRE(*tokens[1].getLeftShift() == SC2::LeftShiftToken{});
      REQUIRE(*tokens[2].getLeftShift() == SC2::LeftShiftToken{});
    }
    SECTION("right shifts are correctly lexed")
    {
      SC2::Lexer              lexer{ ">> >> >>" };
      std::vector<SC2::Token> tokens{};
      for (auto const &token: lexer) { tokens.push_back(token); }
      REQUIRE(*tokens[0].getRightShift() == SC2::RightShiftToken{});
      REQUIRE(*tokens[1].getRightShift() == SC2::RightShiftToken{});
      REQUIRE(*tokens[2].getRightShift() == SC2::RightShiftToken{});
    }
  }
}
