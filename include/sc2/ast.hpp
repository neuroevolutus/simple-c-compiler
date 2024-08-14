#ifndef SC2_AST_HPP_INCLUDED
#define SC2_AST_HPP_INCLUDED

#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/utility.hpp>
#include <string_view>

#include <cstddef>
#include <memory>
#include <ostream>
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

  class InstructionTACKYASTNode;
  class ValueTACKYASTNode;
  struct ExpressionASTNode: public ASTNode
  {
    [[nodiscard]] virtual std::pair<
      std::shared_ptr<ValueTACKYASTNode>,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>>>
      emitTACKY(std::string_view, std::vector<std::shared_ptr<InstructionTACKYASTNode>>)
        const
      = 0;

    virtual ~ExpressionASTNode() override = default;
  };

  class UnaryOperatorTACKYASTNode;
  class UnaryOperatorASTNode: public ASTNode
  {
    protected:
    virtual void printUnaryOperator(std::ostream &out) = 0;

    public:
    virtual std::shared_ptr<UnaryOperatorTACKYASTNode> emitTACKY() const = 0;

    constexpr virtual void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      printUnaryOperator(out);
    }

    virtual ~UnaryOperatorASTNode() override = default;
  };

  class ComplementASTNode final: public UnaryOperatorASTNode
  {
    protected:
    virtual constexpr void printUnaryOperator(std::ostream &out) final override
    {
      out << "~";
    }

    public:
    virtual std::shared_ptr<UnaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~ComplementASTNode() final override = default;
  };

  struct NegateASTNode final: public UnaryOperatorASTNode
  {
    protected:
    virtual constexpr void printUnaryOperator(std::ostream &out) final override
    {
      out << "-";
    }

    public:
    virtual std::shared_ptr<UnaryOperatorTACKYASTNode>
    emitTACKY() const final override;

    virtual ~NegateASTNode() final override = default;
  };

  class UnaryExpressionASTNode final: public ExpressionASTNode
  {
    std::shared_ptr<UnaryOperatorASTNode> const unary_operator{};
    std::shared_ptr<ExpressionASTNode>          expression{};

    public:
    UnaryExpressionASTNode(
      std::shared_ptr<UnaryOperatorASTNode> unary_operator,
      std::shared_ptr<ExpressionASTNode>    expression
    )
      : unary_operator{ unary_operator }
      , expression{ expression }
    {}

    [[nodiscard]] std::shared_ptr<UnaryOperatorASTNode>
    getUnaryOperator() const noexcept
    {
      return unary_operator;
    }

    [[nodiscard]] std::shared_ptr<ExpressionASTNode>
    getExpression() const noexcept
    {
      return expression;
    }

    [[nodiscard]] std::pair<
      std::shared_ptr<ValueTACKYASTNode>,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>>>
      emitTACKY(std::string_view, std::vector<std::shared_ptr<InstructionTACKYASTNode>>)
        const final override;

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      getUnaryOperator()->prettyPrintHelper(out, indent_level);
      out << '(';
      getExpression()->prettyPrintHelper(out, indent_level);
      out << ')';
    }

    virtual ~UnaryExpressionASTNode() final override = default;
  };

  class LiteralConstantASTNode: public ExpressionASTNode
  {
    int const value{};

    public:
    explicit constexpr LiteralConstantASTNode(int value): value{ value } {}

    [[nodiscard]] constexpr int getValue() const noexcept { return value; }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << getValue();
    }

    [[nodiscard]] virtual std::pair<
      std::shared_ptr<ValueTACKYASTNode>,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>>>
      emitTACKY(std::string_view, std::vector<std::shared_ptr<InstructionTACKYASTNode>>)
        const final override;

    virtual ~LiteralConstantASTNode() final override = default;
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

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
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

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
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

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      getFunction()->prettyPrintHelper(out, indent_level);
    }

    virtual ~ProgramASTNode() final override = default;
  };
} // namespace SC2

#endif
