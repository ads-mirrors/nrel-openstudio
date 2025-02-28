within Template_IdealHeat;
model HVAC
  parameter Modelica.Units.SI.Temperature TSetCoo = 273.15+25 "Temperature set point  for cooling";
  parameter Modelica.Units.SI.Temperature TSetHea = 273.15+20 "Temperature set point  for heating";
  parameter Modelica.Units.SI.Power QCoo_flow_nominal "Nominal cooling capacity (>0)";
  parameter Modelica.Units.SI.Power QHea_flow_nominal "Nominal heating capacity (>0)";
  Buildings.Controls.OBC.CDL.Reals.Sources.Constant TSetCooCon(k=TSetCoo) "Set point temperature for cooling"
    annotation (Placement(transformation(extent={{-60,30},{-40,50}})));
  Buildings.ThermalZones.EnergyPlus_9_6_0.Examples.SmallOffice.BaseClasses.IdealHeaterCooler
      coo(Q_flow_nominal=-QCoo_flow_nominal)
    "Ideal cooling device for sensible cooling"
    annotation (Placement(transformation(rotation=0,extent={{-20,30},{0,50}})));
  Buildings.Controls.OBC.CDL.Reals.Sources.Constant TSetHeaCon(k=TSetHea) "Set point temperature for heating"
    annotation (Placement(transformation(extent={{-60,-50},{-40,-30}})));
  Buildings.ThermalZones.EnergyPlus_9_6_0.Examples.SmallOffice.BaseClasses.IdealHeaterCooler
      hea(Q_flow_nominal=QHea_flow_nominal)
    "Ideal heating device for sensible heating" annotation (Placement(
        transformation(rotation=0, extent={{-20,-50},{0,-30}})));
  Modelica.Blocks.Interfaces.RealInput TMea "Measured temperature"
    annotation (Placement(transformation(extent={{-140,-20},{-100,20}})));
  Modelica.Thermal.HeatTransfer.Interfaces.HeatPort_b heaPor "Heat port"
    annotation (Placement(transformation(extent={{90,-10},{110,10}})));
  Buildings.Controls.OBC.CDL.Interfaces.RealOutput QCoo_flow
    "Heat flow rate for cooling"
    annotation (Placement(transformation(extent={{100,60},{140,100}})));
  Buildings.Controls.OBC.CDL.Interfaces.RealOutput QHea_flow
    "Heat flow rate for heating"
    annotation (Placement(transformation(extent={{100,-100},{140,-60}})));
equation
  connect(TSetCooCon.y, coo.TSet)
    annotation (Line(points={{-38,40},{-22,40}}, color={0,0,127}));
  connect(TSetHeaCon.y, hea.TSet)
    annotation (Line(points={{-38,-40},{-22,-40}}, color={0,0,127}));
  connect(TMea, coo.TMea)
    annotation (Line(points={{-120,0},{-10,0},{-10,28}}, color={0,0,127}));
  connect(TMea, hea.TMea) annotation (Line(points={{-120,0},{-80,0},{-80,-80},{-10,
          -80},{-10,-52}}, color={0,0,127}));
  connect(coo.heaPor, heaPor)
    annotation (Line(points={{0,40},{40,40},{40,0},{100,0}}, color={191,0,0}));
  connect(hea.heaPor, heaPor) annotation (Line(points={{0,-40},{40,-40},{40,0},{
          100,0}}, color={191,0,0}));
  connect(coo.Q_flow, QCoo_flow)
    annotation (Line(points={{2,46},{2,80},{120,80}}, color={0,0,127}));
  connect(hea.Q_flow, QHea_flow) annotation (Line(points={{2,-34},{20,-34},{20,
          -80},{120,-80}}, color={0,0,127}));
  annotation (Icon(coordinateSystem(preserveAspectRatio=false), graphics={
        Text(
          extent={{-150,152},{150,112}},
          textString="%name",
          textColor={0,0,255}),
        Rectangle(
          extent={{-100,0},{100,-100}},
          lineColor={0,0,0},
          fillColor={238,46,47},
          fillPattern=FillPattern.Solid),
        Rectangle(
          extent={{-100,100},{100,0}},
          lineColor={0,0,0},
          fillColor={28,108,200},
          fillPattern=FillPattern.Solid)}),                      Diagram(
        coordinateSystem(preserveAspectRatio=false)));
end HVAC;
