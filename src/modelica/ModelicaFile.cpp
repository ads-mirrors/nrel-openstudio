#include "ModelicaFile.hpp"
#include "antlr/modelicaParser.h"
#include "antlr/modelicaLexer.h"
#include "../utilities/core/Logger.hpp"
#include <antlr4-runtime.h>
#include <antlr4-runtime/tree/xpath/XPath.h>
#include <fmt/core.h>

namespace openstudio::modelica {

ModelicaFile::ModelicaFile([[maybe_unused]] const openstudio::path& path) {
  std::ifstream moFile(path);
  std::stringstream buffer;
  buffer << moFile.rdbuf();

  parse(buffer.str());
}

void ModelicaFile::parse(std::string_view text) {
  //m_text = text;

  // Create ANTLR input stream
  m_inputStream = std::make_unique<antlr4::ANTLRInputStream>(text);
  m_modelicaLexer = std::make_unique<modelicaLexer>(m_inputStream.get());
  m_tokenStream = std::make_unique<antlr4::CommonTokenStream>(m_modelicaLexer.get());
  m_modelicaParser = std::make_unique<modelicaParser>(m_tokenStream.get());
  //m_ctx = m_modelicaParser->stored_definition();

  updateCTX();
}

modelicaParser* ModelicaFile::parser() const {
  return m_modelicaParser.get();
}

void ModelicaFile::updateCTX() {
  for (auto& syntax : m_syntaxStore) {
    syntax.updateCTX();
  }
}

std::string ModelicaFile::getText() const {
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

  to_s(m_modelicaParser->stored_definition(), m_modelicaParser.get(), 0);

  return ss.str();
}

std::vector<ClassDefinition> ModelicaFile::classDefinitions() {
  std::vector<ClassDefinition> result;

  auto class_definitions =
    antlr4::tree::xpath::XPath::findAll(m_modelicaParser->stored_definition(), std::string(ClassDefinition::xpath), m_modelicaParser.get());

  for (auto* ctx : class_definitions) {
    auto* class_def_ctx = dynamic_cast<modelicaParser::Class_definitionContext*>(ctx);
    assert(class_def_ctx != nullptr);

    // Only include class definitions with long class specifiers
    if (class_def_ctx->class_specifier() && class_def_ctx->class_specifier()->long_class_specifier()) {
      result.emplace_back(this, class_def_ctx);  // NOLINT
    }
  }

  return result;
}

ClassDefinition ModelicaFile::addClassDefinition(const std::string& text) {
  // First, validate that the new text is a valid class definition
  try {
    antlr4::ANTLRInputStream testStream(text);
    modelicaLexer testLexer(&testStream);
    antlr4::CommonTokenStream testTokens(&testLexer);
    modelicaParser testParser(&testTokens);

    // Try to parse as a class definition
    auto* classDefCtx = testParser.class_definition();
    if (!classDefCtx || testParser.getNumberOfSyntaxErrors() > 0) {
      LOG_AND_THROW("Invalid class definition syntax: " + text);
    }

    // Only accept long class definitions, reject short class specifiers
    if (!classDefCtx->class_specifier() || !classDefCtx->class_specifier()->long_class_specifier()) {
      LOG_AND_THROW("Only long class definitions are supported, not short class specifiers: " + text);
    }
  } catch (const std::exception& e) {
    LOG_AND_THROW("Failed to parse class definition: " + std::string(e.what()));
  }

  // Get current file content
  std::string currentContent = getText();

  // Find insertion point after the last class definition
  size_t insertPos = currentContent.length();

  // Find all existing class definitions to get the end of the last one
  auto existingClasses = classDefinitions();
  if (!existingClasses.empty()) {
    // Get the context of the last class definition
    auto& lastClass = existingClasses.back();
    auto* lastCtx = lastClass.ctx();

    // Find the end position of the last class in the original text
    auto* stopToken = lastCtx->getStop();
    if (stopToken) {
      size_t endPos = stopToken->getStopIndex() + 1;
      insertPos = endPos;
    }
  }

  // Insert the new class definition after the last one
  currentContent.insert(insertPos, "\n" + text + "\n");

  // Reparse the entire file with new content
  parse(currentContent);

  return std::move(classDefinitions().back());
}

std::string ClassDefinition::longClassSpecifier() const {
  return ctx()->class_specifier()->long_class_specifier()->IDENT(0)->getText();
}

std::vector<ComponentClause> ClassDefinition::componentClauses() {
  std::vector<ComponentClause> result;

  const auto component_clause = antlr4::tree::xpath::XPath::findAll(ctx(), std::string(ComponentClause::xpath), parser());

  for (auto* c : component_clause) {
    auto* component_clause_ctx = dynamic_cast<modelicaParser::Component_clauseContext*>(c);
    assert(component_clause_ctx != nullptr);
    result.emplace_back(modelicaFile(), component_clause_ctx);  // NOLINT
  }

  return result;
}

void ClassDefinition::addComponentClause(const std::string& text) {
  // First, validate that the new text is a valid component clause
  try {
    antlr4::ANTLRInputStream testStream(text);
    modelicaLexer testLexer(&testStream);
    antlr4::CommonTokenStream testTokens(&testLexer);
    modelicaParser testParser(&testTokens);

    // Try to parse as a component clause
    auto* componentClauseCtx = testParser.component_clause();
    if (!componentClauseCtx || testParser.getNumberOfSyntaxErrors() > 0) {
      LOG_AND_THROW("Invalid component clause syntax: " + text);
    }
  } catch (const std::exception& e) {
    LOG_AND_THROW("Failed to parse component clause: " + std::string(e.what()));
  }

  // Find insertion point - look for the composition within this class
  auto* composition = ctx()->class_specifier()->long_class_specifier()->composition();
  if (!composition) {
    LOG_AND_THROW("Cannot find composition in class definition");
  }

  // Get the element list (vector of elements)
  auto elementList = composition->element_list();

  // Find the end position - either after the last element or at the composition end
  antlr4::Token* stopToken = nullptr;
  if (!elementList.empty()) {
    // Insert after the last element
    stopToken = elementList.back()->getStop();
  } else {
    // Empty element list, insert at the start of composition
    stopToken = composition->getStart();
  }
  if (!stopToken) {
    LOG_AND_THROW("Cannot determine insertion position for component clause");
  }

  // Get the full file content
  std::string fullContent = modelicaFile()->getText();

  // Calculate the insertion position using ANTLR token positions
  size_t insertPos = stopToken->getStopIndex() + 1;

  // Insert the new component clause with proper formatting
  fullContent.insert(insertPos, "\n  " + text + "\n");

  // Reparse the entire file
  modelicaFile()->parse(fullContent);
}

std::string ClassDefinition::idFromCTX(modelicaParser::Class_definitionContext* ctx) {
  auto* specifier = ctx->class_specifier();
  if (!specifier) {
    LOG_AND_THROW("ClassDefinition context has null class_specifier");
  }

  if (!specifier->long_class_specifier()) {
    LOG_AND_THROW("ClassDefinition context has null long_class_specifier");
  }

  return specifier->long_class_specifier()->IDENT(0)->getText();
}

std::string ComponentClause::idFromCTX(modelicaParser::Component_clauseContext* ctx) {
  if (!ctx) {
    return "";
  }

  std::string id;

  // Find the containing class definition to make this globally unique
  auto* parent = ctx->parent;
  while (parent) {
    auto* classDefCtx = dynamic_cast<modelicaParser::Class_definitionContext*>(parent);
    if (classDefCtx) {
      // Found the containing class, use its name as a prefix
      auto* specifier = classDefCtx->class_specifier();
      if (specifier && specifier->long_class_specifier() && specifier->long_class_specifier()->IDENT(0)) {
        id = specifier->long_class_specifier()->IDENT(0)->getText() + "::";
      }
      break;
    }
    parent = parent->parent;
  }

  // Get the type specifier (e.g., "Real", "parameter Real", "Modelica.Units.SI.Temperature")
  if (ctx->type_specifier()) {
    id += ctx->type_specifier()->getText();
  }

  // Get the first component declaration name to make it unique within the class
  if ((ctx->component_list() != nullptr) && !ctx->component_list()->component_declaration().empty()) {
    auto* firstComponent = ctx->component_list()->component_declaration()[0];
    if (firstComponent && firstComponent->declaration() && firstComponent->declaration()->IDENT()) {
      id += "::" + firstComponent->declaration()->IDENT()->getText();
    }
  }

  return id;
}

//std::vector<ComponentDeclaration> ComponentClause::componentDeclarations() {
//  std::vector<ComponentDeclaration> result;
//
//  const auto component_declarations = antlr4::tree::xpath::XPath::findAll(ctx(), "//Component_declaration", parser());
//
//  for (auto* c : component_declarations) {
//    auto* component_declaration_ctx = dynamic_cast<modelicaParser::Component_declarationContext*>(c);
//    assert(component_declaration_ctx != nullptr);
//    result.emplace_back(modelicaFile(), component_declaration_ctx);  // NOLINT
//  }
//
//  return result;
//}

//std::string ComponentDeclaration::idFromCTX([[maybe_unused]] modelicaParser::Component_declarationContext* ctx) {
//  // TODO
//  return "";
//}

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
