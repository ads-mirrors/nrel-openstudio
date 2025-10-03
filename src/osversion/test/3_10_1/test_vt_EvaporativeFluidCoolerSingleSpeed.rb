#require '/usr/local/openstudio-3.10.0/Ruby/openstudio'

include OpenStudio::Model

m = Model.new

efcss = EvaporativeFluidCoolerSingleSpeed.new(m)
# Field before insertion
efcss.setPerformanceInputMethod("StandardDesignCapacity")
# Field after insertion, also last field
efcss.setStandardDesignCapacity(123.0)

m.save('test_vt_EvaporativeFluidCoolerSingleSpeed.osm', true)
