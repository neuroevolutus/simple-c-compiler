#include <sc2/lexer.hpp>

#include <regex>

namespace SC2 {
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

  std::regex
    Lexer::tilde_regex("^~", std::regex::ECMAScript | std::regex::optimize);

  std::regex
    Lexer::hyphen_regex("^-", std::regex::ECMAScript | std::regex::optimize);

  std::regex Lexer::decrement_regex(
    "^--",
    std::regex::ECMAScript | std::regex::optimize
  );

  std::regex Lexer::plus_sign_regex(
    "^\\+",
    std::regex::ECMAScript | std::regex::optimize
  );

  std::regex Lexer::asterisk_regex(
    "^\\*",
    std::regex::ECMAScript | std::regex::optimize
  );

  std::regex Lexer::forward_slash_regex(
    "^/",
    std::regex::ECMAScript | std::regex::optimize
  );

  std::regex Lexer::percent_sign_regex(
    "^%",
    std::regex::ECMAScript | std::regex::optimize
  );

  std::regex Lexer::bitwise_and_regex(
    "^&",
    std::regex::ECMAScript | std::regex::optimize
  );

  std::regex Lexer::bitwise_or_regex(
    "^\\|",
    std::regex::ECMAScript | std::regex::optimize
  );

  std::regex Lexer::bitwise_xor_regex(
    "^\\^",
    std::regex::ECMAScript | std::regex::optimize
  );

  std::regex Lexer::left_shift_regex(
    "^<<",
    std::regex::ECMAScript | std::regex::optimize
  );

  std::regex Lexer::right_shift_regex(
    "^>>",
    std::regex::ECMAScript | std::regex::optimize
  );
} // namespace SC2
