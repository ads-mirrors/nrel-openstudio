add_library(openstudio_modelica
  OBJECT
  antlr/modelicaBaseListener.cpp
  antlr/modelicaBaseListener.h
  antlr/modelicaBaseVisitor.cpp
  antlr/modelicaBaseVisitor.h
  antlr/modelicaLexer.cpp
  antlr/modelicaLexer.h
  antlr/modelicaListener.cpp
  antlr/modelicaListener.h
  antlr/modelicaParser.cpp
  antlr/modelicaParser.h
  antlr/modelicaVisitor.cpp
  antlr/modelicaVisitor.h
)

target_link_libraries(openstudio_modelica 
  PUBLIC 
  antlr4_static
)

add_executable(antdemo
  main.cpp
  ModelicaFile.hpp
  ModelicaFile.cpp
)

target_link_libraries(antdemo
  PUBLIC
  openstudio_modelica
  openstudio_utilities
)
