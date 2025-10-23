within Template_IdealHeat.MVP1.TemplatesZoneRTU;
model SingleZoneRTU "A template model for single zone RTU"
  Buildings.ThermalZones.EnergyPlus_24_2_0.ThermalZone zon(redeclare package
      Medium = Buildings.Media.Air) "Zone model"
    annotation (Placement(transformation(extent={{40,-20},{80,20}})));
  replaceable TemplatesRTUs.RTU_Unconditioned rtu constrainedby
    Template_IdealHeat.MVP1.TemplatesRTUs.Interfaces.RTU
    "Single zone RTU model"
    annotation (Placement(transformation(extent={{-60,-20},{-20,20}})));
  Modelica.Blocks.Sources.Constant qIntGai[3](each k=0)
    "Internal heat gains, set to zero because these are modeled in EnergyPlus"
    annotation (Placement(transformation(extent={{0,10},{20,30}})));
equation
  connect(qIntGai.y, zon.qGai_flow) annotation (Line(points={{21,20},{30,20},{30,
          10},{38,10}}, color={0,0,127}));
  connect(zon.TAir, rtu.TMea) annotation (Line(points={{81,18},{90,18},{90,40},{
          -80,40},{-80,0},{-62,0}}, color={0,0,127}));
  connect(rtu.heaPor, zon.heaPorAir)
    annotation (Line(points={{-20,0},{60,0}}, color={191,0,0}));
  annotation (Icon(coordinateSystem(preserveAspectRatio=false), graphics={
          Rectangle(
          extent={{-100,100},{100,-100}},
          lineColor={28,108,200},
          fillColor={255,255,255},
          fillPattern=FillPattern.Solid),
        Text(
          extent={{-150,140},{150,100}},
          textString="%name",
          textColor={0,0,255})}),                                Diagram(
        coordinateSystem(preserveAspectRatio=false)));
end SingleZoneRTU;
