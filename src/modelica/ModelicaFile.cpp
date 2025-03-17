#include "ModelicaFile.hpp"
#include "antlr/modelicaParser.h"
#include "antlr/modelicaLexer.h"
#include <antlr4-runtime.h>
#include <antlr4-runtime/tree/xpath/XPath.h>
#include <fmt/core.h>

namespace openstudio::modelica {

ModelicaFile::ModelicaFile([[maybe_unused]] const openstudio::path& path) {
  // Read Modelica file content into a string
  std::ifstream moFile(path);
  std::stringstream buffer;
  buffer << moFile.rdbuf();
  std::string input = buffer.str();

  // Create ANTLR input stream
  m_inputStream = std::make_unique<antlr4::ANTLRInputStream>(input);
  m_modelicaLexer = std::make_unique<modelicaLexer>(m_inputStream.get());
  m_tokenStream = std::make_unique<antlr4::CommonTokenStream>(m_modelicaLexer.get());
  m_modelicaParser = std::make_unique<modelicaParser>(m_tokenStream.get());

  m_ctx = m_modelicaParser->stored_definition();  // NOLINT
}

std::string ModelicaFile::print() const {
  return m_modelicaParser->getTokenStream()->getText();
}

std::string ModelicaFile::printTree() const {
  std::stringstream ss;

  const std::function<void(antlr4::tree::ParseTree*, antlr4::Parser*, int)> to_s = [&ss, &to_s](antlr4::tree::ParseTree* tree, antlr4::Parser* parser,
                                                                                                int indent) {
    if (!tree) {
      return;
    }

    // Print indentation
    for (int i = 0; i < indent; i++) {
      ss << "  ";
    }

    // Check if the node is a rule (internal) or a leaf (token)
    auto* ruleContext = dynamic_cast<antlr4::ParserRuleContext*>(tree);
    if (ruleContext) {
      // Internal rule node - print rule name
      ss << "rule:  " << parser->getRuleNames()[ruleContext->getRuleIndex()] << std::endl;  // NOLINT
    } else {
      // Leaf token node - print actual token text
      ss << "token: " << tree->getText() << std::endl;  // NOLINT
    }

    // Recursively print children with increased indentation
    for (auto* child : tree->children) {
      to_s(child, parser, indent + 1);
    }
  };

  to_s(m_ctx, m_modelicaParser.get(), 0);

  return ss.str();
}

std::vector<ClassDefinition> ModelicaFile::classDefinitions() const {
  std::vector<ClassDefinition> result;

  auto class_definition = antlr4::tree::xpath::XPath::findAll(m_ctx, "//class_definition", m_modelicaParser.get());
  result.reserve(class_definition.size());

  for (auto* ctx : class_definition) {
    result.emplace_back(dynamic_cast<modelicaParser::Class_definitionContext*>(ctx), m_modelicaParser.get());
  }

  return result;
}

ClassDefinition::ClassDefinition(modelicaParser::Class_definitionContext* ctx, modelicaParser* parser) : m_ctx(ctx), m_parser(parser) {}

std::vector<ComponentClause> ClassDefinition::componentClauses() {
  std::vector<ComponentClause> result;

  auto component_clause = antlr4::tree::xpath::XPath::findAll(m_ctx, "//component_clause", m_parser);
  result.reserve(component_clause.size());

  for (auto* ctx : component_clause) {
    result.emplace_back(dynamic_cast<modelicaParser::Component_clauseContext*>(ctx), m_parser);
  }

  return result;
}

void ClassDefinition::addComponentClause(const std::string& text) {
  fmt::print("New text: {}\n", text);

  auto* elementList = antlr4::tree::xpath::XPath::findAll(m_ctx, "//class_specifier/long_class_specifier/composition/element_list", m_parser).front();

  antlr4::ANTLRInputStream input(std::string("\n") + text + ";");  // Ensure it's a complete statement
  modelicaLexer lexer(&input);
  antlr4::CommonTokenStream tempTokens(&lexer);
  modelicaParser tempParser(&tempTokens);
  auto* component_clause = tempParser.component_clause();

  fmt::print("component_clause: {}\n", component_clause->getText());

  auto* tokens = m_parser->getTokenStream();
  antlr4::TokenStreamRewriter rewriter(tokens);

  const auto insertPos = elementList->getSourceInterval().b;
  fmt::print("elementList: {}\n", elementList->getText());
  fmt::print("insertPos: {}\n", insertPos);

  rewriter.insertAfter(insertPos, "\n    " + component_clause->getText() + ";");
  fmt::print("rewriter.getText()...\n{}\n", rewriter.getText());

  //std::cout << m_parser->getTokenStream()->getText() << std::endl;
}

ComponentClause::ComponentClause(modelicaParser::Component_clauseContext* ctx, modelicaParser* parser) : m_ctx(ctx), m_parser(parser) {}

std::vector<ComponentDeclaration> ComponentClause::componentDeclarations() {
  std::vector<ComponentDeclaration> result;

  auto component_declaration = antlr4::tree::xpath::XPath::findAll(m_ctx, "//Component_declaration", m_parser);
  result.reserve(component_declaration.size());

  //for (auto* ctx : component_clause) {
  //  result.emplace_back(dynamic_cast<modelicaParser::Component_clauseContext*>(ctx), m_parser);
  //}

  return result;
}

ComponentDeclaration::ComponentDeclaration(modelicaParser::Component_declarationContext* ctx, modelicaParser* parser)
  : m_ctx(ctx), m_parser(parser) {}

//std::vector<Declaration> ClassDefinition::declarations() {
//  class_definition::GetDeclarationsVisitor v;
//  v.visit(m_ctx);
//  return v.getResult();
//}
//
//void ClassDefinition::addDeclaration(const std::string& declarationText) {
//  const auto el = m_ctx->class_specifier()->long_class_specifier()->composition()->element_list();
//
//  antlr4::ANTLRInputStream input(declarationText + ";");  // Ensure it's a complete statement
//  modelicaLexer lexer(&input);
//  antlr4::CommonTokenStream tempTokens(&lexer);
//  modelicaParser tempParser(&tempTokens);
//
//  [[maybe_unused]] const auto* component = tempParser.component_clause();
//
//  // Attempt to parse as a component_clause (variable declaration)
//  modelicaParser::Component_clauseContext* parsedDecl = nullptr;
//}
//
//Declaration::Declaration(modelicaParser::DeclarationContext* ctx) : m_ctx(ctx) {}
//
//std::string Declaration::name() const {
//  return m_ctx->IDENT()->getText();
//}

}  // namespace openstudio::modelica
