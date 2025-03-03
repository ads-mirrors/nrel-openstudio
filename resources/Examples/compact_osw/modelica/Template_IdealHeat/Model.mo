within Template_IdealHeat;
model Model
  "Simple template model for testing OpenStudio-Spawn measures"
  extends Modelica.Icons.Example;

  // Zone and HVAC Parameters

  parameter Integer n = 1 "Number of zones";
  parameter String zoneNames[n] = {"LIVING ZONE"} "Zone names in idf";
  parameter String idfPath = "modelica://Buildings/Resources/Data/ThermalZones/EnergyPlus_24_2_0/Examples/SingleFamilyHouse_TwoSpeed_ZoneAirBalance/SingleFamilyHouse_TwoSpeed_ZoneAirBalance.idf" "Path to idf file";
  parameter String epwPath = "modelica://Buildings/Resources/weatherdata/USA_IL_Chicago-OHare.Intl.AP.725300_TMY3.epw" "Path to epw file";
  parameter String weaPath = "modelica://Buildings/Resources/weatherdata/USA_IL_Chicago-OHare.Intl.AP.725300_TMY3.mos" "Path to weather file";
  parameter Modelica.Units.SI.Temperature TSetCoo[n] = {273.15+24} "Temperature set points for cooling";
  parameter Modelica.Units.SI.Temperature TSetHea[n] = {273.15+21} "Temperature set points for heating";
  parameter Modelica.Units.SI.Power QCoo_flow_nominal[n] = {10000} "Nominal cooling capacities (>0)";
  parameter Modelica.Units.SI.Power QHea_flow_nominal[n] = {10000} "Nominal heating capacities (>0)";

  // Building Definition

  inner Buildings.ThermalZones.EnergyPlus_24_2_0.Building building(
    idfName = Modelica.Utilities.Files.loadResource(idfPath),
    epwName = Modelica.Utilities.Files.loadResource(epwPath),
    weaName = Modelica.Utilities.Files.loadResource(weaPath),
    usePrecompiledFMU = false,
    computeWetBulbTemperature = false) "Building model" annotation(
    Placement(transformation(extent = {{-80, -90}, {-60, -70}})));

  // Zone Definition

  package Medium = Buildings.Media.Air "Medium model";
  Buildings.ThermalZones.EnergyPlus_24_2_0.ThermalZone zon[n](redeclare package Medium = Medium, zoneName = zoneNames) "Thermal zones" annotation(
    Placement(transformation(extent = {{0, -20}, {40, 20}})));
  Modelica.Blocks.Sources.Constant qIntGai[n,3](each k = 0) "Internal heat gains, set to zero because these are modeled in EnergyPlus" annotation(
    Placement(transformation(extent = {{-40, 0}, {-20, 20}})));

  // HVAC Definition
  // KSB: Can the `HVAC` model be contained within a Measure?
  // KSB: Can / should it be replaceable?

  HVAC hvac[n](
    TSetCoo=TSetCoo,
    TSetHea=TSetHea,
    QCoo_flow_nominal=QCoo_flow_nominal,
    QHea_flow_nominal=QHea_flow_nominal) "HVAC system"
    annotation (Placement(transformation(extent={{0,50},{20,70}})));

equation
  connect(qIntGai.y, zon.qGai_flow)
    annotation (Line(points={{-19,10},{-2,10}}, color={0,0,127}));
  connect(hvac.heaPor, zon.heaPorAir)
    annotation (Line(points={{20,60},{30,60},{30,0},{20,0}}, color={191,0,0}));
  connect(zon.TAir, hvac.TMea) annotation (Line(points={{41,18},{60,18},{60,80},
          {-20,80},{-20,60},{-2,60}}, color={0,0,127}));
  annotation(
    Documentation(info = "<html>
<p>
This example model uses the parameter n to instantiate 
n thermal zones modeled with Spawn connected to n ideal HVAC systems,
with one HVAC system serving each zone.
The orders of elements in parameter vectors defining zone names and 
HVAC parameters must align.
</p>
</html>", revisions = "<html>
<ul>
<li>
February 10, 2025, by David Blum:<br/>
First implementation.
</li>
</ul>
</html>"),
    experiment(
      StopTime=432000,
      Interval=300,
      Tolerance=1e-06,
      __Dymola_Algorithm="Dassl"));
end Model;
