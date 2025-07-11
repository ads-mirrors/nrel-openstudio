#require '/usr/local/openstudio-3.10.0/Ruby/openstudio'

include OpenStudio::Model

m = Model.new

people_def = PeopleDefinition.new(m)
people = People.new(people_def)
s = OpenStudio::Model::ScheduleConstant.new(m)
people.setClothingInsulationSchedule(s)

m.save('test_vt_People.osm', true)
