#ifndef SC2_AST_HPP_INCLUDED
#define SC2_AST_HPP_INCLUDED

#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/utility.hpp>
#include <string_view>

#include <cstddef>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace SC2 {
  struct ASTNode
    : public std::enable_shared_from_this<ASTNode>
    , public PrettyPrinterMixin
  {
    virtual ~ASTNode() = default;
  };

  struct InstructionTACKYASTNode;
  struct ExpressionASTNodeEmitTACKYInput
  {
    std::string_view                                      identifier{};
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions{};
  };

  struct ValueTACKYASTNode;
  struct ExpressionASTNodeEmitTACKYOutput
  {
    std::shared_ptr<ValueTACKYASTNode>                    output_value{};
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions{};
  };

  struct ExpressionASTNode: public ASTNode
  {
    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
      emitTACKY(ExpressionASTNodeEmitTACKYInput) const
      = 0;

    virtual ~ExpressionASTNode() override = default;
  };

  class LiteralConstantASTNode final: public ExpressionASTNode
  {
    int const value{};

    public:
    explicit constexpr LiteralConstantASTNode(int value): value{ value } {}

    [[nodiscard]] constexpr int getValue() const noexcept { return value; }

    [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
      emitTACKY(ExpressionASTNodeEmitTACKYInput) const final override;

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << getValue();
    }

    virtual ~LiteralConstantASTNode() final override = default;
  };

  class UnaryOperatorTACKYASTNode;
  struct UnaryOperatorASTNode: public ASTNode
  {
    virtual std::shared_ptr<UnaryOperatorTACKYASTNode> emitTACKY() const = 0;

    virtual ~UnaryOperatorASTNode() override = default;
  };

  struct ComplementASTNode final: public UnaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "~";
    }

    virtual std::shared_ptr<UnaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~ComplementASTNode() final override = default;
  };

  struct NegateASTNode final: public UnaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "-";
    }

    virtual std::shared_ptr<UnaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~NegateASTNode() final override = default;
  };

  class UnaryExpressionASTNode final: public ExpressionASTNode
  {
    std::shared_ptr<UnaryOperatorASTNode> const unary_operator{};
    std::shared_ptr<ExpressionASTNode> const    expression{};

    public:
    UnaryExpressionASTNode(
      std::shared_ptr<UnaryOperatorASTNode> unary_operator,
      std::shared_ptr<ExpressionASTNode>    expression
    )
      : unary_operator{ unary_operator }
      , expression{ expression }
    {}

    [[nodiscard]] std::shared_ptr<UnaryOperatorASTNode> getUnaryOperator() const
    {
      return unary_operator;
    }

    [[nodiscard]] std::shared_ptr<ExpressionASTNode> getExpression() const
    {
      return expression;
    }

    [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
      emitTACKY(ExpressionASTNodeEmitTACKYInput) const final override;

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      getUnaryOperator()->prettyPrintHelper(out, indent_level);
      out << '(';
      getExpression()->prettyPrintHelper(out, indent_level);
      out << ')';
    }

    virtual ~UnaryExpressionASTNode() final override = default;
  };

  class BinaryOperatorTACKYASTNode;
  struct BinaryOperatorASTNode: public ASTNode
  {
    virtual std::shared_ptr<BinaryOperatorTACKYASTNode> emitTACKY() const = 0;

    virtual ~BinaryOperatorASTNode() override = default;
  };

  struct AddASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '+';
    }

    virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~AddASTNode() final override = default;
  };

  struct SubtractASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '-';
    }

    virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~SubtractASTNode() final override = default;
  };

  struct MultiplyASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '*';
    }

    virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~MultiplyASTNode() final override = default;
  };

  struct DivideASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '/';
    }

    virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~DivideASTNode() final override = default;
  };

  struct ModuloASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '%';
    }

    virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~ModuloASTNode() final override = default;
  };

  struct BitwiseAndASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '&';
    }

    virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~BitwiseAndASTNode() final override = default;
  };

  struct BitwiseOrASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '|';
    }

    virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~BitwiseOrASTNode() final override = default;
  };

  struct BitwiseXorASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '^';
    }

    virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~BitwiseXorASTNode() final override = default;
  };

  struct LeftShiftASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "<<";
    }

    virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~LeftShiftASTNode() final override = default;
  };

  struct RightShiftASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << ">>";
    }

    virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~RightShiftASTNode() final override = default;
  };

  class BinaryExpressionASTNode final: public ExpressionASTNode
  {
    std::shared_ptr<BinaryOperatorASTNode> const binary_operator{};
    std::shared_ptr<ExpressionASTNode> const     left_operand{};
    std::shared_ptr<ExpressionASTNode> const     right_operand{};

    public:
    BinaryExpressionASTNode(
      std::shared_ptr<BinaryOperatorASTNode> binary_operator,
      std::shared_ptr<ExpressionASTNode>     left_operand,
      std::shared_ptr<ExpressionASTNode>     right_operand
    )
      : binary_operator{ binary_operator }
      , left_operand{ left_operand }
      , right_operand{ right_operand }
    {}

    [[nodiscard]] std::shared_ptr<BinaryOperatorASTNode>
    getBinaryOperator() const
    {
      return binary_operator;
    }

    [[nodiscard]] std::shared_ptr<ExpressionASTNode> getLeftOperand() const
    {
      return left_operand;
    }

    [[nodiscard]] std::shared_ptr<ExpressionASTNode> getRightOperand() const
    {
      return right_operand;
    }

    [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
      emitTACKY(ExpressionASTNodeEmitTACKYInput) const final override;

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      out << '(';
      left_operand->prettyPrintHelper(out, indent_level);
      out << ' ';
      binary_operator->prettyPrintHelper(out, indent_level);
      out << ' ';
      right_operand->prettyPrintHelper(out, indent_level);
      out << ')';
    }

    virtual ~BinaryExpressionASTNode() final override = default;
  };

  class StatementASTNode final: public ASTNode
  {
    std::shared_ptr<ExpressionASTNode> expression{};

    public:
    explicit StatementASTNode(std::shared_ptr<ExpressionASTNode> expression)
      : expression{ expression }
    {}

    [[nodiscard]] std::shared_ptr<ExpressionASTNode>
    getExpression() const noexcept
    {
      return expression;
    }

    [[nodiscard]] std::vector<std::shared_ptr<InstructionTACKYASTNode>>
      emitTACKY(std::string_view) const;

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << "return ";
      getExpression()->prettyPrintHelper(out, indent_level);
      out << ";\n";
    }

    virtual ~StatementASTNode() final override = default;
  };

  class FunctionTACKYASTNode;
  class FunctionASTNode final: public ASTNode
  {
    std::string const                 identifier{};
    std::shared_ptr<StatementASTNode> statement{};

    public:
    FunctionASTNode(
      std::string_view const            identifier,
      std::shared_ptr<StatementASTNode> statement
    )
      : identifier{ identifier }
      , statement{ statement }
    {}

    [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept
    {
      return identifier;
    }

    [[nodiscard]] std::shared_ptr<StatementASTNode>
    getStatement() const noexcept
    {
      return statement;
    }

    [[nodiscard]] std::shared_ptr<FunctionTACKYASTNode> emitTACKY() const;

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << "int " << getIdentifier() << "(void) {\n";
      getStatement()->prettyPrintHelper(out, indent_level + 2);
      out << "}\n";
    }

    virtual ~FunctionASTNode() final override = default;
  };

  class ProgramTACKYASTNode;
  class ProgramASTNode final: public ASTNode
  {
    std::shared_ptr<FunctionASTNode> function{};

    public:
    explicit ProgramASTNode(std::shared_ptr<FunctionASTNode> function)
      : function{ function }
    {}

    [[nodiscard]] std::shared_ptr<FunctionASTNode> getFunction() const
    {
      return function;
    }

    [[nodiscard]] std::shared_ptr<ProgramTACKYASTNode> emitTACKY() const;

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      getFunction()->prettyPrintHelper(out, indent_level);
    }

    virtual ~ProgramASTNode() final override = default;
  };
} // namespace SC2

#endif
