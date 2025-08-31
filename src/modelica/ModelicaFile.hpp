#ifndef MODELICA_MODELICAFILE_H
#define MODELICA_MODELICAFILE_H

#include "modelica/antlr/modelicaLexer.h"
#include "utilities/core/Filesystem.hpp"
#include "utilities/core/Logger.hpp"
#include "antlr/modelicaParser.h"
#include <string_view>
#include <memory>
#include <algorithm>

namespace openstudio::modelica {

class StoredDefinition;
class ClassDefinition;
class ComponentClause;
class ComponentDeclaration;
class Declaration;
class ModelicaSyntaxWrapper;
//template <typename T>
template <typename T, class Derived>
class ModelicaSyntax;

class ModelicaFile
{
  template <typename T, class Derived>
  friend class ModelicaSyntax;

 public:
  ModelicaFile(const openstudio::path& path);

  void parse(std::string_view text);

  std::string getText() const;
  std::string printTree() const;

  std::vector<ClassDefinition> classDefinitions();
  ClassDefinition addClassDefinition(const std::string& text);

 protected:
  void updateCTX();

  modelicaParser* parser() const;

  template <typename T, class Derived>
  void addModelicaSyntax(ModelicaSyntax<T, Derived>* syntax) {
    m_syntaxStore.emplace_back(syntax);
  }

  template <typename T, class Derived>
  void removeModelicaSyntax(ModelicaSyntax<T, Derived>* syntax) {
    m_syntaxStore.erase(std::remove_if(m_syntaxStore.begin(), m_syntaxStore.end(), [syntax](const auto& wrapper) { return wrapper == syntax; }),
                        m_syntaxStore.end());
  }

 private:
  std::unique_ptr<antlr4::ANTLRInputStream> m_inputStream{nullptr};
  std::unique_ptr<modelicaLexer> m_modelicaLexer{nullptr};
  std::unique_ptr<antlr4::CommonTokenStream> m_tokenStream{nullptr};
  std::unique_ptr<modelicaParser> m_modelicaParser{nullptr};
  std::vector<ModelicaSyntaxWrapper> m_syntaxStore;

  REGISTER_LOGGER("openstudio.modelica.ModelicaFile");
};

template <typename T, class Derived>
class ModelicaSyntax
{
  friend class ModelicaFile;

 public:
  ModelicaSyntax(ModelicaFile* m_modelicaFile, T* ctx)  // NOLINT
    : m_modelicaFile(m_modelicaFile), m_ctx(ctx), m_id(Derived::idFromCTX(ctx)) {
    m_modelicaFile->addModelicaSyntax(this);
  }
  ModelicaSyntax(const ModelicaSyntax&) = delete;  // NOLINT
  ModelicaSyntax(ModelicaSyntax&&) = default;      // NOLINT
  ModelicaSyntax& operator=(const ModelicaSyntax&) = delete;
  ModelicaSyntax& operator=(ModelicaSyntax&&) = delete;
  virtual ~ModelicaSyntax() {
    m_modelicaFile->removeModelicaSyntax(this);
  }

  std::string getText() const {
    auto* tokens = dynamic_cast<antlr4::CommonTokenStream*>(m_modelicaFile->parser()->getTokenStream());
    if (tokens && ctx()->getStart() && ctx()->getStop()) {
      return tokens->getText(ctx()->getStart(), ctx()->getStop());
    }
    return ctx()->getText();  // fallback
  }

  void updateCTX() {
    std::string currentID = id();

    // Find the same class definition in the newly parsed tree
    auto allContexts = antlr4::tree::xpath::XPath::findAll(parser()->stored_definition(), std::string(Derived::xpath), parser());

    for (auto* ctx : allContexts) {
      auto* typed_ctx = dynamic_cast<T*>(ctx);
      if (!typed_ctx) {
        continue;
      }
      const auto id = Derived::idFromCTX(typed_ctx);
      if (id == currentID) {
        auto* typed_ctx = dynamic_cast<T*>(ctx);
        assert(typed_ctx != nullptr);
        m_ctx = typed_ctx;
        break;
      }
    }
  }

