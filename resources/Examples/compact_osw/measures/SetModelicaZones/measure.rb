
# insert your copyright here

# see the URL below for information on how to write OpenStudio measures
# http://nrel.github.io/OpenStudio-user-documentation/reference/measure_writing_guide/

# start the measure
class SetModelicaZones < OpenStudio::Measure::ModelicaMeasure
  # human readable name
  def name
    # Measure name should be the title case of the class name.
    return 'SetModelicaZones'
  end

  # human readable description
  def description
    return 'DESCRIPTION_TEXT'
  end

  # human readable description of modeling approach
  def modeler_description
    return 'MODELER_DESCRIPTION_TEXT'
  end

  # define the arguments that the user will input
  def arguments(model, workspace)
    args = OpenStudio::Measure::OSArgumentVector.new
    return args
  end

  # define what happens when the measure is run
  def run(model, workspace, runner, user_arguments)
    super(model, workspace, runner, user_arguments)  # Do **NOT** remove this line

    # use the built-in error checking
    if !runner.validateUserArguments(arguments(model, workspace), user_arguments)
      return false
    end

    workflow = runner.workflow()
    seed_modelica_model = workflow.seedModelicaModel().get()
    params = runner.modelicaParameters()

    all_zones = model.getThermalZones()
    # In this example, only include zones that have people in them
    conditioned_zones = all_zones.select { |zone| zone.numberOfPeople > 0.0 }
    # quote the zone names and separate with commas
    zone_names = conditioned_zones.map { |z| "\"#{z.nameString()}\"" }.join(", ")
    # The final parameter value is wrapped in curly braces
    zones_param = "{#{zone_names}}"
    params.setParameterValue(seed_modelica_model, "zoneNames", zones_param)
    params.setParameterValue(seed_modelica_model, "zoneCount", conditioned_zones.count().to_s)

    return true
  end
end

# register the measure to be used by the application
SetModelicaZones.new.registerWithApplication
