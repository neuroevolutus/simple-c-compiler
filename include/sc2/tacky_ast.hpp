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
#include <stdexcept>
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

  struct OperandAssemblyASTNode;
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

  class NotTACKYASTNode final: public UnaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printUnaryOperator(std::ostream &out) final override
    {
      out << "Not";
    }

    public:
    [[nodiscard]] virtual std::shared_ptr<UnaryOperatorAssemblyASTNode>
    emitAssembly() const final override
    {
      throw std::runtime_error{ "Not implemented" };
    }

    virtual ~NotTACKYASTNode() final override = default;
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

  struct BinaryOperatorTACKYASTNodeEmitAssemblyInput
  {
    std::shared_ptr<ValueTACKYASTNode> const    left_operand{};
    std::shared_ptr<ValueTACKYASTNode> const    right_operand{};
    std::shared_ptr<VariableTACKYASTNode> const destination{};
  };

  class BinaryTACKYASTNode;
  class BinaryOperatorTACKYASTNode: public TACKYASTNode
  {
    protected:
    virtual void printBinaryOperator(std::ostream &) = 0;

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input) const = 0;

    constexpr virtual void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      printBinaryOperator(out);
    }

    virtual ~BinaryOperatorTACKYASTNode() override = default;
  };

  class AddTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "Add";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
    ) const final override;

    virtual ~AddTACKYASTNode() final override = default;
  };

  class SubtractTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "Subtract";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
    ) const final override;

    virtual ~SubtractTACKYASTNode() final override = default;
  };

  class MultiplyTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "Multiply";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
    ) const final override;

    virtual ~MultiplyTACKYASTNode() final override = default;
  };

  class DivideTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "Divide";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
    ) const final override;

    virtual ~DivideTACKYASTNode() final override = default;
  };

  class ModuloTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "Modulo";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
    ) const final override;

    virtual ~ModuloTACKYASTNode() final override = default;
  };

  class BitwiseAndTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "BitwiseAnd";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
    ) const final override;

    virtual ~BitwiseAndTACKYASTNode() final override = default;
  };

  class BitwiseOrTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "BitwiseOr";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
    ) const final override;

    virtual ~BitwiseOrTACKYASTNode() final override = default;
  };

  class BitwiseXorTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "BitwiseXor";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
    ) const final override;

    virtual ~BitwiseXorTACKYASTNode() final override = default;
  };

  class LeftShiftTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "LeftShift";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
    ) const final override;

    virtual ~LeftShiftTACKYASTNode() final override = default;
  };

  class RightShiftTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "RightShift";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&input
    ) const final override;

    virtual ~RightShiftTACKYASTNode() final override = default;
  };

  class EqualsTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "Equals";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&)
      const final override
    {
      throw std::runtime_error("Not implemented");
    }

    virtual ~EqualsTACKYASTNode() final override = default;
  };

  class NotEqualsTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "NotEquals";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&)
      const final override
    {
      throw std::runtime_error("Not implemented");
    }

    virtual ~NotEqualsTACKYASTNode() final override = default;
  };

  class LessThanTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "LessThan";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&)
      const final override
    {
      throw std::runtime_error("Not implemented");
    }

    virtual ~LessThanTACKYASTNode() final override = default;
  };

  class GreaterThanTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "GreaterThan";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&)
      const final override
    {
      throw std::runtime_error("Not implemented");
    }

    virtual ~GreaterThanTACKYASTNode() final override = default;
  };

  class LessThanOrEqualToTACKYASTNode final: public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "LessThanOrEqualTo";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&)
      const final override
    {
      throw std::runtime_error("Not implemented");
    }

    virtual ~LessThanOrEqualToTACKYASTNode() final override = default;
  };

  class GreaterThanOrEqualToTACKYASTNode final
    : public BinaryOperatorTACKYASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "GreaterThanOrEqualTo";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly(BinaryOperatorTACKYASTNodeEmitAssemblyInput &&)
      const final override
    {
      throw std::runtime_error("Not implemented");
    }

    virtual ~GreaterThanOrEqualToTACKYASTNode() final override = default;
  };

  class BinaryTACKYASTNode final: public InstructionTACKYASTNode
  {
    std::shared_ptr<BinaryOperatorTACKYASTNode> const binary_operator{};
    std::shared_ptr<ValueTACKYASTNode> const          left_operand{};
    std::shared_ptr<ValueTACKYASTNode> const          right_operand{};
    std::shared_ptr<VariableTACKYASTNode> const       destination{};

    [[nodiscard]] std::shared_ptr<BinaryOperatorTACKYASTNode>
    getBinaryOperator() const
    {
      return binary_operator;
    }

    [[nodiscard]] std::shared_ptr<ValueTACKYASTNode> getLeftOperand() const
    {
      return left_operand;
    }

    [[nodiscard]] std::shared_ptr<ValueTACKYASTNode> getRightOperand() const
    {
      return right_operand;
    }

    [[nodiscard]] std::shared_ptr<VariableTACKYASTNode> getDestination() const
    {
      return destination;
    }

    public:
    BinaryTACKYASTNode(
      std::shared_ptr<BinaryOperatorTACKYASTNode> binary_operator,
      std::shared_ptr<ValueTACKYASTNode>          left_operand,
      std::shared_ptr<ValueTACKYASTNode>          right_operand,
      std::shared_ptr<VariableTACKYASTNode>       destination
    )
      : binary_operator{ binary_operator }
      , left_operand{ left_operand }
      , right_operand{ right_operand }
      , destination{ destination }
    {}

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly() const final override;

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << "Binary(";
      getBinaryOperator()->prettyPrintHelper(out, indent_level);
      out << ", ";
      getLeftOperand()->prettyPrintHelper(out, indent_level);
      out << ", ";
      getRightOperand()->prettyPrintHelper(out, indent_level);
      out << ", ";
      getDestination()->prettyPrintHelper(out, indent_level);
      out << ")\n";
    }

    virtual ~BinaryTACKYASTNode() final override = default;
  };

  class CopyTACKYASTNode final: public InstructionTACKYASTNode
  {
    std::shared_ptr<ValueTACKYASTNode> const    source{};
    std::shared_ptr<VariableTACKYASTNode> const destination{};

    [[nodiscard]] std::shared_ptr<ValueTACKYASTNode> getSource() const
    {
      return source;
    }

    [[nodiscard]] std::shared_ptr<VariableTACKYASTNode> getDestination() const
    {
      return destination;
    }

    public:
    CopyTACKYASTNode(
      std::shared_ptr<ValueTACKYASTNode>    source,
      std::shared_ptr<VariableTACKYASTNode> destination
    )
      : source{ source }
      , destination{ destination }
    {}

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly() const final override
    {
      throw std::runtime_error{ "Not implemented" };
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Copy(";
      getSource()->prettyPrintHelper(out, indent_level);
      out << ", ";
      getDestination()->prettyPrintHelper(out, indent_level);
      out << ")\n";
    }

    virtual ~CopyTACKYASTNode() final override = default;
  };

  class JumpTACKYASTNode final: public InstructionTACKYASTNode
  {
    std::string const identifier{};

    [[nodiscard]] std::string_view getIdentifier() const { return identifier; }

    public:
    JumpTACKYASTNode(std::string_view identifier): identifier{ identifier } {}

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly() const final override
    {
      throw std::runtime_error{ "Not implemented" };
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Jump(" << getIdentifier() << ")\n";
    }

    virtual ~JumpTACKYASTNode() final override = default;
  };

  class JumpIfZeroTACKYASTNode final: public InstructionTACKYASTNode
  {
    std::shared_ptr<ValueTACKYASTNode> const condition{};
    std::string const                        identifier{};

    [[nodiscard]] std::shared_ptr<ValueTACKYASTNode> getCondition() const
    {
      return condition;
    }

    [[nodiscard]] std::string_view getIdentifier() const { return identifier; }

    public:
    JumpIfZeroTACKYASTNode(
      std::shared_ptr<ValueTACKYASTNode> condition,
      std::string_view                   identifier
    )
      : condition{ condition }
      , identifier{ identifier }
    {}

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly() const final override
    {
      throw std::runtime_error{ "Not implemented" };
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "JumpIfZero(";
      getCondition()->prettyPrintHelper(out, indent_level);
      out << ", " << getIdentifier() << ")\n";
    }

    virtual ~JumpIfZeroTACKYASTNode() final override = default;
  };

  class JumpIfNotZeroTACKYASTNode final: public InstructionTACKYASTNode
  {
    std::shared_ptr<ValueTACKYASTNode> const condition{};
    std::string const                        identifier{};

    [[nodiscard]] std::shared_ptr<ValueTACKYASTNode> getCondition() const
    {
      return condition;
    }

    [[nodiscard]] std::string_view getIdentifier() const { return identifier; }

    public:
    JumpIfNotZeroTACKYASTNode(
      std::shared_ptr<ValueTACKYASTNode> condition,
      std::string_view                   identifier
    )
      : condition{ condition }
      , identifier{ identifier }
    {}

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly() const final override
    {
      throw std::runtime_error{ "Not implemented" };
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "JumpIfNotZero(";
      getCondition()->prettyPrintHelper(out, indent_level);
      out << ", " << getIdentifier() << ")\n";
    }

    virtual ~JumpIfNotZeroTACKYASTNode() final override = default;
  };

  class LabelTACKYASTNode final: public InstructionTACKYASTNode
  {
    std::string const identifier{};

    [[nodiscard]] std::string_view getIdentifier() const { return identifier; }

    public:
    LabelTACKYASTNode(std::string_view identifier): identifier{ identifier } {}

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    emitAssembly() const final override
    {
      throw std::runtime_error{ "Not implemented" };
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Label(" << getIdentifier() << ")\n";
    }

    virtual ~LabelTACKYASTNode() final override = default;
  };

  class FunctionAssemblyASTNode;
  class FunctionTACKYASTNode final: public TACKYASTNode
  {
    std::string const                                     identifier{};
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions{};

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

    public:
    constexpr FunctionTACKYASTNode(
      std::string_view                                      identifier,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions
    )
      : identifier{ identifier }
      , instructions{ std::move(instructions) }
    {}

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

    [[nodiscard]] std::shared_ptr<FunctionTACKYASTNode>
    getFunction() const noexcept
    {
      return function;
    }

    public:
    explicit ProgramTACKYASTNode(std::shared_ptr<FunctionTACKYASTNode> function)
      : function{ function }
    {}

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