  std::string id() const {
    return m_id;
  }

 protected:
  T* ctx() const {
    return m_ctx;
  }

  ModelicaFile* modelicaFile() {
    return m_modelicaFile;
  }

  modelicaParser* parser() {
    return m_modelicaFile->parser();
  }

 private:
  ModelicaFile* m_modelicaFile;
  T* m_ctx;
  std::string m_id;
};

// Non-owning type-erased wrapper for ModelicaSyntax templates
class ModelicaSyntaxWrapper
{
 public:
  // Constructor template that accepts any ModelicaSyntax<T>*
  template <typename T, class Derived>
  ModelicaSyntaxWrapper(ModelicaSyntax<T, Derived>* syntax) : m_impl(std::make_unique<Impl<T, Derived>>(syntax)) {}

  // Forward common operations through type erasure
  void updateCTX() {
    m_impl->updateCTX();
  }
  //std::string id() const { return m_impl->id(); }
  //ModelicaFile* modelicaFile() { return m_impl->modelicaFile(); }

  // Comparison operators for removal from store
  template <typename T, class Derived>
  bool operator==(const ModelicaSyntax<T, Derived>* syntax) const {
    return m_impl->equals(syntax);
  }

  template <typename T, class Derived>
  bool operator!=(const ModelicaSyntax<T, Derived>* syntax) const {
    return !(*this == syntax);
  }

 private:
  // Abstract base for type erasure
  struct ImplBase
  {
    ImplBase() = default;
    ImplBase(const ImplBase&) = default;
    ImplBase(ImplBase&&) = default;
    ImplBase& operator=(const ImplBase&) = default;
    ImplBase& operator=(ImplBase&&) = default;
    virtual ~ImplBase() = default;
    virtual void updateCTX() = 0;
    virtual bool equals(const void* ptr) const = 0;
  };

  // Concrete implementation that holds a non-owning pointer
  template <typename T, class Derived>
  struct Impl : ImplBase
  {
    ModelicaSyntax<T, Derived>* syntax;  // Non-owning pointer

    explicit Impl(ModelicaSyntax<T, Derived>* s) : syntax(s) {}

    void updateCTX() override {
      syntax->updateCTX();
    }
    //std::string id() const override { return syntax->id(); }
    //ModelicaFile* modelicaFile() override { return syntax->modelicaFile(); }
    bool equals(const void* ptr) const override {
      return syntax == ptr;
    }
  };

  std::unique_ptr<ImplBase> m_impl;
};

class ClassDefinition : public ModelicaSyntax<modelicaParser::Class_definitionContext, ClassDefinition>
{
 public:
  using ModelicaSyntax::ModelicaSyntax;
  static constexpr std::string_view xpath = "//class_definition";
  std::string longClassSpecifier() const;

  std::vector<ComponentClause> componentClauses();
  void addComponentClause(const std::string& text);

  static std::string idFromCTX(modelicaParser::Class_definitionContext* ctx);

 private:
  REGISTER_LOGGER("openstudio.modelica.ClassDefinition");
};

class ComponentClause : public ModelicaSyntax<modelicaParser::Component_clauseContext, ComponentClause>
{
 public:
  using ModelicaSyntax::ModelicaSyntax;
  static constexpr std::string_view xpath = "//component_clause";

  //std::vector<ComponentDeclaration> componentDeclarations();

  static std::string idFromCTX(modelicaParser::Component_clauseContext* ctx);
};

//class ComponentDeclaration : public ModelicaSyntax<modelicaParser::Component_declarationContext, ComponentDeclaration>
//{
// public:
//  using ModelicaSyntax::ModelicaSyntax;
//  static constexpr std::string_view xpath = "//component_declaration";
//
//  static std::string idFromCTX(modelicaParser::Component_declarationContext* ctx);
//};

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
