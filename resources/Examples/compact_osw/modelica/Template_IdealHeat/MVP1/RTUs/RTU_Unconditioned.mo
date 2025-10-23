within Template_IdealHeat.MVP1.RTUs;
model RTU_Unconditioned
  "An RTU model that provides no heating and cooling"
  extends Template_IdealHeat.MVP1.RTUs.Interfaces.RTU(
    TSetCoo=273.15+25,
    TSetHea=273.15+20,
    QCoo_flow_nominal=0,
    QHea_flow_nominal=0);
  Modelica.Thermal.HeatTransfer.Sources.FixedHeatFlow qZero(Q_flow=0)
    "Zero heating and cooling"
    annotation (Placement(transformation(extent={{0,-10},{20,10}})));
equation
  connect(qZero.port, heaPor)
    annotation (Line(points={{20,0},{200,0}}, color={191,0,0}));
end RTU_Unconditioned;
