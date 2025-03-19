# insert your copyright here

# see the URL below for information on how to write OpenStudio measures
# http://nrel.github.io/OpenStudio-user-documentation/reference/measure_writing_guide/

require 'erb'

# start the measure
class RubyReportingMeasureWithModelOutputRequests < OpenStudio::Measure::ReportingMeasure
  # human readable name
  def name
    # Measure name should be the title case of the class name.
    return 'Reporting Measure with Model Output Requests'
  end

  # human readable description
  def description
    return 'A new reporting measure that has a modelOutputRequests'
  end

  # human readable description of modeling approach
  def modeler_description
    return 'This was added at 3.10.0'
  end

  # define the arguments that the user will input
  def arguments(model = nil)
    args = OpenStudio::Measure::OSArgumentVector.new

    # bool argument to report report_drybulb_temp
    report_drybulb_temp = OpenStudio::Measure::OSArgument.makeBoolArgument('report_drybulb_temp', true)
    report_drybulb_temp.setDisplayName('Add output variables for Drybulb Temperature')
    report_drybulb_temp.setDescription('Will add drybulb temp and report min/mix value in html.')
    report_drybulb_temp.setDefaultValue(true)
    args << report_drybulb_temp

    add_output_json = OpenStudio::Measure::OSArgument.makeBoolArgument('add_output_json', true)
    add_output_json.setDisplayName('Request JSON output')
    add_output_json.setDescription('Will add Output:JSON with TimeSeriesAndTabular and set Output JSON to true')
    add_output_json.setDefaultValue(true)
    args << add_output_json

    return args
  end

  # define the outputs that the measure will create
  def outputs
    outs = OpenStudio::Measure::OSOutputVector.new

    # this measure does not produce machine readable outputs with registerValue, return an empty list

    return outs
  end

  # This method is called on all reporting measures immediately before the translation to E+ IDF
  # There is an implicit contract that this method should NOT be modifying your model in a way that would produce
  # different results, meaning it should only add or modify reporting-related elements
  # (eg: OutputTableSummaryReports, OutputControlFiles, etc)
  # If you mean to modify the model in a significant way, use a `ModelMeasure`
  # NOTE: this method will ONLY be called if you use the C++ CLI, not the `classic` (Ruby) one
  def modelOutputRequests(model, runner, user_arguments)
    if runner.getBoolArgumentValue('add_output_json', user_arguments)
      output_json = model.getOutputJSON
      output_json.setOptionType('TimeSeriesAndTabular')
      output_json.setOutputJSON(true)
      output_json.setOutputCBOR(false)
      output_json.setOutputMessagePack(false)
    end

    return true
  end

  # return a vector of IdfObject's to request EnergyPlus objects needed by the run method
  # Warning: Do not change the name of this method to be snake_case. The method must be lowerCamelCase.
  # This is done after ForwardTranslation to IDF, and there is a list of
  # accepted objects
  def energyPlusOutputRequests(runner, user_arguments)
    super(runner, user_arguments)  # Do **NOT** remove this line

    result = OpenStudio::IdfObjectVector.new

    # To use the built-in error checking we need the model...
    # get the last model and sql file
    model = runner.lastOpenStudioModel
    if model.empty?
      runner.registerError('Cannot find last model.')
      return result
    end
    model = model.get

    # use the built-in error checking
    if !runner.validateUserArguments(arguments(model), user_arguments)
      return result
    end

    # NOTE: this should rather be done in modelOutputRequests
    if runner.getBoolArgumentValue('report_drybulb_temp', user_arguments)
      request = OpenStudio::IdfObject.load('Output:Variable,,Site Outdoor Air Drybulb Temperature,Hourly;').get
      result << request
    end

    return result
  end

  # define what happens when the measure is run
  def run(runner, user_arguments)
    super(runner, user_arguments)

    # get the last model and sql file
    model = runner.lastOpenStudioModel
    if model.empty?
      runner.registerError('Cannot find last model.')
      return result
    end
    model = model.get

    # use the built-in error checking (need model)
    if !runner.validateUserArguments(arguments(model), user_arguments)
      return result
    end

    # get measure arguments
    report_drybulb_temp = runner.getBoolArgumentValue('report_drybulb_temp', user_arguments)

    # load sql file
    sql_file = runner.lastEnergyPlusSqlFile
    if sql_file.empty?
      runner.registerError('Cannot find last sql file.')
      return result
    end
    sql_file = sql_file.get
    model.setSqlFile(sql_file)
    # write file: any file named 'report*.*' in the current working directory
    # will be copied to the ./reports/ folder as 'reports/<measure_class_name>_<filename>.html'
    File.write("./report_ruby_with.txt", "OK")

    # close the sql file
    sql_file.close

    return true
  end
end

# register the measure to be used by the application
RubyReportingMeasureWithModelOutputRequests.new.registerWithApplication
