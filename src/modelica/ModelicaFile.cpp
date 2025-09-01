#include "ModelicaFile.hpp"
#include "ModelicaFileImpl.hpp"

namespace openstudio::modelica {

// ModelicaFile implementation

ModelicaFile::ModelicaFile(const openstudio::path& path)
  : m_impl(std::make_shared<detail::ModelicaFileImpl>(path)) {}

ModelicaFile::ModelicaFile(const ModelicaFile& other) = default;

ModelicaFile::ModelicaFile(ModelicaFile&& other) noexcept = default;

ModelicaFile& ModelicaFile::operator=(const ModelicaFile& other) = default;

ModelicaFile& ModelicaFile::operator=(ModelicaFile&& other) noexcept = default;

ModelicaFile::~ModelicaFile() = default;

void ModelicaFile::parse(std::string_view text) {
  m_impl->parse(text);
}

std::string ModelicaFile::getText() const {
  return m_impl->getText();
}

std::string ModelicaFile::printTree() const {
  return m_impl->printTree();
}

std::vector<ClassDefinition> ModelicaFile::getClassDefinitions() {
  auto implClasses = m_impl->getClassDefinitions();
  std::vector<ClassDefinition> result;
  result.reserve(implClasses.size());
  
  for (auto& implClass : implClasses) {
    // Create shared_ptr from the moved impl object
    auto sharedImpl = std::make_shared<detail::ClassDefinitionImpl>(std::move(implClass));
    result.emplace_back(ClassDefinition(sharedImpl));
  }
  
  return result;
}

ClassDefinition ModelicaFile::getClassDefinitionByName(const std::string& name) {
  auto implClass = m_impl->getClassDefinitionByName(name);
  auto sharedImpl = std::make_shared<detail::ClassDefinitionImpl>(std::move(implClass));
  return ClassDefinition(sharedImpl);
}

ClassDefinition ModelicaFile::addClassDefinition(const std::string& text) {
  auto implClass = m_impl->addClassDefinition(text);
  auto sharedImpl = std::make_shared<detail::ClassDefinitionImpl>(std::move(implClass));
  return ClassDefinition(sharedImpl);
}

// ClassDefinition implementation

ClassDefinition::ClassDefinition() = default;

ClassDefinition::ClassDefinition(const ClassDefinition& other) = default;

ClassDefinition::ClassDefinition(ClassDefinition&& other) noexcept = default;

ClassDefinition& ClassDefinition::operator=(const ClassDefinition& other) = default;

ClassDefinition& ClassDefinition::operator=(ClassDefinition&& other) noexcept = default;

ClassDefinition::~ClassDefinition() = default;

ClassDefinition::ClassDefinition(std::shared_ptr<detail::ClassDefinitionImpl> impl)
  : m_impl(std::move(impl)) {}

std::string ClassDefinition::longClassSpecifier() const {
  if (!m_impl) {
    return "";
  }
  return m_impl->longClassSpecifier();
}

std::string ClassDefinition::getText() const {
  if (!m_impl) {
    return "";
  }
  return m_impl->getText();
}

void ClassDefinition::addComponentClause(const std::string& text) {
  if (m_impl) {
    m_impl->addComponentClause(text);
  }
}

}  // namespace openstudio::modelica
