#ifndef MODELICA_MODELICAFILE_H
#define MODELICA_MODELICAFILE_H

#include "utilities/core/Filesystem.hpp"
#include <string>
#include <vector>
#include <memory>

namespace openstudio::modelica {

namespace detail {
  class ModelicaFileImpl;
  class ClassDefinitionImpl;
}

class ClassDefinition;

class ModelicaFile
{
 public:
  ModelicaFile(const openstudio::path& path);
  
  // Copy constructor
  ModelicaFile(const ModelicaFile& other);
  
  // Move constructor
  ModelicaFile(ModelicaFile&& other) noexcept;
  
  // Copy assignment
  ModelicaFile& operator=(const ModelicaFile& other);
  
  // Move assignment
  ModelicaFile& operator=(ModelicaFile&& other) noexcept;
  
  // Destructor
  ~ModelicaFile();

  void parse(std::string_view text);

  std::string getText() const;
  std::string printTree() const;

  std::vector<ClassDefinition> getClassDefinitions();
  ClassDefinition getClassDefinitionByName(const std::string& name);
  ClassDefinition addClassDefinition(const std::string& text);

 private:
  std::shared_ptr<detail::ModelicaFileImpl> m_impl;
};

class ClassDefinition
{
 public:
  // Default constructor for invalid/empty state
  ClassDefinition();
  
  // Copy constructor
  ClassDefinition(const ClassDefinition& other);
  
  // Move constructor
  ClassDefinition(ClassDefinition&& other) noexcept;
  
  // Copy assignment
  ClassDefinition& operator=(const ClassDefinition& other);
  
  // Move assignment
  ClassDefinition& operator=(ClassDefinition&& other) noexcept;
  
  // Destructor
  ~ClassDefinition();

  std::string longClassSpecifier() const;
  std::string getText() const;
  
  void addComponentClause(const std::string& text);

 private:
  friend class ModelicaFile;
  explicit ClassDefinition(std::shared_ptr<detail::ClassDefinitionImpl> impl);
  
  std::shared_ptr<detail::ClassDefinitionImpl> m_impl;
};

}  // namespace openstudio::modelica

#endif  // MODELICA_MODELICAFILE_H
