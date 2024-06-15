#ifndef SC2_AST_HPP_INCLUDED
#define SC2_AST_HPP_INCLUDED

#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/tokens.hpp>
#include <sc2/utility.hpp>

#include <cstddef>
#include <memory>
#include <ostream>

namespace SC2 {
  struct ASTNode
    : public std::enable_shared_from_this<ASTNode>
    , public PrettyPrinterMixin
  {
    virtual ~ASTNode() = default;
  };

  class ExpressionASTNode: public ASTNode
  {
    LiteralConstant const value{};

    public:
    explicit constexpr ExpressionASTNode(LiteralConstant const value)
      : value{ value }
    {}

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      out << value.getValue();
    }

    LiteralConstant getLiteralConstant() const { return value; }

    virtual ~ExpressionASTNode() override = default;
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
    virtual ~StatementASTNode() override = default;
  };

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

    constexpr Identifier getName() const { return function_name; }

    constexpr std::shared_ptr<StatementASTNode> getStatement() const
    {
      return statement;
    }

    virtual ~FunctionASTNode() override = default;
  };

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

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t indent_level) override
    {
      function->prettyPrintHelper(out, indent_level);
    }

    virtual ~ProgramASTNode() override = default;
  };
} // namespace SC2

#endif
