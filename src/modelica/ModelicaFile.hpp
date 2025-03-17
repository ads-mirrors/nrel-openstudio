#ifndef MODELICA_MODELICAFILE_H
#define MODELICA_MODELICAFILE_H

#include "modelica/antlr/modelicaLexer.h"
#include "utilities/core/Filesystem.hpp"
#include "antlr/modelicaParser.h"

namespace openstudio::modelica {

class StoredDefinition;
class ClassDefinition;
class ComponentClause;
class ComponentDeclaration;
class Declaration;

class ModelicaFile
{
 public:
  ModelicaFile(const openstudio::path& path);
  std::string print() const;
  std::string printTree() const;

  std::vector<ClassDefinition> classDefinitions() const;

 private:
  std::unique_ptr<antlr4::ANTLRInputStream> m_inputStream;
  std::unique_ptr<modelicaLexer> m_modelicaLexer;
  std::unique_ptr<antlr4::CommonTokenStream> m_tokenStream;
  std::unique_ptr<modelicaParser> m_modelicaParser;

  modelicaParser::Stored_definitionContext* m_ctx;
};

class ClassDefinition
{
 public:
  ClassDefinition(modelicaParser::Class_definitionContext* ctx, modelicaParser* parser);

  std::vector<ComponentClause> componentClauses();
  void addComponentClause(const std::string& text);

 private:
  modelicaParser::Class_definitionContext* m_ctx;
  modelicaParser* m_parser;
};

class ComponentClause
{
 public:
  ComponentClause(modelicaParser::Component_clauseContext* ctx, modelicaParser* parser);

  std::vector<ComponentDeclaration> componentDeclarations();

 private:
  modelicaParser::Component_clauseContext* m_ctx;
  modelicaParser* m_parser;
};

class ComponentDeclaration
{
 public:
  ComponentDeclaration(modelicaParser::Component_declarationContext* ctx, modelicaParser* parser);

 private:
  modelicaParser::Component_declarationContext* m_ctx;
  modelicaParser* m_parser;
};

}  // namespace openstudio::modelica

//class StoredDefinition
//{
// public:
//  std::vector<ClassDefinition> classDefinitions();
//
// protected:
//  StoredDefinition(modelicaParser::Stored_definitionContext* ctx);
//
// private:
//  antlr4::tree::ParseTree* m_ctx;
//
//  class Visitor : public modelicaBaseVisitor
//  {
//    virtual std::any visitClass_definition([[maybe_unused]] modelicaParser::Class_definitionContext* ctx) override {
//      return {};
//    }
//
//    //virtual std::any visitClass_definition(modelicaParser::Class_definitionContext* ctx) override {
//    //  std::string name = ctx->declaration()->IDENT()->getText();
//    //  fmt::print("Component Declaration {}: {}\n", name, ctx->getText());
//    //  return visitChildren(ctx);
//    //}
//
//    std::vector<ClassDefinition> m_classDefinitions;
//  };
//
//  Visitor m_classDefinitionVisitor;
//};

#endif  // MODELICA_MODELICAFILE_H
