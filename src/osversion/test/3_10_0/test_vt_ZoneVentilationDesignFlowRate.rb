#require '/usr/local/openstudio-3.9.0/Ruby/openstudio'

include OpenStudio::Model

m = Model.new

zv_dfr = ZoneVentilationDesignFlowRate.new(m)

m.save('test_vt_ZoneVentilationDesignFlowRate.osm', true)
