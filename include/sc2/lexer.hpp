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
        std::format("Lexer error: invalid token: {}", invalid_program_text)
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
    static std::regex plus_sign_regex;
    static std::regex asterisk_regex;
    static std::regex forward_slash_regex;
    static std::regex percent_sign_regex;
    static std::regex bitwise_and_regex;
    static std::regex bitwise_or_regex;
    static std::regex bitwise_xor_regex;
    static std::regex left_shift_regex;
    static std::regex right_shift_regex;
    static std::regex exclamation_point_regex;
    static std::regex double_ampersand_regex;
    static std::regex double_pipe_regex;
    static std::regex equal_to_regex;
    static std::regex not_equal_to_regex;
    static std::regex less_than_regex;
    static std::regex greater_than_regex;
    static std::regex less_than_or_equal_to_regex;
    static std::regex greater_than_or_equal_to_regex;
    static std::regex assignment_regex;
    static std::regex add_assignment_regex;
    static std::regex subtract_assignment_regex;
    static std::regex multiply_assignment_regex;
    static std::regex divide_assignment_regex;
    static std::regex modulo_assignment_regex;
    static std::regex bitwise_and_assignment_regex;
    static std::regex bitwise_or_assignment_regex;
    static std::regex bitwise_xor_assignment_regex;
    static std::regex left_shift_assignment_regex;
    static std::regex right_shift_assignment_regex;
    static std::regex increment_regex;
    static std::regex comma_regex;

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
        std::cmatch plus_sign_regex_match{};
        std::cmatch asterisk_regex_match{};
        std::cmatch forward_slash_regex_match{};
        std::cmatch percent_sign_regex_match{};
        std::cmatch bitwise_and_regex_match{};
        std::cmatch bitwise_or_regex_match{};
        std::cmatch bitwise_xor_regex_match{};
        std::cmatch left_shift_regex_match{};
        std::cmatch right_shift_regex_match{};
        std::cmatch exclamation_point_regex_match{};
        std::cmatch double_ampersand_regex_match{};
        std::cmatch double_pipe_regex_match{};
        std::cmatch equal_to_regex_match{};
        std::cmatch not_equal_to_regex_match{};
        std::cmatch less_than_regex_match{};
        std::cmatch greater_than_regex_match{};
        std::cmatch less_than_or_equal_to_regex_match{};
        std::cmatch greater_than_or_equal_to_regex_match{};
        std::cmatch assignment_regex_match{};
        std::cmatch add_assignment_regex_match{};
        std::cmatch subtract_assignment_regex_match{};
        std::cmatch multiply_assignment_regex_match{};
        std::cmatch divide_assignment_regex_match{};
        std::cmatch modulo_assignment_regex_match{};
        std::cmatch bitwise_and_assignment_regex_match{};
        std::cmatch bitwise_or_assignment_regex_match{};
        std::cmatch bitwise_xor_assignment_regex_match{};
        std::cmatch left_shift_assignment_regex_match{};
        std::cmatch right_shift_assignment_regex_match{};
        std::cmatch increment_regex_match{};
        std::cmatch comma_regex_match{};
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
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          plus_sign_regex_match,
          plus_sign_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          asterisk_regex_match,
          asterisk_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          forward_slash_regex_match,
          forward_slash_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          percent_sign_regex_match,
          percent_sign_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          bitwise_and_regex_match,
          bitwise_and_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          bitwise_or_regex_match,
          bitwise_or_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          bitwise_xor_regex_match,
          bitwise_xor_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          left_shift_regex_match,
          left_shift_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          right_shift_regex_match,
          right_shift_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          exclamation_point_regex_match,
          exclamation_point_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          double_ampersand_regex_match,
          double_ampersand_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          double_pipe_regex_match,
          double_pipe_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          equal_to_regex_match,
          equal_to_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          not_equal_to_regex_match,
          not_equal_to_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          less_than_regex_match,
          less_than_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          greater_than_regex_match,
          greater_than_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          less_than_or_equal_to_regex_match,
          less_than_or_equal_to_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          greater_than_or_equal_to_regex_match,
          greater_than_or_equal_to_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          assignment_regex_match,
          assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          add_assignment_regex_match,
          add_assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          subtract_assignment_regex_match,
          subtract_assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          multiply_assignment_regex_match,
          multiply_assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          divide_assignment_regex_match,
          divide_assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          modulo_assignment_regex_match,
          modulo_assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          bitwise_and_assignment_regex_match,
          bitwise_and_assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          bitwise_or_assignment_regex_match,
          bitwise_or_assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          bitwise_xor_assignment_regex_match,
          bitwise_xor_assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          left_shift_assignment_regex_match,
          left_shift_assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          right_shift_assignment_regex_match,
          right_shift_assignment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          increment_regex_match,
          increment_regex
        );
        std::regex_search(
          program_text.c_str(),
          program_text.c_str() + program_text.size(),
          comma_regex_match,
          comma_regex
        );
        std::array<std::cmatch::difference_type, 39> const match_sizes{
          identifier_regex_match.length(),
          literal_constant_regex_match.length(),
          parenthesis_regex_match.length(),
          brace_regex_match.length(),
          semicolon_regex_match.length(),
          tilde_regex_match.length(),
          hyphen_regex_match.length(),
          decrement_regex_match.length(),
          plus_sign_regex_match.length(),
          asterisk_regex_match.length(),
          forward_slash_regex_match.length(),
          percent_sign_regex_match.length(),
          bitwise_and_regex_match.length(),
          bitwise_or_regex_match.length(),
          bitwise_xor_regex_match.length(),
          left_shift_regex_match.length(),
          right_shift_regex_match.length(),
          exclamation_point_regex_match.length(),
          double_ampersand_regex_match.length(),
          double_pipe_regex_match.length(),
          equal_to_regex_match.length(),
          not_equal_to_regex_match.length(),
          less_than_regex_match.length(),
          greater_than_regex_match.length(),
          less_than_or_equal_to_regex_match.length(),
          greater_than_or_equal_to_regex_match.length(),
          assignment_regex_match.length(),
          add_assignment_regex_match.length(),
          subtract_assignment_regex_match.length(),
          multiply_assignment_regex_match.length(),
          divide_assignment_regex_match.length(),
          modulo_assignment_regex_match.length(),
          bitwise_and_assignment_regex_match.length(),
          bitwise_or_assignment_regex_match.length(),
          bitwise_xor_assignment_regex_match.length(),
          left_shift_assignment_regex_match.length(),
          right_shift_assignment_regex_match.length(),
          increment_regex_match.length(),
          comma_regex_match.length()
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
          } else if (identifier == "typedef") {
            current_token = Token(std::make_shared<TypedefKeywordToken>());
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
        } else if (plus_sign_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<PlusSignToken>());
        } else if (asterisk_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<AsteriskToken>());
        } else if (forward_slash_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<ForwardSlashToken>());
        } else if (percent_sign_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<PercentSignToken>());
        } else if (bitwise_and_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<BitwiseAndToken>());
        } else if (bitwise_or_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<BitwiseOrToken>());
        } else if (bitwise_xor_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<BitwiseXorToken>());
        } else if (left_shift_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<LeftShiftToken>());
        } else if (right_shift_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<RightShiftToken>());
        } else if (exclamation_point_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<ExclamationPointToken>());
        } else if (double_ampersand_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<DoubleAmpersandToken>());
        } else if (double_pipe_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<DoublePipeToken>());
        } else if (equal_to_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<EqualToToken>());
        } else if (not_equal_to_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<NotEqualToToken>());
        } else if (less_than_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<LessThanToken>());
        } else if (greater_than_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<GreaterThanToken>());
        } else if (less_than_or_equal_to_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<LessThanOrEqualToToken>());
        } else if (greater_than_or_equal_to_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<GreaterThanOrEqualToToken>());
        } else if (assignment_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<AssignmentToken>());
        } else if (add_assignment_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<AddAssignmentToken>());
        } else if (subtract_assignment_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<SubtractAssignmentToken>());
        } else if (multiply_assignment_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<MultiplyAssignmentToken>());
        } else if (divide_assignment_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<DivideAssignmentToken>());
        } else if (modulo_assignment_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<ModuloAssignmentToken>());
        } else if (bitwise_and_assignment_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<BitwiseAndAssignmentToken>());
        } else if (bitwise_or_assignment_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<BitwiseOrAssignmentToken>());
        } else if (bitwise_xor_assignment_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<BitwiseXorAssignmentToken>());
        } else if (left_shift_assignment_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<LeftShiftAssignmentToken>());
        } else if (right_shift_assignment_regex_match.length()
                   == largest_match_size) {
          current_token = Token(std::make_shared<RightShiftAssignmentToken>());
        } else if (increment_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<IncrementToken>());
        } else if (comma_regex_match.length() == largest_match_size) {
          current_token = Token(std::make_shared<CommaToken>());
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
