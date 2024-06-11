#ifndef SC2_LEXER_HPP_INCLUDED
#define SC2_LEXER_HPP_INCLUDED

#include <sc2/tokens.hpp>
#include <string_view>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <exception>
#include <generator>
#include <iostream>
#include <iterator>
#include <regex>
#include <stdexcept>
#include <utility>
#include <vector>

namespace SC2 {

  struct LexerError: public std::exception
  {
    virtual ~LexerError() = default;
  };

  class LexerInvalidTokenError: public LexerError
  {
    std::string const message{};

    public:
    explicit constexpr LexerInvalidTokenError(
      std::string_view const invalid_program_text
    )
      : message{
        std::format("Lexer error: Invalid token: {}", invalid_program_text)
      }
    {}
    virtual constexpr char const *what() const noexcept override
    {
      return message.c_str();
    }
    virtual ~LexerInvalidTokenError() override = default;
  };

  struct LexerEOFError: public LexerError
  {
    virtual constexpr char const *what() const noexcept override
    {
      return "Lexer error: reached end of file";
    }
    virtual ~LexerEOFError() override = default;
  };

  class Lexer
  {
    static std::regex whitespace_prefix_regex;
    static std::regex identifier_regex;
    static std::regex literal_constant_regex;
    static std::regex parenthesis_regex;
    static std::regex brace_regex;
    static std::regex semicolon_regex;

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

    constexpr Lexer(Lexer const &) = default;

    [[nodiscard]] constexpr Lexer &begin() noexcept(noexcept(operator++()))
    {
      return *this;
    }

    [[nodiscard]] constexpr std::default_sentinel_t end() noexcept(false)
    {
      return std::default_sentinel;
    }

    [[nodiscard]] constexpr Token operator*() noexcept(false)
    {
      if (finished) throw LexerEOFError{};
      return current_token;
    }

    [[nodiscard]] constexpr Token *operator->() noexcept(false)
    {
      if (finished) throw LexerEOFError{};
      return &current_token;
    }

    [[nodiscard]] constexpr bool operator==(std::default_sentinel_t
    ) const noexcept
    {
      return finished;
    }

    constexpr Lexer operator++(int) noexcept(noexcept(operator++()))
    {
      auto const result{ *this };
      operator++();
      return result;
    }

    constexpr Lexer &operator++() noexcept(false)
    {
      if (finished) throw LexerEOFError{};
      clearWhitespaceFromStartOfProgramText();
      if (program_text.size() > 0) {
        std::cmatch identifier_regex_match{};
        std::cmatch literal_constant_regex_match{};
        std::cmatch parenthesis_regex_match{};
        std::cmatch brace_regex_match{};
        std::cmatch semicolon_regex_match{};
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
        std::vector<std::cmatch::difference_type> const match_sizes{
          identifier_regex_match.length(),
          literal_constant_regex_match.length(),
          parenthesis_regex_match.length(),
          brace_regex_match.length(),
          semicolon_regex_match.length()
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
            current_token = Token(Keyword::INT);
          } else if (identifier == "return") {
            current_token = Token(Keyword::RETURN);
          } else if (identifier == "void") {
            current_token = Token(Keyword::VOID);
          } else
            current_token = Token(Identifier(identifier));
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
          current_token = Token(LiteralConstant(literal_constant));
        } else if (parenthesis_regex_match.length() == largest_match_size) {
          auto const &parenthesis{ parenthesis_regex_match.str() };
          if (parenthesis == "(")
            current_token = Token(Parenthesis::LEFT_PARENTHESIS);
          else if (parenthesis == ")")
            current_token = Token(Parenthesis::RIGHT_PARENTHESIS);
          else
            std::unreachable();
        } else if (brace_regex_match.length() == largest_match_size) {
          auto const &brace{ brace_regex_match.str() };
          if (brace == "{")
            current_token = Token(Brace::LEFT_BRACE);
          else if (brace == "}")
            current_token = Token(Brace::RIGHT_BRACE);
          else
            std::unreachable();
        } else if (semicolon_regex_match.length() == largest_match_size) {
          current_token = Token(Semicolon);
        } else
          std::unreachable();
        program_text.erase(0, largest_match_size);
      } else
        finished = true;
      return *this;
    }
  };

  std::regex Lexer::whitespace_prefix_regex(
    "^(\\s+)",
    std::regex::ECMAScript | std::regex::optimize
  );
  std::regex Lexer::identifier_regex(
    "^[a-zA-Z_]\\w*\\b",
    std::regex::ECMAScript | std::regex::optimize
  );
  std::regex Lexer::literal_constant_regex(
    "^[0-9]+\\b",
    std::regex::ECMAScript | std::regex::optimize
  );
  std::regex Lexer::parenthesis_regex(
    "^(\\(|\\))",
    std::regex::ECMAScript | std::regex::optimize
  );
  std::regex Lexer::brace_regex(
    "^(\\{|\\})",
    std::regex::ECMAScript | std::regex::optimize
  );
  std::regex
    Lexer::semicolon_regex("^;", std::regex::ECMAScript | std::regex::optimize);

} // namespace SC2

#endif
