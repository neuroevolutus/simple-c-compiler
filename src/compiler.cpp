#include <sc2/assembly_ast.hpp>
#include <sc2/ast.hpp>
#include <sc2/compiler_error.hpp>
#include <sc2/lexer.hpp>
#include <sc2/parser.hpp>
#include <sc2/tacky_ast.hpp>

#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <print>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

constexpr char const * const usage_error_message{
  "Error: Usage: sc2 [-(-(lex|parse|codegen|tacky)|S)] /path/to/file.c\n"
};

void exit_with_usage_error_message()
{
  std::cerr << usage_error_message;
  std::exit(EXIT_FAILURE);
}

int main(int argc, char const * const * const argv)
{
  using namespace std::literals::string_literals;
  if (argc < 2 || argc > 3)
    throw std::runtime_error("Wrong number of arguments");
  try {
    auto const &[option, preprocessed_file]{ [argc, argv] {
      if (argc == 3) {
        if (argv[1] != "--lex"s && argv[1] != "--parse"s
            && argv[1] != "--codegen"s && argv[1] != "--tacky"s
            && argv[1] != "--validate"s && argv[1] != "-S"s)
          throw std::invalid_argument(
            std::format("Invalid long option: {}", argv[1])
          );
        return std::make_pair(
          std::optional{ std::string(argv[1]) },
          std::string(argv[2])
        );
      } else
        return std::make_pair(
          std::optional<std::string>{},
          std::string(argv[1])
        );
    }() };
    auto const &file_basename{
      preprocessed_file.substr(0, preprocessed_file.length() - 2)
    };
    auto const &file_stream{ std::ifstream(preprocessed_file) };
    if (!file_stream) {
      std::perror("File opening failed");
      return EXIT_FAILURE;
    }
    std::ostringstream program_buffer{};
    program_buffer << file_stream.rdbuf();
    std::string const program_text{ program_buffer.str() };
    SC2::Lexer        dummy_lexer{ program_text };
    for (auto const &token: dummy_lexer); // NOLINT
    if (option && *option == "--lex") return EXIT_SUCCESS;
    SC2::Lexer  lexer{ program_text };
    SC2::Parser parser{ lexer };
    auto const  program{ parser.parseProgram() };
    if (option && (*option == "--parse" || *option == "--validate"))
      return EXIT_SUCCESS;
    auto const tacky{ program->emitTACKY() };
    if (option && *option == "--tacky") return EXIT_SUCCESS;
    auto const assembly{ tacky->emitAssembly() };
    auto const [last_offset, _, cleaned_assembly]{
      assembly->replacePseudoRegisters()
    };
    auto const fixed_assembly{ cleaned_assembly->fixUp(-last_offset) };
    if (option && *option == "--codegen") return EXIT_SUCCESS;
    auto output_file_stream{ std::ofstream(file_basename + ".s") };
    fixed_assembly->emitCode(output_file_stream);
  } catch (SC2::CompilerError const &exception) {
    std::cerr << "Compiler error:\n" << exception.what();
    std::exit(EXIT_FAILURE);
  } catch (std::exception const &exception) {
    std::cerr << "Usage error:\n" << exception.what() << '\n';
    exit_with_usage_error_message();
  }
  return EXIT_SUCCESS;
}
