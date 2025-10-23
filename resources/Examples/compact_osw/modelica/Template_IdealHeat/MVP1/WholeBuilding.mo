within Template_IdealHeat.MVP1;
model WholeBuilding
  "A model to instantiate templates to represent a whole building"
  // This is the model the OS measure needs to create/modify

  // General parameters
  parameter String idfPath = "modelica://Buildings/Resources/Data/ThermalZones/EnergyPlus_24_2_0/Examples/SingleFamilyHouse_TwoSpeed_ZoneAirBalance/SingleFamilyHouse_TwoSpeed_ZoneAirBalance.idf" "Path to idf file";
  parameter String epwPath = "modelica://Buildings/Resources/weatherdata/USA_IL_Chicago-OHare.Intl.AP.725300_TMY3.epw" "Path to epw file";
  parameter String weaPath = "modelica://Buildings/Resources/weatherdata/USA_IL_Chicago-OHare.Intl.AP.725300_TMY3.mos" "Path to weather file";
  inner Buildings.ThermalZones.EnergyPlus_24_2_0.Building building(
    idfName=Modelica.Utilities.Files.loadResource(idfPath),
    epwName=Modelica.Utilities.Files.loadResource(epwPath),
    weaName=Modelica.Utilities.Files.loadResource(weaPath),
    usePrecompiledFMU=false,
    computeWetBulbTemperature=false) "Building model" annotation(
    Placement(transformation(extent={{-100,80},{-80,100}})));

  // Instantiate zone-hvac templates for each zone in model and specify RTU model
  TemplatesZoneHVAC.SingleZoneRTU LIVING_ZONE(
    redeclare Template_IdealHeat.MVP1.TemplatesRTUs.RTU_Ideal rtu(
      TSetHea=273.15 + 20,
      TSetCoo=273.15 + 25,
      QHea_flow_nominal=10000,
      QCoo_flow_nominal=10000),
    zon(zoneName="LIVING ZONE"))
    annotation (Placement(transformation(extent={{-10,20},{10,40}})));
  TemplatesZoneHVAC.SingleZoneRTU GARAGE_ZONE(
    redeclare Template_IdealHeat.MVP1.TemplatesRTUs.RTU_Unconditioned rtu,
    zon(zoneName="GARAGE ZONE"))
    annotation (Placement(transformation(extent={{-10,-10},{10,10}})));
  TemplatesZoneHVAC.SingleZoneRTU ATTIC_ZONE(
    redeclare Template_IdealHeat.MVP1.TemplatesRTUs.RTU_Unconditioned rtu,
    zon(zoneName="ATTIC ZONE"))
    annotation (Placement(transformation(extent={{-10,-40},{10,-20}})));
  annotation (Icon(coordinateSystem(preserveAspectRatio=false), graphics={
        Rectangle(
          extent={{100,-100},{-100,100}},
          lineColor={0,0,0},
          fillColor={255,255,255},
          fillPattern=FillPattern.Solid),
        Polygon(
          points={{0,80},{-78,38},{80,38},{0,80}},
          lineColor={95,95,95},
          smooth=Smooth.None,
          fillPattern=FillPattern.Solid,
          fillColor={95,95,95}),
        Rectangle(
          extent={{-64,38},{64,-64}},
          lineColor={150,150,150},
          fillPattern=FillPattern.Solid,
          fillColor={150,150,150}),
        Rectangle(
          extent={{-42,-4},{-14,24}},
          lineColor={255,255,255},
          fillColor={255,255,255},
          fillPattern=FillPattern.Solid),
        Rectangle(
          extent={{-42,-48},{-14,-20}},
          lineColor={255,255,255},
          fillColor={255,255,255},
          fillPattern=FillPattern.Solid),
        Rectangle(
          extent={{16,-48},{44,-20}},
          lineColor={255,255,255},
          fillColor={255,255,255},
          fillPattern=FillPattern.Solid),
        Rectangle(
          extent={{16,-4},{44,24}},
          lineColor={255,255,255},
          fillColor={255,255,255},
          fillPattern=FillPattern.Solid)}),                      Diagram(
        coordinateSystem(preserveAspectRatio=false)),
    experiment(
      StopTime=864000,
      Interval=60,
      Tolerance=1e-06,
      __Dymola_Algorithm="Cvode"),
    __Dymola_experimentFlags(
      Advanced(
        EvaluateAlsoTop=false,
        GenerateAnalyticJacobian=false,
        OutputModelicaCode=false),
      Evaluate=true,
      OutputCPUtime=true,
      OutputFlatModelica=false));
end WholeBuilding;
