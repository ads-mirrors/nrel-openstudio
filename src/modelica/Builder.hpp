#ifndef MODELICA_BUILDER_H
#define MODELICA_BUILDER_H

#include <fmt/core.h>
#include <fstream>
#include <antlr4-runtime.h>
#include "antlr/modelicaLexer.h"
#include "antlr/modelicaParser.h"

namespace openstudio::modelica {

// Method to find a Modelica model in the parse tree
[[maybe_unused]] static antlr4::tree::ParseTree* findModel(modelicaParser::Stored_definitionContext* tree, const std::string& modelName) {
  for (auto* child : tree->children) {
    if (auto* classDef = dynamic_cast<modelicaParser::Class_definitionContext*>(child)) {
      auto* specifier = classDef->class_specifier();
      if ((specifier != nullptr) && specifier->long_class_specifier()->IDENT(0)->getText() == modelName) {
        return classDef;
      }
    }
  }
  return nullptr;  // Model not found
}

// Method to instantiate a new sub-model inside an existing model
[[maybe_unused]] static void instantiateSubModel(modelicaParser::Class_definitionContext* model, const std::string& instanceName,
                                                 const std::string& typeName) {
  if (!model) {
    return;
  }
  std::string newInstance = typeName + " " + instanceName + ";\n";
  model->addChild(new antlr4::tree::TerminalNodeImpl(new antlr4::CommonToken(modelicaParser::IDENT, newInstance)));
}

[[maybe_unused]] static void instantiateSubModelFromParsed(modelicaParser::Class_definitionContext* model, const std::string& instanceCode) {
  if (!model) {
    return;
  }

  // Lex and parse the input instanceCode
  antlr4::ANTLRInputStream input(instanceCode);
  modelicaLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  modelicaParser parser(&tokens);

  // Parse as a component_clause (adjust based on your grammar)
  modelicaParser::Component_clauseContext* newComponent = parser.component_clause();

  // Add the new component to the class definition
  model->addChild(newComponent);
}

// Method to add a connect() statement
[[maybe_unused]] static void addConnection(modelicaParser::Equation_sectionContext* equationSection, const std::string& source,
                                           const std::string& target) {
  if (!equationSection) {
    return;
  }
  [[maybe_unused]] std::string newConnect = "connect(" + source + ", " + target + ");\n";
  //equationSection->addChild(new TerminalNodeImpl(new CommonToken(modelicaParser::CONNECT_CLAUSE, newConnect)));
}

// Method to output modified Modelica code
[[maybe_unused]] static void printModifiedModel(modelicaParser::Stored_definitionContext* tree) {
  fmt::print("{}\n", tree->toStringTree());
}

// Method to save the modified Modelica code to a file
[[maybe_unused]] static void saveModifiedModel(modelicaParser::Stored_definitionContext* tree, const std::string& filename) {
  std::ofstream outFile(filename);
  if (outFile.is_open()) {
    outFile << tree->toStringTree() << '\n';
    outFile.close();
  } else {
    fmt::print(stderr, "Error: Unable to open file for writing.\n");
  }
}

}  // namespace openstudio::modelica

#endif  // MODELICA_BUILDER_H
