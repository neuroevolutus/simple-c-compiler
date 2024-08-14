#ifndef SC2_AST_HPP_INCLUDED
#define SC2_AST_HPP_INCLUDED

#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/tokens.hpp>
#include <sc2/utility.hpp>

#include <memory>
#include <sstream>

namespace SC2 {
  struct AbstractSyntaxTreeNode
    : public std::enable_shared_from_this<AbstractSyntaxTreeNode>
    , public PrettyPrinterMixin
  {
    virtual ~AbstractSyntaxTreeNode() = default;
  };

  class Expression: public AbstractSyntaxTreeNode
  {
    LiteralConstant const value{};

    public:
    explicit constexpr Expression(LiteralConstant const value): value{ value }
    {}

    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) override
    {
      out << value.getValue();
    }

    virtual ~Expression() override = default;
  };

  class Statement: public AbstractSyntaxTreeNode
  {
    std::shared_ptr<Expression> expression{};

    public:
    explicit constexpr Statement(std::shared_ptr<Expression> expression)
      : expression{ expression }
    {}

    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) override
    {
      Utility::indent(out, indent_level);
      out << "return ";
      expression->prettyPrintHelper(out, indent_level);
      out << ";\n";
    }

    virtual ~Statement() override = default;
  };

  class Function: public AbstractSyntaxTreeNode
  {
    Identifier const           function_name{};
    std::shared_ptr<Statement> statement{};

    public:
    explicit constexpr Function(
      Identifier const           function_name,
      std::shared_ptr<Statement> statement
    )
      : function_name{ function_name }
      , statement{ statement }
    {}

    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) override
    {
      Utility::indent(out, indent_level);
      out << "int " << function_name.getName() << "(void) {\n";
      statement->prettyPrintHelper(out, indent_level + 2);
      out << "}\n";
    }

    virtual ~Function() override = default;
  };

  class Program: public AbstractSyntaxTreeNode
  {
    std::shared_ptr<Function> function_definition{};

    public:
    explicit constexpr Program(std::shared_ptr<Function> function_definition)
      : function_definition{ function_definition }
    {}

    virtual constexpr void prettyPrintHelper(
      std::ostringstream &out,
      std::size_t         indent_level
    ) override
    {
      function_definition->prettyPrintHelper(out, indent_level);
    }

    virtual ~Program() override = default;
  };
} // namespace SC2

#endif
