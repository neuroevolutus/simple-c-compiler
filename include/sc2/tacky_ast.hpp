#ifndef TACKY_AST_HPP_INCLUDED
#define TACKY_AST_HPP_INCLUDED

#include <sc2/ast.hpp>
#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/tokens.hpp>
#include <sc2/utility.hpp>

#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

namespace SC2 {
  struct TACKYASTNode
    : public std::enable_shared_from_this<TACKYASTNode>
    , public PrettyPrinterMixin
  {
    virtual ~TACKYASTNode() = default;
  };

  struct ValueTACKYASTNode: public TACKYASTNode
  {
    virtual ~ValueTACKYASTNode() override = default;
  };

  class LiteralConstantTACKYASTNode: public ValueTACKYASTNode
  {
    LiteralConstant const literal_constant{};

    public:
    explicit constexpr LiteralConstantTACKYASTNode(
      LiteralConstant const literal_constant
    )
      : literal_constant{ literal_constant }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      out << "LiteralConstant(" << literal_constant.getValue() << ')';
    }

    virtual ~LiteralConstantTACKYASTNode() override = default;
  };

  class VariableTACKYASTNode: public ValueTACKYASTNode
  {
    Identifier const identifier{};

    public:
    explicit constexpr VariableTACKYASTNode(Identifier const identifier)
      : identifier{ identifier }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      out << "Variable(\"" << identifier.getName() << "\")";
    }

    virtual ~VariableTACKYASTNode() override = default;
  };

  struct InstructionTACKYASTNode: public TACKYASTNode
  {
    virtual ~InstructionTACKYASTNode() override = default;
  };

  class ReturnTACKYASTNode: public InstructionTACKYASTNode
  {
    std::shared_ptr<ValueTACKYASTNode> value{};

    public:
    explicit constexpr ReturnTACKYASTNode(
      std::shared_ptr<ValueTACKYASTNode> value
    )
      : value{ value }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      Utility::indent(out, indent_level);
      out << "Return(";
      value->prettyPrintHelper(out, indent_level);
      out << ")\n";
    }

    virtual ~ReturnTACKYASTNode() override = default;
  };

  class UnaryTACKYASTNode: public InstructionTACKYASTNode
  {
    UnaryOperator                         unary_operator{};
    std::shared_ptr<ValueTACKYASTNode>    source{};
    std::shared_ptr<VariableTACKYASTNode> destination{};

    public:
    constexpr UnaryTACKYASTNode(
      UnaryOperator                         unary_operator,
      std::shared_ptr<ValueTACKYASTNode>    source,
      std::shared_ptr<VariableTACKYASTNode> destination
    )
      : unary_operator{ unary_operator }
      , source{ source }
      , destination{ destination }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      Utility::indent(out, indent_level);
      out << "Unary(";
      switch (unary_operator) {
        case UnaryOperator::COMPLEMENT: out << "Complement"; break;
        case UnaryOperator::NEGATE: out << "Negate"; break;
        default: std::unreachable();
      }
      out << ", ";
      source->prettyPrintHelper(out, indent_level);
      out << ", ";
      destination->prettyPrintHelper(out, indent_level);
      out << ")\n";
    }

    virtual ~UnaryTACKYASTNode() override = default;
  };

  class FunctionTACKYASTNode: public TACKYASTNode
  {
    Identifier const                                      identifier{};
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions{};

    public:
    constexpr FunctionTACKYASTNode(
      Identifier                                            identifier,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions
    )
      : identifier{ identifier }
      , instructions{ std::move(instructions) }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      out << "Function: " << identifier.getName() << "\n";
      for (auto &instruction: instructions) {
        instruction->prettyPrintHelper(out, indent_level + 2);
      }
    }

    virtual ~FunctionTACKYASTNode() override = default;
  };

  class ProgramTACKYASTNode: public TACKYASTNode
  {
    std::shared_ptr<FunctionTACKYASTNode> function{};

    public:
    explicit constexpr ProgramTACKYASTNode(
      std::shared_ptr<FunctionTACKYASTNode> function
    )
      : function{ function }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      function->prettyPrintHelper(out, indent_level);
    }

    virtual ~ProgramTACKYASTNode() override = default;
  };
} // namespace SC2

#endif
