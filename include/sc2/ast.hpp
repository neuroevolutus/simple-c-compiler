#ifndef SC2_AST_HPP_INCLUDED
#define SC2_AST_HPP_INCLUDED

#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/tokens.hpp>
#include <sc2/utility.hpp>

#include <cstddef>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

namespace SC2 {
  struct ASTNode
    : public std::enable_shared_from_this<ASTNode>
    , public PrettyPrinterMixin
  {
    virtual ~ASTNode() = default;
  };

  enum class UnaryOperator
  {
    COMPLEMENT,
    NEGATE
  };

  class InstructionTACKYASTNode;
  class ValueTACKYASTNode;
  struct ExpressionASTNode: public ASTNode
  {
    virtual std::pair<
      std::shared_ptr<ValueTACKYASTNode>,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>>>
      emitTACKY(Identifier, std::vector<std::shared_ptr<InstructionTACKYASTNode>>)
        const
      = 0;

    virtual ~ExpressionASTNode() override = default;
  };

  class UnaryExpressionASTNode: public ExpressionASTNode
  {
    UnaryOperator const                unary_operator{};
    std::shared_ptr<ExpressionASTNode> expression{};

    public:
    constexpr UnaryExpressionASTNode(
      UnaryOperator                      unary_operator,
      std::shared_ptr<ExpressionASTNode> expression
    )
      : unary_operator{ unary_operator }
      , expression{ expression }
    {}

    constexpr UnaryOperator getUnaryOperator() const { return unary_operator; }

    constexpr std::shared_ptr<ExpressionASTNode> getExpression() const
    {
      return expression;
    }

    std::pair<
      std::shared_ptr<ValueTACKYASTNode>,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>>>
      emitTACKY(Identifier, std::vector<std::shared_ptr<InstructionTACKYASTNode>>)
        const override;

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      switch (unary_operator) {
        case UnaryOperator::COMPLEMENT: out << '~'; break;
        case UnaryOperator::NEGATE: out << '-'; break;
        default: std::unreachable();
      }
      out << '(';
      expression->prettyPrintHelper(out, indent_level);
      out << ')';
    }

    virtual ~UnaryExpressionASTNode() override = default;
  };

  class LiteralConstantASTNode: public ExpressionASTNode
  {
    LiteralConstant const value{};

    public:
    explicit constexpr LiteralConstantASTNode(LiteralConstant const value)
      : value{ value }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      out << value.getValue();
    }

    LiteralConstant getLiteralConstant() const { return value; }

    std::pair<
      std::shared_ptr<ValueTACKYASTNode>,
      std::vector<std::shared_ptr<InstructionTACKYASTNode>>>
      emitTACKY(Identifier, std::vector<std::shared_ptr<InstructionTACKYASTNode>>)
        const override;

    virtual ~LiteralConstantASTNode() override = default;
  };

  class StatementASTNode: public ASTNode
  {
    std::shared_ptr<ExpressionASTNode> expression{};

    public:
    explicit constexpr StatementASTNode(
      std::shared_ptr<ExpressionASTNode> expression
    )
      : expression{ expression }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      Utility::indent(out, indent_level);
      out << "return ";
      expression->prettyPrintHelper(out, indent_level);
      out << ";\n";
    }

    std::shared_ptr<ExpressionASTNode> getExpression() const
    {
      return expression;
    }

    std::vector<std::shared_ptr<InstructionTACKYASTNode>> emitTACKY(Identifier
    ) const;

    virtual ~StatementASTNode() override = default;
  };

  class FunctionTACKYASTNode;
  class FunctionASTNode: public ASTNode
  {
    Identifier const                  function_name{};
    std::shared_ptr<StatementASTNode> statement{};

    public:
    explicit constexpr FunctionASTNode(
      Identifier const                  function_name,
      std::shared_ptr<StatementASTNode> statement
    )
      : function_name{ function_name }
      , statement{ statement }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      Utility::indent(out, indent_level);
      out << "int " << function_name.getName() << "(void) {\n";
      statement->prettyPrintHelper(out, indent_level + 2);
      out << "}\n";
    }

    constexpr Identifier getIdentifier() const { return function_name; }

    std::shared_ptr<FunctionTACKYASTNode> emitTACKY() const;

    constexpr std::shared_ptr<StatementASTNode> getStatement() const
    {
      return statement;
    }

    virtual ~FunctionASTNode() override = default;
  };

  class ProgramTACKYASTNode;
  class ProgramASTNode: public ASTNode
  {
    std::shared_ptr<FunctionASTNode> function{};

    public:
    explicit constexpr ProgramASTNode(std::shared_ptr<FunctionASTNode> function)
      : function{ function }
    {}

    constexpr std::shared_ptr<FunctionASTNode> getFunction() const
    {
      return function;
    }

    std::shared_ptr<ProgramTACKYASTNode> emitTACKY() const;

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      function->prettyPrintHelper(out, indent_level);
    }

    virtual ~ProgramASTNode() override = default;
  };
} // namespace SC2

#endif
