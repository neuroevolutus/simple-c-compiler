#ifndef SC2_AST_HPP_INCLUDED
#define SC2_AST_HPP_INCLUDED

#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/utility.hpp>
#include <string_view>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <ostream>
#include <span>
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

  struct TypeASTNode: public ASTNode
  {
    [[nodiscard]] virtual std::string toString() const = 0;
    [[nodiscard]] bool                operator==(TypeASTNode const &) const;
    virtual ~TypeASTNode() override = default;
  };

  struct IntTypeASTNode: public TypeASTNode
  {
    [[nodiscard]] virtual std::string toString() const final override
    {
      return "int";
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << toString();
    }

    virtual ~IntTypeASTNode() final override = default;
  };

  struct VoidTypeASTNode: public TypeASTNode
  {
    [[nodiscard]] virtual std::string toString() const final override
    {
      return "void";
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << toString();
    }

    virtual ~VoidTypeASTNode() final override = default;
  };

  struct InstructionTACKYASTNode;
  struct ExpressionASTNodeEmitTACKYInput
  {
    std::string                                           identifier{};
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions{};
  };

  struct ExpressionASTNode;
  struct BinaryOperatorASTNodeEmitTACKYInput
  {
    std::string                                           identifier{};
    std::shared_ptr<ExpressionASTNode>                    left_operand{};
    std::shared_ptr<ExpressionASTNode>                    right_operand{};
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
    emitTACKY(ExpressionASTNodeEmitTACKYInput &&) const
      = 0;

    virtual ~ExpressionASTNode() override = default;
  };

  class LiteralConstantASTNode final: public ExpressionASTNode
  {
    int const value{};

    [[nodiscard]] constexpr int getValue() const noexcept { return value; }

    public:
    explicit constexpr LiteralConstantASTNode(int value): value{ value } {}

    [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
    emitTACKY(ExpressionASTNodeEmitTACKYInput &&) const final override;

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << getValue();
    }

    virtual ~LiteralConstantASTNode() final override = default;
  };

  class VariableASTNode final: public ExpressionASTNode
  {
    std::string const identifier{};

    [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept
    {
      return identifier;
    }

    public:
    explicit constexpr VariableASTNode(std::string_view identifier)
      : identifier{ identifier }
    {}

    [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
    emitTACKY(ExpressionASTNodeEmitTACKYInput &&) const final override;

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << getIdentifier();
    }

    virtual ~VariableASTNode() final override = default;
  };

  struct UnaryOperatorPrettyPrintHelperInput
  {
    std::ostream                      &out;
    std::size_t                        indent_level{};
    std::shared_ptr<ExpressionASTNode> expression{};
  };

  struct UnaryOperatorASTNodeEmitTACKYInput
  {
    // TODO: consider making into string_view
    std::string                                           identifier{};
    std::shared_ptr<ExpressionASTNode>                    expression{};
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions{};
  };

  class UnaryOperatorTACKYASTNode;
  struct UnaryOperatorASTNode: public ASTNode
  {
    virtual void
    unaryOperatorPrettyPrintHelper(UnaryOperatorPrettyPrintHelperInput &&)
      = 0;

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(UnaryOperatorASTNodeEmitTACKYInput &&) const
      = 0;

    virtual ~UnaryOperatorASTNode() override = default;
  };

  class PrefixUnaryOperatorASTNode: public UnaryOperatorASTNode
  {
    protected:
    virtual void
    unaryOperatorPrettyPrintHelper(UnaryOperatorPrettyPrintHelperInput &&input
    ) override
    {
      auto &&[out, indent_level, expression]{ std::move(input) };
      prettyPrintHelper(out, indent_level);
      out << '(';
      expression->prettyPrintHelper(out, indent_level);
      out << ')';
    }

    public:
    virtual ~PrefixUnaryOperatorASTNode() override = default;
  };

  struct ComplementASTNode final: public PrefixUnaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "~";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(UnaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~ComplementASTNode() final override = default;
  };

  struct NegateASTNode final: public PrefixUnaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "-";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(UnaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~NegateASTNode() final override = default;
  };

  class NotTACKYASTNode;
  struct NotASTNode final: public PrefixUnaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "!";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(UnaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~NotASTNode() final override = default;
  };

  class BinaryOperatorTACKYASTNode;
  class PrefixAddSubtractASTNode: public PrefixUnaryOperatorASTNode
  {
    protected:
    [[nodiscard]] virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitBinaryOperatorTACKYASTNode() const = 0;

    public:
    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(UnaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual void
    unaryOperatorPrettyPrintHelper(UnaryOperatorPrettyPrintHelperInput &&input
    ) final override
    {
      auto &&[out, indent_level, expression]{ std::move(input) };
      prettyPrintHelper(out, indent_level);
      expression->prettyPrintHelper(out, indent_level);
    }

    virtual ~PrefixAddSubtractASTNode() override = default;
  };

  class PrefixIncrementASTNode final: public PrefixAddSubtractASTNode
  {
    protected:
    [[nodiscard]] virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitBinaryOperatorTACKYASTNode() const final override;

    public:
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "++";
    }

    virtual ~PrefixIncrementASTNode() final override = default;
  };

  class PrefixDecrementASTNode final: public PrefixAddSubtractASTNode
  {
    protected:
    [[nodiscard]] virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitBinaryOperatorTACKYASTNode() const final override;

    public:
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "--";
    }

    virtual ~PrefixDecrementASTNode() final override = default;
  };

  struct PostfixUnaryOperatorASTNode: public UnaryOperatorASTNode
  {
    virtual void
    unaryOperatorPrettyPrintHelper(UnaryOperatorPrettyPrintHelperInput &&input
    ) override
    {
      auto &&[out, indent_level, expression]{ std::move(input) };
      expression->prettyPrintHelper(out, indent_level);
      prettyPrintHelper(out, indent_level);
    }

    virtual ~PostfixUnaryOperatorASTNode() override = default;
  };

  class PostfixAddSubtractASTNode: public PostfixUnaryOperatorASTNode
  {
    protected:
    [[nodiscard]] virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitBinaryOperatorTACKYASTNode() const = 0;

    public:
    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(UnaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~PostfixAddSubtractASTNode() override = default;
  };

  class PostfixIncrementASTNode final: public PostfixAddSubtractASTNode
  {
    protected:
    [[nodiscard]] virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitBinaryOperatorTACKYASTNode() const final override;

    public:
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "++";
    }

    virtual ~PostfixIncrementASTNode() final override = default;
  };

  class PostfixDecrementASTNode final: public PostfixAddSubtractASTNode
  {
    protected:
    [[nodiscard]] virtual std::shared_ptr<BinaryOperatorTACKYASTNode>
    emitBinaryOperatorTACKYASTNode() const final override;

    public:
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "--";
    }

    virtual ~PostfixDecrementASTNode() final override = default;
  };

  class UnaryExpressionASTNode final: public ExpressionASTNode
  {
    std::shared_ptr<UnaryOperatorASTNode> const unary_operator{};
    std::shared_ptr<ExpressionASTNode> const    expression{};

    [[nodiscard]] std::shared_ptr<UnaryOperatorASTNode> getUnaryOperator() const
    {
      return unary_operator;
    }

    [[nodiscard]] std::shared_ptr<ExpressionASTNode> getExpression() const
    {
      return expression;
    }

    public:
    UnaryExpressionASTNode(
      std::shared_ptr<UnaryOperatorASTNode> unary_operator,
      std::shared_ptr<ExpressionASTNode>    expression
    )
      : unary_operator{ unary_operator }
      , expression{ expression }
    {}

    [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
    emitTACKY(ExpressionASTNodeEmitTACKYInput &&input) const final override
    {
      auto const &[identifier, instructions]{ std::move(input) };
      return getUnaryOperator()->emitTACKY(
        { identifier, getExpression(), std::move(instructions) }
      );
    }

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      getUnaryOperator()->unaryOperatorPrettyPrintHelper(
        { out, indent_level, std::move(getExpression()) }
      );
    }

    virtual ~UnaryExpressionASTNode() final override = default;
  };

  struct BinaryOperatorASTNode: public ASTNode
  {
    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&input) const
      = 0;

    virtual ~BinaryOperatorASTNode() override = default;
  };

  struct AddASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '+';
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~AddASTNode() final override = default;
  };

  struct SubtractASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '-';
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~SubtractASTNode() final override = default;
  };

  struct MultiplyASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '*';
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~MultiplyASTNode() final override = default;
  };

  struct DivideASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '/';
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~DivideASTNode() final override = default;
  };

  struct ModuloASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '%';
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~ModuloASTNode() final override = default;
  };

  struct BitwiseAndASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '&';
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~BitwiseAndASTNode() final override = default;
  };

  struct BitwiseOrASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '|';
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~BitwiseOrASTNode() final override = default;
  };

  struct BitwiseXorASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '^';
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~BitwiseXorASTNode() final override = default;
  };

  struct LeftShiftASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "<<";
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~LeftShiftASTNode() final override = default;
  };

  struct RightShiftASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << ">>";
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~RightShiftASTNode() final override = default;
  };

  struct AndASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "&&";
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~AndASTNode() final override = default;
  };

  struct OrASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "||";
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~OrASTNode() final override = default;
  };

  struct EqualsASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "==";
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~EqualsASTNode() final override = default;
  };

  struct NotEqualsASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "!=";
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~NotEqualsASTNode() final override = default;
  };

  struct LessThanASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "<";
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~LessThanASTNode() final override = default;
  };

  struct GreaterThanASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << ">";
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~GreaterThanASTNode() final override = default;
  };

  struct LessThanOrEqualToASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "<=";
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~LessThanOrEqualToASTNode() final override = default;
  };

  struct GreaterThanOrEqualToASTNode final: public BinaryOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << ">=";
    }

    virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BinaryOperatorASTNodeEmitTACKYInput &&) const final override;

    virtual ~GreaterThanOrEqualToASTNode() final override = default;
  };

  class AssignmentASTNode;
  struct BasicAssignmentOperatorASTNodeEmitTACKYInput
  {
    std::string const                                     identifier{};
    std::shared_ptr<VariableASTNode>                      variable{};
    std::shared_ptr<ExpressionASTNode>                    expression{};
    std::vector<std::shared_ptr<InstructionTACKYASTNode>> instructions{};
  };

  struct BasicAssignmentOperatorASTNode: public ASTNode
  {
    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&) const
      = 0;

    virtual ~BasicAssignmentOperatorASTNode() override = default;
  };

  struct AssignmentOperatorASTNode final: public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '=';
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~AssignmentOperatorASTNode() final override = default;
  };

  struct AddAssignmentOperatorASTNode final
    : public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "+=";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~AddAssignmentOperatorASTNode() final override = default;
  };

  struct SubtractAssignmentOperatorASTNode final
    : public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "-=";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~SubtractAssignmentOperatorASTNode() final override = default;
  };

  struct MultiplyAssignmentOperatorASTNode final
    : public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "*=";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~MultiplyAssignmentOperatorASTNode() final override = default;
  };

  struct DivideAssignmentOperatorASTNode final
    : public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "/=";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~DivideAssignmentOperatorASTNode() final override = default;
  };

  struct ModuloAssignmentOperatorASTNode final
    : public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "%=";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~ModuloAssignmentOperatorASTNode() final override = default;
  };

  struct BitwiseAndAssignmentOperatorASTNode final
    : public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "&=";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~BitwiseAndAssignmentOperatorASTNode() final override = default;
  };

  struct BitwiseOrAssignmentOperatorASTNode final
    : public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "|=";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~BitwiseOrAssignmentOperatorASTNode() final override = default;
  };

  struct BitwiseXorAssignmentOperatorASTNode final
    : public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "^=";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~BitwiseXorAssignmentOperatorASTNode() final override = default;
  };

  struct LeftShiftAssignmentOperatorASTNode final
    : public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "<<=";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~LeftShiftAssignmentOperatorASTNode() final override = default;
  };

  struct RightShiftAssignmentOperatorASTNode final
    : public BasicAssignmentOperatorASTNode
  {
    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << ">>=";
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(BasicAssignmentOperatorASTNodeEmitTACKYInput &&)
      const final override;

    virtual ~RightShiftAssignmentOperatorASTNode() final override = default;
  };

  class AssignmentASTNode final: public ExpressionASTNode
  {
    std::shared_ptr<BasicAssignmentOperatorASTNode> basic_assignment_operator{};
    std::shared_ptr<VariableASTNode> const          variable{};
    std::shared_ptr<ExpressionASTNode> const        expression{};

    [[nodiscard]] std::shared_ptr<BasicAssignmentOperatorASTNode>
    getBasicAssignmentOperator() const
    {
      return basic_assignment_operator;
    }

    [[nodiscard]] std::shared_ptr<VariableASTNode> getVariable() const
    {
      return variable;
    }

    [[nodiscard]] std::shared_ptr<ExpressionASTNode> getExpression() const
    {
      return expression;
    }

    public:
    AssignmentASTNode(
      std::shared_ptr<BasicAssignmentOperatorASTNode> basic_assignment_operator,
      std::shared_ptr<VariableASTNode>                variable,
      std::shared_ptr<ExpressionASTNode>              expression
    )
      : basic_assignment_operator{ basic_assignment_operator }
      , variable{ variable }
      , expression{ expression }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << '(';
      getVariable()->prettyPrintHelper(out, 0);
      out << ' ';
      getBasicAssignmentOperator()->prettyPrintHelper(out, 0);
      out << ' ';
      getExpression()->prettyPrintHelper(out, 0);
      out << ')';
    }

    [[nodiscard]] virtual ExpressionASTNodeEmitTACKYOutput
    emitTACKY(ExpressionASTNodeEmitTACKYInput &&input) const
    {
      auto const &[identifier, instructions]{ std::move(input) };
      return getBasicAssignmentOperator()->emitTACKY(
        BasicAssignmentOperatorASTNodeEmitTACKYInput{ identifier,
                                                      variable,
                                                      expression,
                                                      std::move(instructions) }
      );
    }

    virtual ~AssignmentASTNode() final override = default;
  };

  class BinaryExpressionASTNode final: public ExpressionASTNode
  {
    std::shared_ptr<BinaryOperatorASTNode> const binary_operator{};
    std::shared_ptr<ExpressionASTNode> const     left_operand{};
    std::shared_ptr<ExpressionASTNode> const     right_operand{};

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

    [[nodiscard]] ExpressionASTNodeEmitTACKYOutput
    emitTACKY(ExpressionASTNodeEmitTACKYInput &&input) const final override
    {
      auto const &[identifier, instructions]{ std::move(input) };
      return getBinaryOperator()->emitTACKY({ std::move(identifier),
                                              std::move(getLeftOperand()),
                                              std::move(getRightOperand()),
                                              std::move(instructions) });
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      out << '(';
      getLeftOperand()->prettyPrintHelper(out, indent_level);
      out << ' ';
      getBinaryOperator()->prettyPrintHelper(out, indent_level);
      out << ' ';
      getRightOperand()->prettyPrintHelper(out, indent_level);
      out << ')';
    }

    virtual ~BinaryExpressionASTNode() final override = default;
  };

  struct BlockItemASTNode: public ASTNode
  {
    BlockItemASTNode() = default;
    [[nodiscard]] virtual std::vector<std::shared_ptr<InstructionTACKYASTNode>>
    emitTACKY(std::string_view) const
    {
      return std::vector<std::shared_ptr<InstructionTACKYASTNode>>{};
    }
    virtual ~BlockItemASTNode() override = default;
  };

  struct StatementASTNode: public BlockItemASTNode
  {
    StatementASTNode() = default;
    [[nodiscard]] virtual std::vector<std::shared_ptr<InstructionTACKYASTNode>>
      emitTACKY(std::string_view) const  = 0;
    virtual ~StatementASTNode() override = default;
  };

  class ReturnStatementASTNode final: public StatementASTNode
  {
    std::shared_ptr<ExpressionASTNode> expression{};

    [[nodiscard]] std::shared_ptr<ExpressionASTNode>
    getExpression() const noexcept
    {
      return expression;
    }

    public:
    explicit ReturnStatementASTNode(
      std::shared_ptr<ExpressionASTNode> expression
    )
      : expression{ expression }
    {}

    [[nodiscard]] virtual std::vector<std::shared_ptr<InstructionTACKYASTNode>>
      emitTACKY(std::string_view) const final override;

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << "return ";
      getExpression()->prettyPrintHelper(out, indent_level);
      out << ";\n";
    }

    virtual ~ReturnStatementASTNode() final override = default;
  };

  class ExpressionStatementASTNode final: public StatementASTNode
  {
    std::shared_ptr<ExpressionASTNode> expression{};

    [[nodiscard]] std::shared_ptr<ExpressionASTNode>
    getExpression() const noexcept
    {
      return expression;
    }

    public:
    explicit ExpressionStatementASTNode(
      std::shared_ptr<ExpressionASTNode> expression
    )
      : expression{ expression }
    {}

    [[nodiscard]] virtual std::vector<std::shared_ptr<InstructionTACKYASTNode>>
      emitTACKY(std::string_view) const final override;

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      getExpression()->prettyPrintHelper(out, indent_level);
      out << ";\n";
    }

    virtual ~ExpressionStatementASTNode() final override = default;
  };

  class NullStatementASTNode final: public StatementASTNode
  {
    public:
    [[nodiscard]] virtual std::vector<std::shared_ptr<InstructionTACKYASTNode>>
    emitTACKY(std::string_view) const final override
    {
      return std::vector<std::shared_ptr<InstructionTACKYASTNode>>{};
    }

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << ";\n";
    }

    virtual ~NullStatementASTNode() final override = default;
  };

  class DeclarationASTNode final: public BlockItemASTNode
  {
    std::shared_ptr<TypeASTNode>       type{};
    std::string const                  identifier{};
    std::shared_ptr<ExpressionASTNode> initializer{};

    [[nodiscard]] constexpr std::string_view getIdentifier() const
    {
      return identifier;
    }

    [[nodiscard]] std::shared_ptr<ExpressionASTNode> getInitializer() const
    {
      return initializer;
    }

    public:
    DeclarationASTNode(
      std::shared_ptr<TypeASTNode>       type,
      std::string_view                   identifier,
      std::shared_ptr<ExpressionASTNode> initializer
    )
      : type{ type }
      , identifier{ identifier }
      , initializer{ initializer }
    {}

    [[nodiscard]] virtual std::vector<std::shared_ptr<InstructionTACKYASTNode>>
      emitTACKY(std::string_view) const override;

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      type->prettyPrintHelper(out, 0);
      out << ' ' << identifier;
      if (initializer) {
        out << " = ";
        initializer->prettyPrintHelper(out, 0);
      }
      out << ";\n";
    }

    virtual ~DeclarationASTNode() final override = default;
  };

  class FunctionTACKYASTNode;
  class FunctionASTNode final: public ASTNode
  {
    std::string const                              identifier{};
    std::vector<std::shared_ptr<BlockItemASTNode>> block_items{};

    [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept
    {
      return identifier;
    }

    [[nodiscard]] constexpr std::span<std::shared_ptr<BlockItemASTNode>>
    getBlockItems()
    {
      return block_items;
    }

    public:
    FunctionASTNode(
      std::string_view const                         identifier,
      std::vector<std::shared_ptr<BlockItemASTNode>> block_items
    )
      : identifier{ identifier }
      , block_items{ std::move(block_items) }
    {}

    [[nodiscard]] std::shared_ptr<FunctionTACKYASTNode> emitTACKY();

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << "int " << getIdentifier() << "(void) {\n";
      std::ranges::for_each(
        getBlockItems(),
        [&out, &indent_level](std::shared_ptr<BlockItemASTNode> block_item) {
        block_item->prettyPrintHelper(out, indent_level + 2);
      }
      );
      out << "}\n";
    }

    virtual ~FunctionASTNode() final override = default;
  };

  class ProgramTACKYASTNode;
  class ProgramASTNode final: public ASTNode
  {
    std::shared_ptr<FunctionASTNode> function{};

    [[nodiscard]] std::shared_ptr<FunctionASTNode> getFunction() const
    {
      return function;
    }

    public:
    explicit ProgramASTNode(std::shared_ptr<FunctionASTNode> function)
      : function{ function }
    {}

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
