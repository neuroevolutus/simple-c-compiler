#ifndef SC2_PARSER_HPP_INCLUDED
#define SC2_PARSER_HPP_INCLUDED

#include <sc2/ast.hpp>
#include <sc2/compiler_error.hpp>
#include <sc2/lexer.hpp>
#include <sc2/semantic_analysis_error.hpp>
#include <sc2/tokens.hpp>
#include <unordered_map>

#include <concepts>
#include <format>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace SC2 {

  struct ParserError: public CompilerError
  {
    virtual ~ParserError() = default;
  };

  class ParserNonTerminalError final: public ParserError
  {
    std::string const message{};

    public:
    ParserNonTerminalError(
      std::string_view   non_terminal,
      ParserError const &childError
    )
      : message{ std::format(
          "Parser error: invalid non-terminal <{}>:\n{}",
          non_terminal,
          childError.what()
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~ParserNonTerminalError() final override = default;
  };

  class ParserTokenCreationError final: public ParserError
  {
    std::string const message{};

    public:
    ParserTokenCreationError(TokenConversionError const &error)
      : message{ std::format(
          "Parser error: cannot create ({}) from ({})",
          error.getDestinationTokenType(),
          error.getSourceToken().toString()
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~ParserTokenCreationError() final override = default;
  };

  class ParserTokenExpectationError final: public ParserError
  {
    std::string const message{};

    public:
    ParserTokenExpectationError(Token expected_token, Token actual_token)
      : message{ std::format(
          "Parser error: expected ({}) but got ({})",
          expected_token.toString(),
          actual_token.toString()
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~ParserTokenExpectationError() final override = default;
  };

  struct ParserUnmatchedParenthesesError final: public ParserError
  {
    virtual constexpr char const *what() const noexcept final override
    {
      return "Parser error: unmatched parentheses";
    }

    virtual ~ParserUnmatchedParenthesesError() final override = default;
  };

  struct ParserEOFError final: public ParserError
  {
    static constexpr char const * const message{
      "Parser error: reached end of file"
    };

    virtual constexpr char const *what() const noexcept final override
    {
      return message;
    }

    virtual ~ParserEOFError() final override = default;
  };

  struct ParserExtraneousTokenError final: public ParserError
  {
    std::string const message{};

    public:
    ParserExtraneousTokenError(Token token)
      : message{
        std::format("Parser error: extraneous token: ({})", token.toString())
      }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~ParserExtraneousTokenError() final override = default;
  };

  class ParserInvalidTokenError final: public ParserError
  {
    std::string const message{};

    public:
    ParserInvalidTokenError(LexerInvalidTokenError const &error)
      : message{ std::format("{}", error.what()) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~ParserInvalidTokenError() final override = default;
  };

  class InvalidLValueError final
    : public ParserError
    , public SemanticAnalysisError
  {
    std::string const message{};

    public:
    constexpr InvalidLValueError(std::shared_ptr<ExpressionASTNode> rvalue)
      : message{ std::format(
          "Semantic analysis error: invalid lvalue: ({})",
          rvalue->prettyPrint()
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~InvalidLValueError() final override = default;
  };

  class UnknownTypeNameError final
    : public ParserError
    , public SemanticAnalysisError
  {
    std::string const message{};

    public:
    UnknownTypeNameError(std::string_view type_name)
      : message{
        std::format("Semantic analysis error: unknown type {}", type_name)
      }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~UnknownTypeNameError() final override = default;
  };

  class TypeRedefinitionError final
    : public ParserError
    , public SemanticAnalysisError
  {
    std::string const message{};

    public:
    TypeRedefinitionError(
      std::string_view type_name,
      std::string_view original_type,
      std::string_view new_type
    )
      : message{ std::format(
          "Semantic analysis error: cannot redefine previously defined alias "
          "({}) with type {} as an alias to type {}",
          type_name,
          original_type,
          new_type
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~TypeRedefinitionError() final override = default;
  };

  class SymbolTypeRedefinitionError final
    : public ParserError
    , public SemanticAnalysisError
  {
    std::string const message{};

    public:
    SymbolTypeRedefinitionError(
      std::string_view type_name,
      std::string_view original_symbol_type,
      std::string_view new_symbol_type
    )
      : message{ std::format(
          "Semantic analysis error: cannot redefine name ({}) with symbol type "
          "<{}> as having symbol type <{}>",
          type_name,
          original_symbol_type,
          new_symbol_type
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~SymbolTypeRedefinitionError() final override = default;
  };

  class VariableRedeclarationError final
    : public ParserError
    , public SemanticAnalysisError
  {
    std::string const message{};

    public:
    VariableRedeclarationError(std::string_view variable)
      : message{ std::format(
          "Semantic analysis error: cannot redeclare variable {}",
          variable
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~VariableRedeclarationError() final override = default;
  };

  class InvalidTypeAliasError final
    : public ParserError
    , public SemanticAnalysisError
  {
    std::string const message{};

    public:
    InvalidTypeAliasError(std::string_view identifier)
      : message{ std::format(
          "Semantic analysis error: cannot use ({}) as a type alias",
          identifier
        ) }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~InvalidTypeAliasError() final override = default;
  };

  class UndefinedVariableError final
    : public ParserError
    , public SemanticAnalysisError
  {
    std::string const message{};

    public:
    UndefinedVariableError(std::string_view variable)
      : message{
        std::format("Semantic analysis error: undefined variable {}", variable)
      }
    {}

    virtual constexpr char const *what() const noexcept final override
    {
      return message.c_str();
    }

    virtual ~UndefinedVariableError() final override = default;
  };

  class TypeAliasToTypeMap
  {
    std::unordered_map<std::string, std::shared_ptr<TypeASTNode>> map{};

    public:
    constexpr TypeAliasToTypeMap() = default;

    TypeAliasToTypeMap(
      std::unordered_map<std::string, std::shared_ptr<TypeASTNode>> map
    )
      : map{ map }
    {}

    [[nodiscard]] std::shared_ptr<TypeASTNode> getType(std::string_view alias
    ) const
    {
      try {
        return map.at(std::string{ alias });
      } catch (std::out_of_range const &) {
        throw UnknownTypeNameError(alias);
      }
    }

    void aliasType(std::shared_ptr<TypeASTNode> type, std::string_view alias)
    {
      map[std::string{ alias }] = type;
    }

    [[nodiscard]] bool contains(std::string_view name) const
    {
      return map.contains(std::string{ name });
    }
  };

  class VariableToTypeAndUniqueIdentifierMap
  {
    std::unordered_map<
      std::string,
      std::tuple<std::shared_ptr<TypeASTNode>, std::string>>
      map{};

    public:
    constexpr VariableToTypeAndUniqueIdentifierMap() = default;

    [[nodiscard]] bool contains(std::string_view identifier) const
    {
      return map.contains(std::string{ identifier });
    }

    [[nodiscard]] std::shared_ptr<TypeASTNode> getType(std::string_view variable
    ) const
    try {
      return std::get<0>(map.at(std::string{ variable }));
    } catch (std::out_of_range const &) {
      throw UndefinedVariableError(variable);
    }

    [[nodiscard]] std::string_view getUniqueIdentifier(std::string_view variable
    ) const
    try {
      return std::get<1>(map.at(std::string{ variable }));
    } catch (std::out_of_range const &) {
      throw UndefinedVariableError(variable);
    }

    void assignTypeAndUniqueIdentifier(
      std::string_view             variable,
      std::string_view             current_function_name,
      std::shared_ptr<TypeASTNode> type
    )
    {
      std::string const identifier{ variable };
      std::string const unique_identifier{ std::format(
        "{}.{}",
        Utility::generateFreshIdentifierWithPrefix(current_function_name),
        identifier
      ) };

      map[identifier] = std::make_tuple(type, unique_identifier);
    }
  };

  class SemanticAnalysisIdentifierInfo
  {
    VariableToTypeAndUniqueIdentifierMap
                       variable_to_type_and_unique_identifier_map{};
    TypeAliasToTypeMap type_alias_to_type_map{};

    public:
    [[nodiscard]] constexpr auto
    getVariableToTypeAndUniqueIdentifierMap(this auto &self
    ) noexcept -> decltype(auto)
    {
      return (self.variable_to_type_and_unique_identifier_map);
    }

    [[nodiscard]] constexpr auto getTypeAliasToTypeMap(this auto &self
    ) noexcept -> decltype(auto)
    {
      return (self.type_alias_to_type_map);
    }
  };

  class Parser
  {
    std::exception_ptr current_exception{};
    std::vector<Token> tokens{};
    Lexer             &lexer;

    [[nodiscard]] Token peekNextToken()
    try {
      return *lexer;
    } catch (LexerEOFError const &) {
      throw ParserEOFError{};
    }

    // We have to do some gymnastics here so that we don't get confusing error
    // messages. We want a non-terminal to parse correctly even if the *next*
    // token following the non-terminal is invalid.
    [[nodiscard]] Token parseNextToken()
    try {
      if (current_exception) {
        try {
          std::rethrow_exception(current_exception);
        } catch (LexerInvalidTokenError const &error) {
          throw ParserInvalidTokenError(error);
        }
      }
      Token next_token{ *lexer };
      tokens.push_back(next_token);
      try {
        ++lexer;
      } catch (...) {
        current_exception = std::current_exception();
      }
      return next_token;
    } catch (LexerEOFError const &) {
      throw ParserEOFError{};
    }

    [[nodiscard]] std::shared_ptr<IdentifierToken> parseIdentifierToken()
    try {
      return parseNextToken().getIdentifier();
    } catch (TokenConversionError const &error) {
      throw ParserTokenCreationError(error);
    }

    [[nodiscard]] std::shared_ptr<LiteralConstantToken>
    parseLiteralConstantToken()
    try {
      return parseNextToken().getLiteralConstant();
    } catch (TokenConversionError const &error) {
      throw ParserTokenCreationError(error);
    }

    void expect(Token const &expected_token)
    {
      if (auto const actual_token{ parseNextToken() };
          expected_token != actual_token)
        throw ParserTokenExpectationError(expected_token, actual_token);
    }

    constexpr void expectFinished()
    {
      if (lexer != lexer.end()) throw ParserExtraneousTokenError(*lexer);
    }

    [[nodiscard]] std::shared_ptr<LiteralConstantASTNode>
    parseLiteralConstantExpression()
    {
      return std::make_shared<LiteralConstantASTNode>(
        parseLiteralConstantToken()->getValue()
      );
    }

    [[nodiscard]] std::shared_ptr<VariableASTNode>
    parseVariable(VariableToTypeAndUniqueIdentifierMap &map)
    {
      std::string const variable{ parseIdentifierToken()->getName() };
      return std::make_shared<VariableASTNode>(map.getUniqueIdentifier(variable)
      );
    }

    [[nodiscard]] std::shared_ptr<PrefixUnaryOperatorASTNode>
    parsePrefixUnaryOperator()
    {
      if (Token const next_token{ parseNextToken() }; next_token.isTilde())
        return std::make_shared<ComplementASTNode>();
      else if (next_token.isHyphen())
        return std::make_shared<NegateASTNode>();
      else if (next_token.isExclamationPoint())
        return std::make_shared<NotASTNode>();
      else if (next_token.isIncrement())
        return std::make_shared<PrefixIncrementASTNode>();
      else if (next_token.isDecrement())
        return std::make_shared<PrefixDecrementASTNode>();
      else
        std::unreachable();
    }

    [[nodiscard]] std::shared_ptr<ExpressionASTNode>
    parseFactor(VariableToTypeAndUniqueIdentifierMap &);

    [[nodiscard]] std::shared_ptr<PostfixUnaryOperatorASTNode>
    parsePostfixUnaryOperator()
    {
      if (Token const next_token{ parseNextToken() }; next_token.isDecrement())
        return std::make_shared<PostfixDecrementASTNode>();
      else if (next_token.isIncrement())
        return std::make_shared<PostfixIncrementASTNode>();
      else
        std::unreachable();
    }

    [[nodiscard]] std::shared_ptr<BinaryOperatorASTNode> parseBinaryOperator()
    {
      if (Token const next_token{ parseNextToken() }; next_token.isPlusSign())
        return std::make_shared<AddASTNode>();
      else if (next_token.isHyphen())
        return std::make_shared<SubtractASTNode>();
      else if (next_token.isAsterisk())
        return std::make_shared<MultiplyASTNode>();
      else if (next_token.isForwardSlash())
        return std::make_shared<DivideASTNode>();
      else if (next_token.isPercentSign())
        return std::make_shared<ModuloASTNode>();
      else if (next_token.isBitwiseAnd())
        return std::make_shared<BitwiseAndASTNode>();
      else if (next_token.isBitwiseOr())
        return std::make_shared<BitwiseOrASTNode>();
      else if (next_token.isBitwiseXor())
        return std::make_shared<BitwiseXorASTNode>();
      else if (next_token.isLeftShift())
        return std::make_shared<LeftShiftASTNode>();
      else if (next_token.isRightShift())
        return std::make_shared<RightShiftASTNode>();
      else if (next_token.isDoubleAmpersand())
        return std::make_shared<AndASTNode>();
      else if (next_token.isDoublePipe())
        return std::make_shared<OrASTNode>();
      else if (next_token.isEqualTo())
        return std::make_shared<EqualsASTNode>();
      else if (next_token.isNotEqualTo())
        return std::make_shared<NotEqualsASTNode>();
      else if (next_token.isLessThan())
        return std::make_shared<LessThanASTNode>();
      else if (next_token.isGreaterThan())
        return std::make_shared<GreaterThanASTNode>();
      else if (next_token.isLessThanOrEqualTo())
        return std::make_shared<LessThanOrEqualToASTNode>();
      else if (next_token.isGreaterThanOrEqualTo())
        return std::make_shared<GreaterThanOrEqualToASTNode>();
      else
        std::unreachable();
    }

    [[nodiscard]] std::shared_ptr<BasicAssignmentOperatorASTNode>
    parseAssignmentOperator()
    {
      if (Token const next_token{ parseNextToken() }; next_token.isAssignment())
        return std::make_shared<AssignmentOperatorASTNode>();
      else if (next_token.isAddAssignment())
        return std::make_shared<AddAssignmentOperatorASTNode>();
      else if (next_token.isSubtractAssignment())
        return std::make_shared<SubtractAssignmentOperatorASTNode>();
      else if (next_token.isMultiplyAssignment())
        return std::make_shared<MultiplyAssignmentOperatorASTNode>();
      else if (next_token.isDivideAssignment())
        return std::make_shared<DivideAssignmentOperatorASTNode>();
      else if (next_token.isModuloAssignment())
        return std::make_shared<ModuloAssignmentOperatorASTNode>();
      else if (next_token.isBitwiseAndAssignment())
        return std::make_shared<BitwiseAndAssignmentOperatorASTNode>();
      else if (next_token.isBitwiseOrAssignment())
        return std::make_shared<BitwiseOrAssignmentOperatorASTNode>();
      else if (next_token.isBitwiseXorAssignment())
        return std::make_shared<BitwiseXorAssignmentOperatorASTNode>();
      else if (next_token.isLeftShiftAssignment())
        return std::make_shared<LeftShiftAssignmentOperatorASTNode>();
      else if (next_token.isRightShiftAssignment())
        return std::make_shared<RightShiftAssignmentOperatorASTNode>();
      else
        std::unreachable();
    }

    [[nodiscard]] std::shared_ptr<ExpressionASTNode> parseExpression(
      std::size_t                           min_precedence,
      VariableToTypeAndUniqueIdentifierMap &map
    )
    try {
      std::shared_ptr<ExpressionASTNode> left_operand{ parseFactor(map) };
      Token                              next_token{ peekNextToken() };
      while (next_token.isBinaryOperatorToken()
             && next_token.getPrecedence() >= min_precedence) {
        if (next_token.isBasicAssignment()) {
          if (!std::dynamic_pointer_cast<VariableASTNode>(left_operand))
            throw InvalidLValueError(left_operand);
          std::shared_ptr<BasicAssignmentOperatorASTNode> const assign_operator{
            parseAssignmentOperator()
          };
          std::shared_ptr<ExpressionASTNode> const right_operand{
            parseExpression(next_token.getPrecedence(), map)
          };
          left_operand = std::make_shared<AssignmentASTNode>(
            assign_operator,
            left_operand,
            right_operand
          );
        } else {
          std::shared_ptr<BinaryOperatorASTNode> const binary_operator{
            parseBinaryOperator()
          };
          std::shared_ptr<ExpressionASTNode> const right_operand{
            parseExpression(next_token.getPrecedence() + 1, map)
          };
          left_operand = std::make_shared<BinaryExpressionASTNode>(
            binary_operator,
            left_operand,
            right_operand
          );
        }
        next_token = peekNextToken();
      }
      return left_operand;
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("expression", error);
    }

    [[nodiscard]] std::shared_ptr<TypeASTNode>
    parseType(TypeAliasToTypeMap const &map)
    try {
      Token token{ parseNextToken() };
      if (token.isIntKeyword())
        return std::make_shared<IntTypeASTNode>();
      else if (token.isVoidKeyword())
        return std::make_shared<VoidTypeASTNode>();
      else
        return map.getType(token.getIdentifier()->getName());
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("type", error);
    }

    [[nodiscard]] std::shared_ptr<BlockItemASTNode> parseDeclaration(
      std::string_view                current_function_name,
      SemanticAnalysisIdentifierInfo &info
    )
    try {
      std::shared_ptr<TypeASTNode> const type{
        parseType(info.getTypeAliasToTypeMap())
      };
      std::string const variable{ parseIdentifierToken()->getName() };
      if (info.getTypeAliasToTypeMap().contains(variable)) {
        throw SymbolTypeRedefinitionError(variable, "type", "variable");
      }
      if (info.getVariableToTypeAndUniqueIdentifierMap().contains(variable)) {
        throw VariableRedeclarationError(variable);
      }
      info.getVariableToTypeAndUniqueIdentifierMap()
        .assignTypeAndUniqueIdentifier(variable, current_function_name, type);
      std::shared_ptr<ExpressionASTNode> expression{};
      if (Token const next_token{ peekNextToken() };
          next_token.isAssignment()) {
        expect(Token(std::make_shared<AssignmentToken>()));
        expression
          = parseExpression(0, info.getVariableToTypeAndUniqueIdentifierMap());
      }
      expect(Token(std::make_shared<SemicolonToken>()));
      return std::make_shared<DeclarationASTNode>(
        type,
        info.getVariableToTypeAndUniqueIdentifierMap().getUniqueIdentifier(
          variable
        ),
        expression
      );
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("declaration", error);
    }

    [[nodiscard]] std::shared_ptr<BlockItemASTNode> parseNullStatement()
    try {
      expect(Token(std::make_shared<SemicolonToken>()));
      return std::make_shared<NullStatementASTNode>();
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("null statement", error);
    }

    [[nodiscard]] std::shared_ptr<BlockItemASTNode>
    parseReturnStatement(VariableToTypeAndUniqueIdentifierMap &map)
    try {
      expect(Token(std::make_shared<ReturnKeywordToken>()));
      std::shared_ptr<ExpressionASTNode> const expression{
        parseExpression(0, map)
      };
      expect(Token(std::make_shared<SemicolonToken>()));
      return std::make_shared<ReturnStatementASTNode>(expression);
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("return statement", error);
    }

    [[nodiscard]] std::shared_ptr<BlockItemASTNode>
    parseExpressionStatement(VariableToTypeAndUniqueIdentifierMap &map)
    try {
      std::shared_ptr<ExpressionASTNode> const expression{
        parseExpression(0, map)
      };
      expect(Token(std::make_shared<SemicolonToken>()));
      return std::make_shared<ExpressionStatementASTNode>(expression);
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("expression statement", error);
    }

    void parseTypeAlias(
      std::shared_ptr<TypeASTNode>    type,
      SemanticAnalysisIdentifierInfo &info
    )
    {
      Token const token{ parseNextToken() };
      if (token.isKeyword())
        throw InvalidTypeAliasError(token.getKeyword()->toString());
      std::string const identifier{ token.getIdentifier()->getName() };
      if (info.getVariableToTypeAndUniqueIdentifierMap().contains(identifier)) {
        throw SymbolTypeRedefinitionError(identifier, "variable", "type");
      }
      if (info.getTypeAliasToTypeMap().contains(identifier)) {
        if (*type != *info.getTypeAliasToTypeMap().getType(identifier)) {
          throw TypeRedefinitionError(
            identifier,
            info.getTypeAliasToTypeMap().getType(identifier)->toString(),
            type->toString()
          );
        }
      }
      info.getTypeAliasToTypeMap().aliasType(type, identifier);
    }

    void parseCommaAndTypeAlias(
      std::shared_ptr<TypeASTNode>    type,
      SemanticAnalysisIdentifierInfo &info
    )
    {
      expect(Token(std::make_shared<CommaToken>()));
      parseTypeAlias(type, info);
    }

    void parseTypedefTail(
      std::shared_ptr<TypeASTNode>    type,
      SemanticAnalysisIdentifierInfo &info
    )
    {
      Token next_token{};
      parseTypeAlias(type, info);
      while (!(next_token = peekNextToken()).isSemicolon()) {
        parseCommaAndTypeAlias(type, info);
      }
      expect(Token(std::make_shared<SemicolonToken>()));
    }

    void parseTypedef(SemanticAnalysisIdentifierInfo &info)
    try {
      expect(Token(std::make_shared<TypedefKeywordToken>()));
      Token                        next_token{ peekNextToken() };
      std::shared_ptr<TypeASTNode> aliased_type{};
      if (next_token.isType())
        aliased_type = parseType(info.getTypeAliasToTypeMap());
      else {
        std::string const aliased_aliased_type{ parseIdentifierToken()->getName(
        ) };
        if (!info.getTypeAliasToTypeMap().contains(aliased_aliased_type)) {
          throw UnknownTypeNameError(aliased_aliased_type);
        } else
          aliased_type
            = info.getTypeAliasToTypeMap().getType(aliased_aliased_type);
      }
      parseTypedefTail(aliased_type, info);
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("typedef", error);
    }

    [[nodiscard]] std::shared_ptr<BlockItemASTNode> parseBlockItem(
      std::string_view                current_function_name,
      SemanticAnalysisIdentifierInfo &info
    )
    try {
      // Use nullptr for first member of return value to indicate no node parsed
      if (Token next_token{ peekNextToken() }; next_token.isSemicolon()) {
        return parseNullStatement();
      } else if (next_token.isReturnKeyword()) {
        return parseReturnStatement(
          info.getVariableToTypeAndUniqueIdentifierMap()
        );
      } else if (next_token.isTypedefKeyword()) {
        parseTypedef(info);
        return nullptr;
      } else if (next_token.isType() || (next_token.isIdentifier() && info.getTypeAliasToTypeMap().contains(next_token.getIdentifier()->getName()))) {
        return parseDeclaration(current_function_name, info);
      } else
        return parseExpressionStatement(
          info.getVariableToTypeAndUniqueIdentifierMap()
        );
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("block item", error);
    }

    [[nodiscard]] std::shared_ptr<FunctionASTNode>
    parseFunction(SemanticAnalysisIdentifierInfo &info)
    try {
      expect(Token(std::make_shared<IntKeywordToken>()));
      std::string const function_name{ parseIdentifierToken()->getName() };
      expect(Token(std::make_shared<LeftParenthesisToken>()));
      expect(Token(std::make_shared<VoidKeywordToken>()));
      expect(Token(std::make_shared<RightParenthesisToken>()));
      expect(Token(std::make_shared<LeftCurlyBraceToken>()));
      std::vector<std::shared_ptr<BlockItemASTNode>> const block_items{
        [this, &function_name, &info]() {
        std::vector<std::shared_ptr<BlockItemASTNode>> block_items{};
        for (Token next_token{ peekNextToken() };
             !next_token.isRightCurlyBrace();
             next_token = peekNextToken()) {
          auto block_item{ parseBlockItem(function_name, info) };
          if (block_item) block_items.push_back(std::move(block_item));
        }
        return block_items;
      }()
      };
      expect(Token(std::make_shared<RightCurlyBraceToken>()));
      return std::make_shared<FunctionASTNode>(
        function_name,
        std::move(block_items)
      );
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("function", error);
    }

    public:
    Parser(Lexer &lexer): lexer{ lexer } {}

    [[nodiscard]] std::shared_ptr<ProgramASTNode> parseProgram()
    try {
      SemanticAnalysisIdentifierInfo info{};
      auto const program{ std::make_shared<ProgramASTNode>(parseFunction(info)
      ) };
      expectFinished();
      return program;
    } catch (ParserError const &error) {
      throw ParserNonTerminalError("program", error);
    }
  };
} // namespace SC2

#endif
