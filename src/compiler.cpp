#include <sc2/assembly_ast.hpp>
#include <sc2/assembly_generator.hpp>
#include <sc2/lexer.hpp>
#include <sc2/parser.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

void exit_with_error_message()
{
  char const * const error_message{
    "Error: Usage: sc2 [-(-(lex|parse|codegen)|S)] /path/to/file.c\n"
  };
  std::cerr << error_message;
  std::exit(EXIT_FAILURE);
}

int main(int argc, char const * const * const argv)
{
  if (argc < 2 || argc > 3)
    throw std::runtime_error("Wrong number of arguments");
  try {
    auto const &[option, preprocessed_file]{ [argc, argv] {
      if (argc == 3) {
        if (argv[1] != "--lex" && argv[1] != "--parse" && argv[1] != "--codegen"
            && argv[1] != "-S")
          throw std::invalid_argument("Invalid long option");
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
    for (auto const &token: dummy_lexer);
    if (option && *option == "--lex") return EXIT_SUCCESS;
    SC2::Lexer  lexer{ program_text };
    SC2::Parser parser{ lexer };
    auto const  program{ parser.parseProgram() };
    if (option && *option == "--parse") return EXIT_SUCCESS;
    auto const assembly{ SC2::AssemblyGenerator::generateProgramAssembly(program
    ) };
    if (option && *option == "--codegen") return EXIT_SUCCESS;
    auto output_file_stream{ std::ofstream(file_basename + ".s") };
    assembly->emitCode(output_file_stream);
  } catch (std::exception const &exception) {
    std::cerr << exception.what() << '\n';
    exit_with_error_message();
  }
  return EXIT_SUCCESS;
}
