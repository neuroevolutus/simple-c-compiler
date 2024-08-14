#ifndef SC2_ASSEMBLY_AST_HPP_INCLUDED
#define SC2_ASSEMBLY_AST_HPP_INCLUDED

#include <sc2/ast.hpp>
#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/utility.hpp>

#include <cstddef>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

namespace SC2 {
  struct AssemblyASTNode
    : public std::enable_shared_from_this<AssemblyASTNode>
    , public PrettyPrinterMixin
  {
    virtual constexpr void emitCode(std::ostream &out) = 0;
    virtual ~AssemblyASTNode()                         = default;
  };

  struct OperandAssemblyASTNode: public AssemblyASTNode
  {
    virtual ~OperandAssemblyASTNode() override = default;
  };

  struct RegisterAssemblyASTNode: public OperandAssemblyASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      out << "Register %eax";
    }

    virtual constexpr void emitCode(std::ostream &out) override
    {
      out << "%eax";
    }

    virtual ~RegisterAssemblyASTNode() override = default;
  };

  class ImmediateValueAssemblyASTNode: public OperandAssemblyASTNode
  {
    LiteralConstant const value{};

    public:
    constexpr ImmediateValueAssemblyASTNode(LiteralConstant const value)
      : value{ value }
    {}

    virtual constexpr void emitCode(std::ostream &out) override
    {
      out << "$" << value.getValue();
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      out << "ImmediateValue " << value.getValue();
    }

    virtual ~ImmediateValueAssemblyASTNode() override = default;
  };

  struct InstructionAssemblyASTNode: public AssemblyASTNode
  {
    virtual ~InstructionAssemblyASTNode() override = default;
  };

  struct ReturnAssemblyASTNode: public InstructionAssemblyASTNode
  {
    virtual constexpr void emitCode(std::ostream &out) override
    {
      out << "ret\n";
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      Utility::indent(out, indent_level);
      out << "Instruction: Ret\n";
    }

    virtual ~ReturnAssemblyASTNode() override = default;
  };

  class MovAssemblyASTNode: public InstructionAssemblyASTNode
  {
    std::shared_ptr<OperandAssemblyASTNode> source{};
    std::shared_ptr<OperandAssemblyASTNode> destination{};

    public:
    constexpr MovAssemblyASTNode(
      std::shared_ptr<OperandAssemblyASTNode> source,
      std::shared_ptr<OperandAssemblyASTNode> destination
    )
      : source{ source }
      , destination{ destination }
    {}

    virtual constexpr void emitCode(std::ostream &out) override
    {
      Utility::indent(out, 2);
      out << "movl ";
      source->emitCode(out);
      out << ", ";
      destination->emitCode(out);
      out << '\n';
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      Utility::indent(out, indent_level);
      out << "Instruction: Mov (";
      source->prettyPrintHelper(out, indent_level);
      out << "), (";
      destination->prettyPrintHelper(out, indent_level);
      out << ")\n";
    }

    virtual ~MovAssemblyASTNode() override = default;
  };

  class FunctionAssemblyASTNode: public AssemblyASTNode
  {
    Identifier const                                         name{};
    std::vector<std::shared_ptr<InstructionAssemblyASTNode>> instructions{};

    public:
    constexpr FunctionAssemblyASTNode(
      Identifier const                                         name,
      std::vector<std::shared_ptr<InstructionAssemblyASTNode>> instructions
    )
      : name{ name }
      , instructions{ std::move(instructions) }
    {}

    constexpr std::string getName() const { return name.getName(); }

    constexpr std::vector<std::shared_ptr<InstructionAssemblyASTNode>> const &
    getInstructions() const
    {
      return instructions;
    }

    constexpr void emitCode(std::ostream &out) override
    {
      Utility::indent(out, 2);
      std::string const specialised_function_name{
        Utility::specialiseFunctionNameForOS(getName())
      };
      out << ".globl " << specialised_function_name << '\n';
      out << specialised_function_name << ":\n";
      for (auto const &instruction: getInstructions()) {
        instruction->emitCode(out);
      }
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      Utility::indent(out, indent_level);
      out << "Function: " << name.getName() << "\n";
      for (auto const &instruction: instructions) {
        instruction->prettyPrintHelper(out, indent_level + 2);
      };
    }

    virtual ~FunctionAssemblyASTNode() override = default;
  };

  class ProgramAssemblyASTNode: public AssemblyASTNode
  {
    std::shared_ptr<FunctionAssemblyASTNode> function{};

    public:
    constexpr ProgramAssemblyASTNode(
      std::shared_ptr<FunctionAssemblyASTNode> function
    )
      : function{ function }
    {}

    constexpr std::shared_ptr<FunctionAssemblyASTNode> getFunction() const
    {
      return function;
    }

    virtual constexpr void emitCode(std::ostream &out) override
    {
      function->emitCode(out);
      Utility::emitAssemblyEpilogue(out);
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      out << "Program:\n";
      function->prettyPrintHelper(out, indent_level + 2);
    }

    virtual ~ProgramAssemblyASTNode() override = default;
  };
} // namespace SC2

#endif
