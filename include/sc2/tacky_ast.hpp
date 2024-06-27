#ifndef TACKY_AST_HPP_INCLUDED
#define TACKY_AST_HPP_INCLUDED

#include <sc2/assembly_ast.hpp>
#include <sc2/ast.hpp>
#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/utility.hpp>
#include <string_view>

#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <span>
#include <string>
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
    [[nodiscard]] virtual std::shared_ptr<OperandAssemblyASTNode>
    emitAssembly() const                  = 0;
    virtual ~ValueTACKYASTNode() override = default;
  };

  class LiteralConstantTACKYASTNode final: public ValueTACKYASTNode
  {
    int const value{};

    public:
    explicit constexpr LiteralConstantTACKYASTNode(int value): value{ value } {}

    [[nodiscard]] constexpr int getValue() const noexcept { return value; }

    [[nodiscard]] virtual std::shared_ptr<OperandAssemblyASTNode>
    emitAssembly() const final override;

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "LiteralConstant(" << getValue() << ')';
    }

    virtual ~LiteralConstantTACKYASTNode() final override = default;
  };

  class VariableTACKYASTNode final: public ValueTACKYASTNode
  {
    std::string const identifier{};

    public:
    explicit constexpr VariableTACKYASTNode(std::string_view identifier)
      : identifier{ identifier }
    {}

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode>
    emitAssembly() const final override;

    [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept
    {
      return identifier;
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "Variable(\"" << getIdentifier() << "\")";
    }

    virtual ~VariableTACKYASTNode() final override = default;
  };

  struct InstructionAssemblyASTNode;
  struct InstructionTACKYASTNode: public TACKYASTNode
  {
    [[nodiscard]] virtual constexpr std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly() const = 0;

    virtual ~InstructionTACKYASTNode() override = default;
  };

  class ReturnTACKYASTNode final: public InstructionTACKYASTNode
  {
    std::shared_ptr<ValueTACKYASTNode> value{};

    public:
    explicit ReturnTACKYASTNode(std::shared_ptr<ValueTACKYASTNode> value)
      : value{ value }
    {}

    [[nodiscard]] std::shared_ptr<ValueTACKYASTNode> getValue() const
    {
      return value;
    }

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly() const final override;

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << "Return(";
      getValue()->prettyPrintHelper(out, indent_level);
      out << ")\n";
    }

    virtual ~ReturnTACKYASTNode() final override = default;
  };

  class UnaryOperatorAssemblyASTNode;
  class UnaryOperatorTACKYASTNode: public TACKYASTNode
  {
    protected:
    virtual void printUnaryOperator(std::ostream &) = 0;

    public:
    [[nodiscard]] virtual std::shared_ptr<UnaryOperatorAssemblyASTNode>
    emitAssembly() const = 0;

    constexpr virtual void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      printUnaryOperator(out);
    }

    virtual ~UnaryOperatorTACKYASTNode() override = default;
  };

  class ComplementAssemblyASTNode;
  class ComplementTACKYASTNode final: public UnaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printUnaryOperator(std::ostream &out) final override
    {
      out << "Complement";
    }

    public:
    [[nodiscard]] virtual std::shared_ptr<UnaryOperatorAssemblyASTNode>
    emitAssembly() const final override;

    virtual ~ComplementTACKYASTNode() final override = default;
  };

  class NegateAssemblyASTNode;
  class NegateTACKYASTNode final: public UnaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printUnaryOperator(std::ostream &out) final override
    {
      out << "Negate";
    }

    public:
    [[nodiscard]] virtual std::shared_ptr<UnaryOperatorAssemblyASTNode>
    emitAssembly() const final override;

    virtual ~NegateTACKYASTNode() final override = default;
  };

  class UnaryTACKYASTNode final: public InstructionTACKYASTNode
  {
    std::shared_ptr<UnaryOperatorTACKYASTNode> unary_operator{};
    std::shared_ptr<ValueTACKYASTNode>         source{};
    std::shared_ptr<VariableTACKYASTNode>      destination{};

    public:
    UnaryTACKYASTNode(
      std::shared_ptr<UnaryOperatorTACKYASTNode> unary_operator,
      std::shared_ptr<ValueTACKYASTNode>         source,
      std::shared_ptr<VariableTACKYASTNode>      destination
    )
      : unary_operator{ unary_operator }
      , source{ source }
      , destination{ destination }
    {}

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly() const final override;

    [[nodiscard]] std::shared_ptr<UnaryOperatorTACKYASTNode>
    getUnaryOperator() const
    {
      return unary_operator;
    }

    [[nodiscard]] std::shared_ptr<ValueTACKYASTNode> getSource() const
    {
      return source;
    }

    [[nodiscard]] std::shared_ptr<VariableTACKYASTNode> getDestination() const
    {
      return destination;
    }

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << "Unary(";
      getUnaryOperator()->prettyPrintHelper(out, indent_level);
      out << ", ";
      getSource()->prettyPrintHelper(out, indent_level);
      out << ", ";
      getDestination()->prettyPrintHelper(out, indent_level);
      out << ")\n";
    }

    virtual ~UnaryTACKYASTNode() final override = default;
  };

  class FunctionAssemblyASTNode;
  class FunctionTACKYASTNode final: public TACKYASTNode
  {
    std::string const                                     identifier{};
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions{};

    public:
    constexpr FunctionTACKYASTNode(
      std::string_view                                      identifier,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions
    )
      : identifier{ identifier }
      , instructions{ std::move(instructions) }
    {}

    [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept
    {
      return identifier;
    }

    [[nodiscard]] constexpr std::span<
      std::shared_ptr<InstructionTACKYASTNode> const>
    getInstructions() const noexcept
    {
      return instructions;
    }

    [[nodiscard]] std::shared_ptr<FunctionAssemblyASTNode> emitAssembly() const;

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      out << "Function: " << getIdentifier() << "\n";
      for (auto &instruction: getInstructions()) {
        instruction->prettyPrintHelper(out, indent_level + 2);
      }
    }

    virtual ~FunctionTACKYASTNode() final override = default;
  };

  class ProgramAssemblyASTNode;
  class ProgramTACKYASTNode final: public TACKYASTNode
  {
    std::shared_ptr<FunctionTACKYASTNode> function{};

    public:
    explicit ProgramTACKYASTNode(std::shared_ptr<FunctionTACKYASTNode> function)
      : function{ function }
    {}

    [[nodiscard]] std::shared_ptr<FunctionTACKYASTNode>
    getFunction() const noexcept
    {
      return function;
    }

    [[nodiscard]] std::shared_ptr<ProgramAssemblyASTNode> emitAssembly() const;

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      getFunction()->prettyPrintHelper(out, indent_level);
    }

    virtual ~ProgramTACKYASTNode() final override = default;
  };
} // namespace SC2

#endif
