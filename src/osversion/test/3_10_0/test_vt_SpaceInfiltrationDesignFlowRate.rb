#require '/usr/local/openstudio-3.9.0/Ruby/openstudio'

include OpenStudio::Model

m = Model.new

si = SpaceInfiltrationDesignFlowRate.new(m)
# Previous last field, field added at end
si.setVelocitySquaredTermCoefficient(0.2)

m.save('test_vt_SpaceInfiltrationDesignFlowRate.osm', true)
