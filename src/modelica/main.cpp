#include <fmt/core.h>
#include <iostream>
#include <antlr4-runtime.h>
#include "utilities/core/ApplicationPathHelpers.hpp"
#include "ModelicaFile.hpp"

int main() {
  const auto moPath = openstudio::getApplicationSourceDirectory() / "resources/Examples/compact_osw/modelica/Template_IdealHeat/Model2.mo";

  openstudio::modelica::ModelicaFile moFile(moPath);
  //std::cout << moFile.printTree() << std::flush;

  std::cout << moFile.print() << std::flush;

  auto model2Class = moFile.classDefinitions().front();

  model2Class.addComponentClause(
    R"(Buildings.ThermalZones.EnergyPlus_24_2_0.ThermalZone zon2(redeclare package Medium = Medium, zoneName = "MyZone2"))");

  std::cout << "************** new file *************** \n" << std::flush;
  std::cout << moFile.print() << std::flush;

  return 0;
}
