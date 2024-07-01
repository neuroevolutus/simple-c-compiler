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
    virtual constexpr void emitCode(std::ostream &out) = 0;
    virtual ~AssemblyASTNode()                         = default;
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

  struct AXRegisterAssemblyASTNode final: public RegisterAssemblyASTNode
  {
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "eax";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%ax";
    }

    virtual ~AXRegisterAssemblyASTNode() final override = default;
  };

  struct DXRegisterAssemblyASTNode final: public RegisterAssemblyASTNode
  {
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "edx";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%dx";
    }

    virtual ~DXRegisterAssemblyASTNode() final override = default;
  };

  struct R10RegisterAssemblyASTNode final: public RegisterAssemblyASTNode
  {
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "r10d";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%r10";
    }

    virtual ~R10RegisterAssemblyASTNode() final override = default;
  };

  struct R11RegisterAssemblyASTNode final: public RegisterAssemblyASTNode
  {
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "r11d";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%r11";
    }

    virtual ~R11RegisterAssemblyASTNode() final override = default;
  };

  struct CLRegisterAssemblyASTNode final: public RegisterAssemblyASTNode
  {
    virtual constexpr void emitRegister(std::ostream &out) final override
    {
      out << "cl";
    }

    virtual constexpr void printRegister(std::ostream &out) final override
    {
      out << "%cl";
    }

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

  class MovAssemblyASTNode final: public InstructionAssemblyASTNode
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
    MovAssemblyASTNode(
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
               std::make_shared<MovAssemblyASTNode>(
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
          std::make_shared<MovAssemblyASTNode>(
            getSource(),
            std::make_shared<R10RegisterAssemblyASTNode>()
          ),
          std::make_shared<MovAssemblyASTNode>(
            std::make_shared<R10RegisterAssemblyASTNode>(),
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
      out << "Instruction: Mov (";
      getSource()->prettyPrintHelper(out, indent_level);
      out << "), (";
      getDestination()->prettyPrintHelper(out, indent_level);
      out << ")\n";
    }

    virtual ~MovAssemblyASTNode() final override = default;
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
      out << "Instruction: Unary (";
      getUnaryOperator()->prettyPrintHelper(out, indent_level);
      out << " (";
      getOperand()->prettyPrintHelper(out, indent_level);
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
    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Not implemented");
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
    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Not implemented");
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

    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Not implemented");
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

    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Not implemented");
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

    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Not implemented");
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

    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Not implemented");
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

    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Not implemented");
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

    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Not implemented");
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

    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Unimplemented");
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Instruction: Binary (";
      getBinaryOperator()->prettyPrintHelper(out, indent_level);
      out << " (";
      getSource()->prettyPrintHelper(out, indent_level);
      out << "), (";
      getDestination()->prettyPrintHelper(out, indent_level);
      out << "))\n";
    }

    virtual ~BinaryAssemblyASTNode() final override = default;
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
      out << "Instruction: AllocateStack(" << getSize() << ")\n";
    }

    virtual ~AllocateStackAssemblyASTNode() final override = default;
  };

  struct CdqAssemblyASTNode final: public InstructionAssemblyASTNode
  {
    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Unimplemented");
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Instruction: Cdq\n";
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
                 std::make_shared<MovAssemblyASTNode>(
                   getOperand(),
                   std::make_shared<R10RegisterAssemblyASTNode>()
                 ),
                 std::make_shared<IdivAssemblyASTNode>(
                   std::make_shared<R10RegisterAssemblyASTNode>()
                 )
               } :
               std::vector<std::shared_ptr<InstructionAssemblyASTNode>>{
                 std::dynamic_pointer_cast<InstructionAssemblyASTNode>(
                   shared_from_this()
                 )
               };
    }

    virtual constexpr void emitCode(std::ostream &) final override
    {
      throw std::runtime_error("Unimplemented");
    }

    virtual constexpr void prettyPrintHelper(
      std::ostream &out,
      std::size_t   indent_level
    ) final override
    {
      Utility::indent(out, indent_level);
      out << "Instruction: Idiv (";
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
      out << "Instruction: Ret\n";
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
