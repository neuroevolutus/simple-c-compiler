#ifndef SC2_ASSEMBLY_AST_HPP_INCLUDED
#define SC2_ASSEMBLY_AST_HPP_INCLUDED

#include <sc2/ast.hpp>
#include <sc2/compiler_error.hpp>
#include <sc2/pretty_printer_mixin.hpp>
#include <sc2/utility.hpp>
#include <string_view>
#include <unordered_map>

#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <numeric>
#include <ostream>
#include <span>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace SC2 {
  class CodeEmissionError: public CompilerError
  {
    std::string const message{};

    public:
    CodeEmissionError(std::string_view assembly_ast_node)
      : message{ std::format(
          "Cannot emit code for assembly AST node: {}",
          assembly_ast_node
        ) }
    {}

    constexpr virtual char const *what() const noexcept final override
    {
      return message.c_str();
    }
  };

  struct ReplacePseudoRegistersInput
  {
    std::intptr_t                                  last_offset{};
    std::unordered_map<std::string, std::intptr_t> identifier_to_offset_map{};
  };

  template <typename OutputNodeType> struct ReplacePseudoRegistersResult
  {
    std::intptr_t                                  last_offset{};
    std::unordered_map<std::string, std::intptr_t> identifier_to_offset_map{};
    std::shared_ptr<OutputNodeType>                new_assembly_ast_node{};
  };

  struct AssemblyASTNode
    : public std::enable_shared_from_this<AssemblyASTNode>
    , public PrettyPrinterMixin
  {
    virtual void emitCode(std::ostream &out) = 0;
    virtual ~AssemblyASTNode()               = default;
  };

  struct OperandAssemblyASTNode: public AssemblyASTNode
  {
    [[nodiscard]] virtual ReplacePseudoRegistersResult<OperandAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input)
    {
      auto [offset, map]{ std::move(input) };
      return ReplacePseudoRegistersResult<OperandAssemblyASTNode>{
        offset,
        std::move(map),
        std::dynamic_pointer_cast<OperandAssemblyASTNode>(shared_from_this())
      };
    }

    virtual ~OperandAssemblyASTNode() override = default;
  };

  class ImmediateValueAssemblyASTNode final: public OperandAssemblyASTNode
  {
    int const value{};

    [[nodiscard]] constexpr int getValue() const noexcept { return value; }

    public:
    constexpr ImmediateValueAssemblyASTNode(int value): value{ value } {}

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "$" << getValue();
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "ImmediateValue: " << getValue();
    }

    virtual ~ImmediateValueAssemblyASTNode() final override = default;
  };

  struct ByteRegisterAssemblyASTNode;
  struct LongWordRegisterAssemblyASTNode;
  class RegisterAssemblyASTNode: public OperandAssemblyASTNode
  {
    protected:
    constexpr void printRegisterPrefix(std::ostream &out)
    {
      out << "Register: ";
    }

    virtual void emitRegister(std::ostream &out) = 0;

    virtual void printRegister(std::ostream &out) = 0;

    public:
    [[nodiscard]] virtual std::shared_ptr<ByteRegisterAssemblyASTNode>
    toByteRegister() = 0;

    [[nodiscard]] virtual std::shared_ptr<LongWordRegisterAssemblyASTNode>
    toLongWordRegister() = 0;

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "%";
      emitRegister(out);
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) override
    {
      printRegisterPrefix(out);
      printRegister(out);
    }

    virtual ~RegisterAssemblyASTNode() override = default;
  };

  struct LongWordRegisterAssemblyASTNode: public RegisterAssemblyASTNode
  {
    [[nodiscard]] virtual std::shared_ptr<LongWordRegisterAssemblyASTNode>
    toLongWordRegister() final override
    {
      return std::dynamic_pointer_cast<LongWordRegisterAssemblyASTNode>(
        shared_from_this()
      );
    }

    virtual ~LongWordRegisterAssemblyASTNode() override = default;
  };

  struct ByteRegisterAssemblyASTNode: public RegisterAssemblyASTNode
  {
    [[nodiscard]] virtual std::shared_ptr<ByteRegisterAssemblyASTNode>
    toByteRegister() final override
    {
      return std::dynamic_pointer_cast<ByteRegisterAssemblyASTNode>(
        shared_from_this()
      );
    }

    virtual ~ByteRegisterAssemblyASTNode() override = default;
  };

  class EAXRegisterAssemblyASTNode final: public LongWordRegisterAssemblyASTNode
  {
    protected:
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "eax";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%eax";
    }

    public:
    [[nodiscard]] std::shared_ptr<ByteRegisterAssemblyASTNode>
    toByteRegister() final override;

    virtual ~EAXRegisterAssemblyASTNode() final override = default;
  };

  class ALRegisterAssemblyASTNode final: public ByteRegisterAssemblyASTNode
  {
    protected:
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "al";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%al";
    }

    public:
    [[nodiscard]] std::shared_ptr<LongWordRegisterAssemblyASTNode>
    toLongWordRegister() final override;

    virtual ~ALRegisterAssemblyASTNode() final override = default;
  };

  class EDXRegisterAssemblyASTNode final: public LongWordRegisterAssemblyASTNode
  {
    protected:
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "edx";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%edx";
    }

    public:
    [[nodiscard]] std::shared_ptr<ByteRegisterAssemblyASTNode>
    toByteRegister() final override;

    virtual ~EDXRegisterAssemblyASTNode() final override = default;
  };

  class DLRegisterAssemblyASTNode final: public ByteRegisterAssemblyASTNode
  {
    protected:
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "dl";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%dl";
    }

    public:
    [[nodiscard]] std::shared_ptr<LongWordRegisterAssemblyASTNode>
    toLongWordRegister() final override;

    virtual ~DLRegisterAssemblyASTNode() final override = default;
  };

  class R10DRegisterAssemblyASTNode final
    : public LongWordRegisterAssemblyASTNode
  {
    protected:
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "r10d";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%r10d";
    }

    public:
    [[nodiscard]] std::shared_ptr<ByteRegisterAssemblyASTNode>
    toByteRegister() final override;

    virtual ~R10DRegisterAssemblyASTNode() final override = default;
  };

  class R10BRegisterAssemblyASTNode final: public ByteRegisterAssemblyASTNode
  {
    protected:
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "r10b";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%r10b";
    }

    public:
    [[nodiscard]] std::shared_ptr<LongWordRegisterAssemblyASTNode>
    toLongWordRegister() final override;

    virtual ~R10BRegisterAssemblyASTNode() final override = default;
  };

  class R11DRegisterAssemblyASTNode final
    : public LongWordRegisterAssemblyASTNode
  {
    protected:
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "r11d";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%r11d";
    }

    public:
    [[nodiscard]] std::shared_ptr<ByteRegisterAssemblyASTNode>
    toByteRegister() final override;

    virtual ~R11DRegisterAssemblyASTNode() final override = default;
  };

  class R11BRegisterAssemblyASTNode final: public ByteRegisterAssemblyASTNode
  {
    protected:
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "r11b";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%r11b";
    }

    public:
    [[nodiscard]] std::shared_ptr<LongWordRegisterAssemblyASTNode>
    toLongWordRegister() final override;

    virtual ~R11BRegisterAssemblyASTNode() final override = default;
  };

  class ECXRegisterAssemblyASTNode final: public LongWordRegisterAssemblyASTNode
  {
    protected:
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "ecx";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%ecx";
    }

    public:
    [[nodiscard]] std::shared_ptr<ByteRegisterAssemblyASTNode>
    toByteRegister() final override;

    virtual ~ECXRegisterAssemblyASTNode() final override = default;
  };

  class CLRegisterAssemblyASTNode final: public ByteRegisterAssemblyASTNode
  {
    protected:
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "cl";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%cl";
    }

    public:
    [[nodiscard]] std::shared_ptr<LongWordRegisterAssemblyASTNode>
    toLongWordRegister() final override;

    virtual ~CLRegisterAssemblyASTNode() final override = default;
  };

  class StackOffsetAssemblyASTNode final: public OperandAssemblyASTNode
  {
    std::intptr_t const offset{};

    [[nodiscard]] constexpr std::intptr_t getOffset() const noexcept
    {
      return offset;
    }

    public:
    explicit constexpr StackOffsetAssemblyASTNode(std::intptr_t offset)
      : offset{ offset }
    {}

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << getOffset() << "(%rbp)";
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "StackOffset: " << getOffset();
    }

    virtual ~StackOffsetAssemblyASTNode() final override = default;
  };

  class PseudoRegisterAssemblyASTNode final: public OperandAssemblyASTNode
  {
    std::string const identifier{};

    [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept
    {
      return identifier;
    }

    [[nodiscard]] constexpr std::string toString() const
    {
      return std::format("PseudoRegister: {}", getIdentifier());
    }

    public:
    explicit constexpr PseudoRegisterAssemblyASTNode(std::string_view identifier
    )
      : identifier{ identifier }
    {}

    [[nodiscard]] virtual ReplacePseudoRegistersResult<OperandAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input) final override
    {
      auto [offset, map]{ std::move(input) };
      if (!map.contains(std::string{ getIdentifier() })) {
        offset                              -= 4;
        map[std::string{ getIdentifier() }]  = offset;
      }
      auto const &stack_offset{ map[std::string{ getIdentifier() }] };
      return { offset,
               std::move(map),
               std::make_shared<StackOffsetAssemblyASTNode>(stack_offset) };
    }

    virtual void emitCode(std::ostream &) final override
    {
      throw CodeEmissionError(toString());
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << toString();
    }

    virtual ~PseudoRegisterAssemblyASTNode() final override = default;
  };

  struct InstructionAssemblyASTNode: public AssemblyASTNode
  {
    [[nodiscard]] virtual ReplacePseudoRegistersResult<
      InstructionAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input)
    {
      auto [offset, map]{ std::move(input) };
      return ReplacePseudoRegistersResult<InstructionAssemblyASTNode>{
        offset,
        std::move(map),
        std::dynamic_pointer_cast<InstructionAssemblyASTNode>(shared_from_this()
        )
      };
    }

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    fixUp()
    {
      return { std::dynamic_pointer_cast<InstructionAssemblyASTNode>(
        shared_from_this()
      ) };
    }

    virtual ~InstructionAssemblyASTNode() override = default;
  };

  class MovlAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::shared_ptr<OperandAssemblyASTNode> const source{};
    std::shared_ptr<OperandAssemblyASTNode> const destination{};

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode> getSource() const
    {
      return source;
    }

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode> getDestination() const
    {
      return destination;
    }

    public:
    MovlAssemblyASTNode(
      std::shared_ptr<OperandAssemblyASTNode> source,
      std::shared_ptr<OperandAssemblyASTNode> destination
    )
      : source{ source }
      , destination{ destination }
    {}

    [[nodiscard]] virtual ReplacePseudoRegistersResult<
      InstructionAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input) final override
    {
      auto [offset_zero, map_zero, new_source]{
        getSource()->replacePseudoRegisters(std::move(input))
      };
      auto [offset_one, map_one, new_destination]{
        getDestination()->replacePseudoRegisters({ offset_zero,
                                                   std::move(map_zero) })
      };
      return { offset_one,
               std::move(map_one),
               std::make_shared<MovlAssemblyASTNode>(
                 std::move(new_source),
                 std::move(new_destination)
               ) };
    }

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    fixUp() override
    {
      auto const &stack_source{
        std::dynamic_pointer_cast<StackOffsetAssemblyASTNode>(getSource())
      };
      auto const &stack_destination{
        std::dynamic_pointer_cast<StackOffsetAssemblyASTNode>(getDestination())
      };
      if (stack_source && stack_destination) {
        return {
          std::make_shared<MovlAssemblyASTNode>(
            getSource(),
            std::make_shared<R10DRegisterAssemblyASTNode>()
          ),
          std::make_shared<MovlAssemblyASTNode>(
            std::make_shared<R10DRegisterAssemblyASTNode>(),
            getDestination()
          ),
        };
      } else
        return { std::dynamic_pointer_cast<InstructionAssemblyASTNode>(
          shared_from_this()
        ) };
    }

    virtual void emitCode(std::ostream &out) final override
    {
      Utility::indent(out, 2);
      out << "movl ";
      getSource()->emitCode(out);
      out << ", ";
      getDestination()->emitCode(out);
      out << '\n';
    }

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << "Movl (";
      getSource()->prettyPrintHelper(out, 0);
      out << "), (";
      getDestination()->prettyPrintHelper(out, 0);
      out << ")\n";
    }

    virtual ~MovlAssemblyASTNode() final override = default;
  };

  class MovbAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::shared_ptr<OperandAssemblyASTNode> const source{};
    std::shared_ptr<OperandAssemblyASTNode> const destination{};

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode> getSource() const
    {
      return source;
    }

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode> getDestination() const
    {
      return destination;
    }

    public:
    MovbAssemblyASTNode(
      std::shared_ptr<OperandAssemblyASTNode> source,
      std::shared_ptr<OperandAssemblyASTNode> destination
    )
      : source{ source }
      , destination{ destination }
    {}

    [[nodiscard]] virtual ReplacePseudoRegistersResult<
      InstructionAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input) final override
    {
      auto [offset_zero, map_zero, new_source]{
        getSource()->replacePseudoRegisters(std::move(input))
      };
      auto [offset_one, map_one, new_destination]{
        getDestination()->replacePseudoRegisters({ offset_zero,
                                                   std::move(map_zero) })
      };
      return { offset_one,
               std::move(map_one),
               std::make_shared<MovbAssemblyASTNode>(
                 std::move(new_source),
                 std::move(new_destination)
               ) };
    }

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    fixUp() override
    {
      auto const &stack_source{
        std::dynamic_pointer_cast<StackOffsetAssemblyASTNode>(getSource())
      };
      auto const &stack_destination{
        std::dynamic_pointer_cast<StackOffsetAssemblyASTNode>(getDestination())
      };
      if (stack_source && stack_destination) {
        return {
          std::make_shared<MovbAssemblyASTNode>(
            getSource(),
            std::make_shared<R10DRegisterAssemblyASTNode>()
          ),
          std::make_shared<MovbAssemblyASTNode>(
            std::make_shared<R10DRegisterAssemblyASTNode>(),
            getDestination()
          ),
        };
      } else
        return { std::dynamic_pointer_cast<InstructionAssemblyASTNode>(
          shared_from_this()
        ) };
    }

    virtual void emitCode(std::ostream &out) final override
    {
      Utility::indent(out, 2);
      out << "movb ";
      getSource()->emitCode(out);
      out << ", ";
      getDestination()->emitCode(out);
      out << '\n';
    }

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << "Movb (";
      getSource()->prettyPrintHelper(out, 0);
      out << "), (";
      getDestination()->prettyPrintHelper(out, 0);
      out << ")\n";
    }

    virtual ~MovbAssemblyASTNode() final override = default;
  };

  class UnaryOperatorAssemblyASTNode: public AssemblyASTNode
  {
    protected:
    virtual void printUnaryOperator(std::ostream &out) = 0;

    public:
    constexpr virtual void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      printUnaryOperator(out);
    }

    virtual ~UnaryOperatorAssemblyASTNode() override = default;
  };

  class ComplementAssemblyASTNode final: public UnaryOperatorAssemblyASTNode
  {
    protected:
    virtual constexpr void printUnaryOperator(std::ostream &out) final override
    {
      out << "Complement";
    }

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "notl";
    }

    public:
    virtual ~ComplementAssemblyASTNode() final override = default;
  };

  class NegateAssemblyASTNode final: public UnaryOperatorAssemblyASTNode
  {
    protected:
    virtual constexpr void printUnaryOperator(std::ostream &out) final override
    {
      out << "Negate";
    }

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "negl";
    }

    public:
    virtual ~NegateAssemblyASTNode() final override = default;
  };

  class UnaryAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::shared_ptr<UnaryOperatorAssemblyASTNode> const unary_operator{};
    std::shared_ptr<OperandAssemblyASTNode> const       operand{};

    [[nodiscard]] std::shared_ptr<UnaryOperatorAssemblyASTNode>
    getUnaryOperator() const
    {
      return unary_operator;
    }

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode> getOperand() const
    {
      return operand;
    }

    public:
    UnaryAssemblyASTNode(
      std::shared_ptr<UnaryOperatorAssemblyASTNode> unary_operator,
      std::shared_ptr<OperandAssemblyASTNode>       operand
    )
      : unary_operator{ unary_operator }
      , operand{ operand }
    {}

    [[nodiscard]] virtual ReplacePseudoRegistersResult<
      InstructionAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input) final override
    {
      auto [new_offset, new_map, new_operand]{
        getOperand()->replacePseudoRegisters(std::move(input))
      };
      return { new_offset,
               std::move(new_map),
               std::make_shared<UnaryAssemblyASTNode>(
                 getUnaryOperator(),
                 std::move(new_operand)
               ) };
    }

    virtual void emitCode(std::ostream &out) final override
    {
      getUnaryOperator()->emitCode(out);
      out << ' ';
      getOperand()->emitCode(out);
      out << '\n';
    }

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      Utility::indent(out, indent_level);
      out << "Unary (";
      getUnaryOperator()->prettyPrintHelper(out, 0);
      out << " (";
      getOperand()->prettyPrintHelper(out, 0);
      out << "))\n";
    }

    virtual ~UnaryAssemblyASTNode() final override = default;
  };

  struct BinaryAssemblyASTNodeFixUpInput
  {
    std::shared_ptr<OperandAssemblyASTNode> source{};
    std::shared_ptr<OperandAssemblyASTNode> destination{};
  };

  class BinaryOperatorAssemblyASTNode: public AssemblyASTNode
  {
    protected:
    virtual void printBinaryOperator(std::ostream &out) = 0;

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    fixUp(BinaryAssemblyASTNodeFixUpInput &&input);

    constexpr virtual void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      printBinaryOperator(out);
    }

    virtual ~BinaryOperatorAssemblyASTNode() override = default;
  };

  class AddAssemblyASTNode final: public BinaryOperatorAssemblyASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "Add";
    }

    public:
    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "addl";
    }

    virtual ~AddAssemblyASTNode() final override = default;
  };

  class SubtractAssemblyASTNode final: public BinaryOperatorAssemblyASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "Subtract";
    }

    public:
    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "subl";
    }

    virtual ~SubtractAssemblyASTNode() final override = default;
  };

  class MultiplyAssemblyASTNode final: public BinaryOperatorAssemblyASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "Multiply";
    }

    public:
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    fixUp(BinaryAssemblyASTNodeFixUpInput &&input) final override;

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "imull";
    }

    virtual ~MultiplyAssemblyASTNode() final override = default;
  };

  class BitwiseAndAssemblyASTNode final: public BinaryOperatorAssemblyASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "BitwiseAnd";
    }

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "andl";
    }

    public:
    virtual ~BitwiseAndAssemblyASTNode() final override = default;
  };

  class BitwiseOrAssemblyASTNode final: public BinaryOperatorAssemblyASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "BitwiseOr";
    }

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "orl";
    }

    public:
    virtual ~BitwiseOrAssemblyASTNode() final override = default;
  };

  class BitwiseXorAssemblyASTNode final: public BinaryOperatorAssemblyASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "BitwiseXor";
    }

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "xorl";
    }

    public:
    virtual ~BitwiseXorAssemblyASTNode() final override = default;
  };

  struct ShiftOperatorAssemblyASTNode: public BinaryOperatorAssemblyASTNode
  {
    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    fixUp(BinaryAssemblyASTNodeFixUpInput &&input) final override;

    virtual ~ShiftOperatorAssemblyASTNode() override = default;
  };

  class LeftShiftAssemblyASTNode final: public ShiftOperatorAssemblyASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "LeftShift";
    }

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "sall";
    }

    public:
    virtual ~LeftShiftAssemblyASTNode() final override = default;
  };

  class RightShiftAssemblyASTNode final: public ShiftOperatorAssemblyASTNode
  {
    protected:
    virtual constexpr void printBinaryOperator(std::ostream &out) final override
    {
      out << "RightShift";
    }

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "sarl";
    }

    public:
    virtual ~RightShiftAssemblyASTNode() final override = default;
  };

  class BinaryAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::shared_ptr<BinaryOperatorAssemblyASTNode> const binary_operator{};
    std::shared_ptr<OperandAssemblyASTNode> const        source{};
    std::shared_ptr<OperandAssemblyASTNode> const        destination{};

    [[nodiscard]] std::shared_ptr<BinaryOperatorAssemblyASTNode>
    getBinaryOperator() const
    {
      return binary_operator;
    }

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode> getSource() const
    {
      return source;
    }

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode> getDestination() const
    {
      return destination;
    }

    public:
    BinaryAssemblyASTNode(
      std::shared_ptr<BinaryOperatorAssemblyASTNode> binary_operator,
      std::shared_ptr<OperandAssemblyASTNode>        source,
      std::shared_ptr<OperandAssemblyASTNode>        destination
    )
      : binary_operator{ binary_operator }
      , source{ source }
      , destination{ destination }
    {}

    [[nodiscard]] virtual ReplacePseudoRegistersResult<
      InstructionAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input) final override
    {
      auto [offset_zero, map_zero, new_source]{
        getSource()->replacePseudoRegisters(std::move(input))
      };
      auto [offset_one, map_one, new_destination]{
        getDestination()->replacePseudoRegisters({ offset_zero,
                                                   std::move(map_zero) })
      };
      return { offset_one,
               std::move(map_one),
               std::make_shared<BinaryAssemblyASTNode>(
                 getBinaryOperator(),
                 std::move(new_source),
                 std::move(new_destination)
               ) };
    }

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    fixUp() override
    {
      return getBinaryOperator()->fixUp({ getSource(), getDestination() });
    }

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      getBinaryOperator()->emitCode(out);
      out << ' ';
      getSource()->emitCode(out);
      out << ", ";
      getDestination()->emitCode(out);
      out << '\n';
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Binary (";
      getBinaryOperator()->prettyPrintHelper(out, 0);
      out << " (";
      getSource()->prettyPrintHelper(out, 0);
      out << "), (";
      getDestination()->prettyPrintHelper(out, 0);
      out << "))\n";
    }

    virtual ~BinaryAssemblyASTNode() final override = default;
  };

  class CmpAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::shared_ptr<OperandAssemblyASTNode> const left_operand{};
    std::shared_ptr<OperandAssemblyASTNode> const right_operand{};

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode> getLeftOperand() const
    {
      return left_operand;
    }

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode>
    getRightOperand() const
    {
      return right_operand;
    }

    public:
    CmpAssemblyASTNode(
      std::shared_ptr<OperandAssemblyASTNode> left_operand,
      std::shared_ptr<OperandAssemblyASTNode> right_operand
    )
      : left_operand{ left_operand }
      , right_operand{ right_operand }
    {}

    [[nodiscard]] virtual ReplacePseudoRegistersResult<
      InstructionAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input) final override
    {
      auto [offset_zero, map_zero, new_left_operand]{
        getLeftOperand()->replacePseudoRegisters(std::move(input))
      };
      auto [offset_one, map_one, new_right_operand]{
        getRightOperand()->replacePseudoRegisters({ offset_zero,
                                                    std::move(map_zero) })
      };
      return { offset_one,
               std::move(map_one),
               std::make_shared<CmpAssemblyASTNode>(
                 std::move(new_left_operand),
                 std::move(new_right_operand)
               ) };
    }

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    fixUp() final override;

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      Utility::indent(out, 2);
      out << "cmpl ";
      getLeftOperand()->emitCode(out);
      out << ", ";
      getRightOperand()->emitCode(out);
      out << '\n';
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Cmp (";
      getLeftOperand()->prettyPrintHelper(out, 0);
      out << "), (";
      getRightOperand()->prettyPrintHelper(out, 0);
      out << ")\n";
    }

    virtual ~CmpAssemblyASTNode() final override = default;
  };

  class JmpAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::string const identifier{};

    [[nodiscard]] std::string_view getIdentifier() const { return identifier; }

    public:
    explicit constexpr JmpAssemblyASTNode(std::string_view identifier)
      : identifier{ identifier }
    {}

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      Utility::indent(out, 2);
      out << "jmp " << Utility::emitLocalLabelPrefix() << getIdentifier()
          << '\n';
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Jmp " << getIdentifier() << "\n";
    }

    virtual ~JmpAssemblyASTNode() final override = default;
  };

  struct CondCodeAssemblyASTNode: public AssemblyASTNode
  {};

  struct ECondCodeAssemblyASTNode final: public CondCodeAssemblyASTNode
  {
    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << 'e';
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << 'E';
    }
  };

  struct NECondCodeAssemblyASTNode final: public CondCodeAssemblyASTNode
  {
    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "ne";
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "NE";
    }
  };

  struct GCondCodeAssemblyASTNode final: public CondCodeAssemblyASTNode
  {
    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << 'g';
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << 'G';
    }
  };

  struct GECondCodeAssemblyASTNode final: public CondCodeAssemblyASTNode
  {
    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "ge";
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "GE";
    }
  };

  struct LCondCodeAssemblyASTNode final: public CondCodeAssemblyASTNode
  {
    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << 'l';
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << 'L';
    }
  };

  struct LECondCodeAssemblyASTNode final: public CondCodeAssemblyASTNode
  {
    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "le";
    }

    virtual constexpr void
    prettyPrintHelper(std::ostream &out, std::size_t) final override
    {
      out << "LE";
    }
  };

  class JmpCCAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::shared_ptr<CondCodeAssemblyASTNode> const condition_code{};
    std::string const                              identifier{};

    [[nodiscard]] std::shared_ptr<CondCodeAssemblyASTNode>
    getConditionCode() const
    {
      return condition_code;
    }

    [[nodiscard]] std::string_view getIdentifier() const { return identifier; }

    public:
    JmpCCAssemblyASTNode(
      std::shared_ptr<CondCodeAssemblyASTNode> condition_code,
      std::string_view                         identifier
    )
      : condition_code{ condition_code }
      , identifier{ identifier }
    {}

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << 'j';
      getConditionCode()->emitCode(out);
      out << ' ' << Utility::emitLocalLabelPrefix() << getIdentifier() << '\n';
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "JmpCC ";
      getConditionCode()->prettyPrintHelper(out, 0);
      out << ", " << getIdentifier() << "\n";
    }

    virtual ~JmpCCAssemblyASTNode() final override = default;
  };

  class SetCCAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::shared_ptr<CondCodeAssemblyASTNode> const condition_code{};
    std::shared_ptr<OperandAssemblyASTNode> const  destination{};

    [[nodiscard]] std::shared_ptr<CondCodeAssemblyASTNode>
    getConditionCode() const
    {
      return condition_code;
    }

    [[nodiscard]] std::shared_ptr<OperandAssemblyASTNode> getDestination() const
    {
      return destination;
    }

    public:
    SetCCAssemblyASTNode(
      std::shared_ptr<CondCodeAssemblyASTNode> condition_code,
      std::shared_ptr<OperandAssemblyASTNode>  destination
    )
      : condition_code{ condition_code }
      , destination{ destination }
    {}

    [[nodiscard]] virtual ReplacePseudoRegistersResult<
      InstructionAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input) final override
    {
      auto [offset_zero, map_zero, new_destination]{
        getDestination()->replacePseudoRegisters(std::move(input))
      };
      return { offset_zero,
               std::move(map_zero),
               std::make_shared<SetCCAssemblyASTNode>(
                 getConditionCode(),
                 std::move(new_destination)
               ) };
    }

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    fixUp() final override;

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "set";
      getConditionCode()->emitCode(out);
      out << ' ';
      getDestination()->emitCode(out);
      out << '\n';
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "SetCC ";
      getConditionCode()->prettyPrintHelper(out, 0);
      out << ", (";
      getDestination()->prettyPrintHelper(out, 0);
      out << ")\n";
    }

    virtual ~SetCCAssemblyASTNode() final override = default;
  };

  class LabelAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::string const identifier{};

    [[nodiscard]] std::string_view getIdentifier() const { return identifier; }

    public:
    explicit constexpr LabelAssemblyASTNode(std::string_view identifier)
      : identifier{ identifier }
    {}

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << Utility::emitLocalLabelPrefix() << getIdentifier() << ":\n";
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Label " << getIdentifier() << '\n';
    }

    virtual ~LabelAssemblyASTNode() final override = default;
  };

  class AllocateStackAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::intptr_t const size{};

    public:
    explicit constexpr AllocateStackAssemblyASTNode(int size): size{ size } {}

    [[nodiscard]] constexpr std::intptr_t getSize() const noexcept
    {
      return size;
    }

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "subq $" << getSize() << ", %rsp\n";
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "AllocateStack(" << getSize() << ")\n";
    }

    virtual ~AllocateStackAssemblyASTNode() final override = default;
  };

  struct CdqAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "cdq\n";
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Cdq\n";
    }

    virtual ~CdqAssemblyASTNode() final override = default;
  };

  class IdivAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    std::shared_ptr<OperandAssemblyASTNode> const operand{};

    [[nodiscard]] constexpr std::shared_ptr<OperandAssemblyASTNode>
    getOperand() const noexcept
    {
      return operand;
    }

    public:
    explicit constexpr IdivAssemblyASTNode(
      std::shared_ptr<OperandAssemblyASTNode> operand
    )
      : operand{ operand }
    {}

    [[nodiscard]] virtual ReplacePseudoRegistersResult<
      InstructionAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input)
    {
      auto [offset, map, new_operand]{
        getOperand()->replacePseudoRegisters(std::move(input))
      };
      return ReplacePseudoRegistersResult<InstructionAssemblyASTNode>{
        offset,
        std::move(map),
        std::make_shared<IdivAssemblyASTNode>(std::move(new_operand))
      };
    }

    [[nodiscard]] virtual std::vector<
      std::shared_ptr<InstructionAssemblyASTNode>>
    fixUp() final override
    {
      return std::dynamic_pointer_cast<ImmediateValueAssemblyASTNode>(
               getOperand()
             ) ?
               std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
                 std::make_shared<MovlAssemblyASTNode>(
                   getOperand(),
                   std::make_shared<R10DRegisterAssemblyASTNode>()
                 ),
                 std::make_shared<IdivAssemblyASTNode>(
                   std::make_shared<R10DRegisterAssemblyASTNode>()
                 )
               } :
               std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
                 std::dynamic_pointer_cast<InstructionAssemblyASTNode>(
                   shared_from_this()
                 )
               };
    }

    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "idivl ";
      getOperand()->emitCode(out);
      out << '\n';
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Idiv (";
      getOperand()->prettyPrintHelper(out, indent_level);
      out << ")\n";
    }

    virtual ~IdivAssemblyASTNode() final override = default;
  };

  struct ReturnAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    virtual constexpr void emitCode(std::ostream &out) final override
    {
      out << "movq %rbp, %rsp\n"
             "popq %rbp\n"
             "ret\n";
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Ret\n";
    }

    virtual ~ReturnAssemblyASTNode() final override = default;
  };

  class FunctionAssemblyASTNode final: public AssemblyASTNode
  {
    std::string const                                        identifier{};
    std::vector<std::shared_ptr<InstructionAssemblyASTNode>> instructions{};

    [[nodiscard]] constexpr std::span<
      std::shared_ptr<InstructionAssemblyASTNode>>
    getInstructions() noexcept
    {
      return instructions;
    }

    public:
    constexpr FunctionAssemblyASTNode(
      std::string_view                                         identifier,
      std::vector<std::shared_ptr<InstructionAssemblyASTNode>> instructions
    )
      : identifier{ identifier }
      , instructions{ std::move(instructions) }
    {}

    [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept
    {
      return identifier;
    }

    [[nodiscard]] constexpr std::span<
      std::shared_ptr<InstructionAssemblyASTNode> const>
    getInstructions() const
    {
      return instructions;
    }

    [[nodiscard]] ReplacePseudoRegistersResult<FunctionAssemblyASTNode>
    replacePseudoRegisters(ReplacePseudoRegistersInput &&input)
    {
      std::vector<std::shared_ptr<InstructionAssemblyASTNode>>
        new_instructions{};
      auto [offset, map]{ std::move(input) };
      for (auto &instruction: getInstructions()) {
        auto [new_offset, new_map, new_instruction]{
          instruction->replacePseudoRegisters({ offset, std::move(map) })
        };
        offset = new_offset;
        map    = std::move(new_map);
        new_instructions.push_back(new_instruction);
      }
      return { offset,
               std::move(map),
               std::make_shared<FunctionAssemblyASTNode>(
                 getIdentifier(),
                 std::move(new_instructions)
               ) };
    }

    [[nodiscard]] std::shared_ptr<FunctionAssemblyASTNode>
    fixUp(std::intptr_t size)
    {
      std::vector<std::shared_ptr<InstructionAssemblyASTNode>> new_instructions{
        std::make_shared<AllocateStackAssemblyASTNode>(size)
      };
      for (auto &instruction: getInstructions())
        for (auto &new_instruction: instruction->fixUp())
          new_instructions.push_back(new_instruction);
      return std::make_shared<FunctionAssemblyASTNode>(
        getIdentifier(),
        new_instructions
      );
    }

    constexpr void emitCode(std::ostream &out) final override
    {
      Utility::indent(out, 2);
      std::string const specialised_function_name{
        Utility::specialiseFunctionNameForOS(getIdentifier())
      };
      out << ".globl " << specialised_function_name << '\n';
      out << specialised_function_name << ":\n";
      Utility::indent(out, 2);
      out << "pushq %rbp\n";
      Utility::indent(out, 2);
      out << "movq %rsp, %rbp\n";
      for (auto const &instruction: getInstructions()) {
        instruction->emitCode(out);
      }
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Function: " << getIdentifier() << "\n";
      for (auto const &instruction: getInstructions()) {
        instruction->prettyPrintHelper(out, indent_level + 2);
      };
    }

    virtual ~FunctionAssemblyASTNode() final override = default;
  };

  class ProgramAssemblyASTNode final: public AssemblyASTNode
  {
    std::shared_ptr<FunctionAssemblyASTNode> function{};

    public:
    ProgramAssemblyASTNode(std::shared_ptr<FunctionAssemblyASTNode> function)
      : function{ function }
    {}

    [[nodiscard]] std::shared_ptr<FunctionAssemblyASTNode> getFunction() const
    {
      return function;
    }

    [[nodiscard]] ReplacePseudoRegistersResult<ProgramAssemblyASTNode>
    replacePseudoRegisters()
    {
      auto [offset, map, new_function]{ getFunction()->replacePseudoRegisters({
        0,
        {},
      }) };
      return { offset,
               std::move(map),
               std::make_shared<ProgramAssemblyASTNode>(std::move(new_function)
               ) };
    }

    [[nodiscard]] std::shared_ptr<ProgramAssemblyASTNode>
    fixUp(std::intptr_t size)
    {
      return std::make_shared<ProgramAssemblyASTNode>(getFunction()->fixUp(size)
      );
    }

    virtual void emitCode(std::ostream &out) final override
    {
      getFunction()->emitCode(out);
      Utility::emitAssemblyEpilogue(out);
    }

    virtual void prettyPrintHelper(std::ostream &out, std::size_t indent_level)
      final override
    {
      out << "Program:\n";
      getFunction()->prettyPrintHelper(out, indent_level + 2);
    }

    virtual ~ProgramAssemblyASTNode() final override = default;
  };
} // namespace SC2

#endif
