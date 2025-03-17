within Template_IdealHeat;
model Model2
  extends Modelica.Icons.Example;

  parameter String epwPath = "modelica://Buildings/Resources/weatherdata/USA_IL_Chicago-OHare.Intl.AP.725300_TMY3.epw" "Path to epw file";
  parameter String weaPath = "modelica://Buildings/Resources/weatherdata/USA_IL_Chicago-OHare.Intl.AP.725300_TMY3.mos" "Path to weather file";
  parameter Modelica.Units.SI.Temperature TSetCoo[zoneCount] = {273.15+24} "Temperature set points for cooling";
  parameter Modelica.Units.SI.Temperature TSetHea[zoneCount] = {273.15+21} "Temperature set points for heating";
  parameter Modelica.Units.SI.Power QCoo_flow_nominal[zoneCount] = {10000} "Nominal cooling capacities (>0)";
  parameter Modelica.Units.SI.Power QHea_flow_nominal[zoneCount] = {10000} "Nominal heating capacities (>0)";

  // Building Definition

  inner Buildings.ThermalZones.EnergyPlus_24_2_0.Building building(
    idfName = Modelica.Utilities.Files.loadResource(idfPath),
    epwName = Modelica.Utilities.Files.loadResource(epwPath),
    weaName = Modelica.Utilities.Files.loadResource(weaPath),
    usePrecompiledFMU = false,
    computeWetBulbTemperature = false);

  package Medium = Buildings.Media.Air "Medium model";

  Buildings.ThermalZones.EnergyPlus_24_2_0.ThermalZone zon(redeclare package Medium = Medium, zoneName = "MyZone");

end Model2;
