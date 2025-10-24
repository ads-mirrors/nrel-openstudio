#ifndef MODELICA_MODELICAFILE_H
#define MODELICA_MODELICAFILE_H

#include "ModelicaAPI.hpp"
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

/**
 * A parsed Modelica source file backed by the OpenStudio Modelica parser.
 *
 * ModelicaFile owns the parsed token stream and exposes helper methods for
 * interrogating and mutating the class definitions contained in the file.
 */
class MODELICA_API ModelicaFile
{
 public:
  /** Construct a ModelicaFile by reading and parsing the file located at @p path. */
  ModelicaFile(const openstudio::path& path);

  /// @name Special member functions
  /// @{
  /** Copy-construct a ModelicaFile, sharing the underlying implementation. */
  ModelicaFile(const ModelicaFile& other);

  /** Move-construct a ModelicaFile, transferring ownership of the implementation. */
  ModelicaFile(ModelicaFile&& other) noexcept;

  /** Copy-assign a ModelicaFile, sharing the underlying implementation. */
  ModelicaFile& operator=(const ModelicaFile& other);

  /** Move-assign a ModelicaFile, transferring ownership of the implementation. */
  ModelicaFile& operator=(ModelicaFile&& other) noexcept;
  /// @}

  /** Destroy the ModelicaFile. */
  ~ModelicaFile();

  /** Parse Modelica source text, replacing the current contents. */
  void parse(std::string_view text);

  /** Return the raw Modelica source text represented by this file. */
  std::string getText() const;

  /**
   * Return a formatted representation of the parse tree primarily for debugging.
   */
  std::string printTree() const;

  /** Retrieve all top-level class definitions contained in the file. */
  std::vector<ClassDefinition> getClassDefinitions();

  /** Retrieve the class definition whose long class specifier matches @p name. */
  ClassDefinition getClassDefinitionByName(const std::string& name);

  /** Append a new class definition represented by @p text to the file. */
  ClassDefinition addClassDefinition(const std::string& text);

 private:
  std::shared_ptr<detail::ModelicaFileImpl> m_impl;
};

/**
 * Lightweight proxy that provides access to a single Modelica class definition.
 *
 * ClassDefinition instances are invalidated when their owning ModelicaFile reparses
 * the underlying text; to maintain validity, always reacquire handles after edits.
 */
class MODELICA_API ClassDefinition
{
 public:
  /// @name Construction & Assignment
  /// @{
  /** Create an empty/invalid ClassDefinition handle. */
  ClassDefinition();

  /** Copy-construct a ClassDefinition, sharing the underlying implementation. */
  ClassDefinition(const ClassDefinition& other);

  /** Move-construct a ClassDefinition, transferring ownership of the implementation. */
  ClassDefinition(ClassDefinition&& other) noexcept;

  /** Copy-assign a ClassDefinition, sharing the underlying implementation. */
  ClassDefinition& operator=(const ClassDefinition& other);

  /** Move-assign a ClassDefinition, transferring ownership of the implementation. */
  ClassDefinition& operator=(ClassDefinition&& other) noexcept;
  /// @}

  /** Destroy the ClassDefinition. */
  ~ClassDefinition();

  /** Return the fully-qualified name (long class specifier) of the class. */
  std::string longClassSpecifier() const;

  /** Return the raw Modelica text for this class definition. */
  std::string getText() const;

  /** Append a new component clause, represented by @p text, to the class body. */
  void addComponentClause(const std::string& text);

 private:
  friend class ModelicaFile;
  explicit ClassDefinition(std::shared_ptr<detail::ClassDefinitionImpl> impl);

  std::shared_ptr<detail::ClassDefinitionImpl> m_impl;
};

}  // namespace openstudio::modelica

#endif  // MODELICA_MODELICAFILE_H
