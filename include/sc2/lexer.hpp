#ifndef SC2_LEXER_HPP_INCLUDED
#define SC2_LEXER_HPP_INCLUDED

#include <sc2/compiler_error.hpp>
#include <sc2/tokens.hpp>
#include <string_view>

#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <exception>
#include <iostream>
#include <iterator>
#include <memory>
#include <regex>
#include <stdexcept>
#include <utility>

namespace SC2 {

  struct LexerError: public CompilerError
  {
    virtual ~LexerError() = default;
  };

  class LexerInvalidTokenError final: public LexerError
  {
    std::string const message{};

    public:
    explicit LexerInvalidTokenError(std::string_view const invalid_program_text)
      : message{
        std::format("Lexer error: Invalid token: {}", invalid_program_text)
      }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~LexerInvalidTokenError() final override = default;
  };

  struct LexerEOFError final: public LexerError
  {
    virtual constexpr char const *what() const noexcept final override
    {
      return "Lexer error: reached end of file";
    }
    virtual ~LexerEOFError() final override = default;
  };

  class Lexer
  {
    static std::regex whitespace_prefix_regex;
    static std::regex identifier_regex;
    static std::regex literal_constant_regex;
    static std::regex parenthesis_regex;
    static std::regex brace_regex;
    static std::regex semicolon_regex;
    static std::regex tilde_regex;
    static std::regex hyphen_regex;
    static std::regex decrement_regex;

    std::string program_text{};
    Token       current_token;
    bool        finished{};

    void clearWhitespaceFromStartOfProgramText() noexcept
    {
      if (std::cmatch whitespace_prefix_match{}; std::regex_search(
            program_text.c_str(),
            program_text.c_str() + program_text.size(),
            whitespace_prefix_match,
            whitespace_prefix_regex
          ))
        program_text.erase(0, whitespace_prefix_match.length());
    }

    public:
    explicit constexpr Lexer(std::string_view program_text)
      : program_text{ program_text }
    {
      operator++();
    }

    Lexer(Lexer const &) = default;

    [[nodiscard]] constexpr Lexer &begin() noexcept(noexcept(operator++()))
    {
      return *this;
    }

    [[nodiscard]] constexpr std::default_sentinel_t end()
    {
      return std::default_sentinel;
    }

    [[nodiscard]] Token operator*()
    {
      if (finished) throw LexerEOFError{};
      return current_token;
    }

    [[nodiscard]] constexpr Token *operator->()
    {
      if (finished) throw LexerEOFError{};
      return &current_token;
    }

    [[nodiscard]] constexpr bool operator==(std::default_sentinel_t
    ) const noexcept
    {
      return finished;
    }

    Lexer operator++(int) noexcept(noexcept(operator++()))
    {
      auto const result{ *this };
      operator++();
      return result;
    }

    constexpr Lexer &operator++()
    {
      if (finished) throw LexerEOFError{};
      clearWhitespaceFromStartOfProgramText();
      if (program_text.size() > 0) {
        std::cmatch identifier_regex_match{};
        std::cmatch literal_constant_regex_match{};
        std::cmatch parenthesis_regex_match{};
        std::cmatch brace_regex_match{};
        std::cmatch semicolon_regex_match{};
        std::cmatch tilde_regex_match{};
        std::cmatch hyphen_regex_match{};
        std::cmatch decrement_regex_match{};
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          identifier_regex_match,
          identifier_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          literal_constant_regex_match,
          literal_constant_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          parenthesis_regex_match,
          parenthesis_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          brace_regex_match,
          brace_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          semicolon_regex_match,
          semicolon_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          tilde_regex_match,
          tilde_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          hyphen_regex_match,
          hyphen_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          decrement_regex_match,
          decrement_regex
        );
        std::array<std::cmatch::difference_type, 8> const match_sizes{
          identifier_regex_match.length(),
          literal_constant_regex_match.length(),
          parenthesis_regex_match.length(),
          brace_regex_match.length(),
          semicolon_regex_match.length(),
          tilde_regex_match.length(),
          hyphen_regex_match.length(),
          decrement_regex_match.length()
        };
        std::cmatch::difference_type const largest_match_size{
          match_sizes[std::ranges::distance(
            match_sizes.cbegin(),
            std::ranges::max_element(match_sizes)
          )]
        };
        if (largest_match_size < 1)
          throw LexerInvalidTokenError(program_text);
        else if (identifier_regex_match.length() == largest_match_size) {
          if (auto const &identifier{ identifier_regex_match.str() };
              identifier == "int") {
            current_token = Token(std::make_shared<IntKeywordToken>());
          } else if (identifier == "return") {
            current_token = Token(std::make_shared<ReturnKeywordToken>());
          } else if (identifier == "void") {
            current_token = Token(std::make_shared<VoidKeywordToken>());
          } else
            current_token
              = Token(std::make_shared<IdentifierToken>(identifier));
        } else if (literal_constant_regex_match.length()
                   == largest_match_size) {
          auto const &literal_constant_string{ literal_constant_regex_match.str(
          ) };
          int         literal_constant{};
          if (std::from_chars_result result{ std::from_chars(
                literal_constant_string.c_str(),
                literal_constant_string.c_str()
                  + literal_constant_string.size(),
                literal_constant,
                10
              ) };
              result.ec == std::errc{}) {
          } else {
            if (result.ec == std::errc::result_out_of_range)
              throw std::invalid_argument(std::format(
                "Literal constant does not fit into domain of int: {}",
                literal_constant_string
              ));
            else
              std::unreachable();
          }
          current_token
            = Token(std::make_shared<LiteralConstantToken>(literal_constant));
        } else if (parenthesis_regex_match.length() == largest_match_size) {
          auto const &parenthesis{ parenthesis_regex_match.str() };
          if (parenthesis == "(")
            current_token = Token(std::make_shared<LeftParenthesisToken>());
          else if (parenthesis == ")")
            current_token = Token(std::make_shared<RightParenthesisToken>());
          else
            std::unreachable();
        } else if (brace_regex_match.length() == largest_match_size) {
          auto const &brace{ brace_regex_match.str() };
          if (brace == "{")
            current_token = Token(std::make_shared<LeftCurlyBraceToken>());
          else if (brace == "}")
            current_token = Token(std::make_shared<RightCurlyBraceToken>());
          else
            std::unreachable();
        } else if (semicolon_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<SemicolonToken>());
        } else if (tilde_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<TildeToken>());
        } else if (hyphen_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<HyphenToken>());
        } else if (decrement_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<DecrementToken>());
        } else
          std::unreachable();
        program_text.erase(0, largest_match_size);
      } else
        finished = true;
      return *this;
    }
  };
} // namespace SC2

#endif
