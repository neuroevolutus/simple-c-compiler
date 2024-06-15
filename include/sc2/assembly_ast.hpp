#ifndef SC2_ASSEMBLY_AST_HPP_INCLUDED
#define SC2_ASSEMBLY_AST_HPP_INCLUDED

#include <sc2/ast.hpp>
#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/utility.hpp>

#include <cstddef>
#include <memory>
#include <sstream>
#include <vector>

namespace SC2 {
  struct AssemblyASTNode
    : public std::enable_shared_from_this<AssemblyASTNode>
    , public PrettyPrinterMixin
  {
    virtual ~AssemblyASTNode() = default;
  };

  struct OperandAssemblyASTNode: public AssemblyASTNode
  {
    virtual ~OperandAssemblyASTNode() override = default;
  };

  struct RegisterAssemblyASTNode: public OperandAssemblyASTNode
  {
    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) override
    {
      out << "Register %eax";
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

    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) override
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
    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) override
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

    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) override
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
      , instructions{ instructions }
    {}

    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) override
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
    ProgramAssemblyASTNode(std::shared_ptr<FunctionAssemblyASTNode> function)
      : function{ function }
    {}

    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) override
    {
      out << "Program:\n";
      function->prettyPrintHelper(out, indent_level + 2);
    }
    virtual ~ProgramAssemblyASTNode() override = default;
  };
} // namespace SC2

#endif
