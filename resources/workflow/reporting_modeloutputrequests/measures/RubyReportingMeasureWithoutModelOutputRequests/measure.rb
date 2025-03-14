# insert your copyright here

# see the URL below for information on how to write OpenStudio measures
# http://nrel.github.io/OpenStudio-user-documentation/reference/measure_writing_guide/

require 'erb'

# start the measure
class RubyReportingMeasureWithoutModelOutputRequests < OpenStudio::Measure::ReportingMeasure
  # human readable name
  def name
    # Measure name should be the title case of the class name.
    return 'Reporting Measure without Model Output Requests'
  end

  # human readable description
  def description
    return 'An older reporting measure that does not have modelOutputRequests'
  end

  # human readable description of modeling approach
  def modeler_description
    return 'This is a measure that was added before 3.10.0'
  end

  # define the arguments that the user will input
  def arguments(model = nil)
    args = OpenStudio::Measure::OSArgumentVector.new

    # bool argument to report report_drybulb_temp
    report_drybulb_temp = OpenStudio::Measure::OSArgument.makeBoolArgument('report_drybulb_temp', true)
    report_drybulb_temp.setDisplayName('Add output variables for Drybulb Temperature')
    report_drybulb_temp.setDescription('Will add drybulb temp and report min/mix value in html.')
    report_drybulb_temp.setValue(true)
    args << report_drybulb_temp

    return args
  end

  # define the outputs that the measure will create
  def outputs
    outs = OpenStudio::Measure::OSOutputVector.new

    # this measure does not produce machine readable outputs with registerValue, return an empty list

    return outs
  end

  # return a vector of IdfObject's to request EnergyPlus objects needed by the run method
  # Warning: Do not change the name of this method to be snake_case. The method must be lowerCamelCase.
  def energyPlusOutputRequests(runner, user_arguments)
    super(runner, user_arguments)  # Do **NOT** remove this line

    result = OpenStudio::IdfObjectVector.new

    # To use the built-in error checking we need the model...
    # get the last model and sql file
    model = runner.lastOpenStudioModel
    if model.empty?
      runner.registerError('Cannot find last model.')
      return false
    end
    model = model.get

    # use the built-in error checking (need model)
    if !runner.validateUserArguments(arguments(model), user_arguments)
      return false
    end

    # get measure arguments
    report_drybulb_temp = runner.getBoolArgumentValue('report_drybulb_temp', user_arguments)

    # load sql file
    sql_file = runner.lastEnergyPlusSqlFile
    if sql_file.empty?
      runner.registerError('Cannot find last sql file.')
      return false
    end
    sql_file = sql_file.get
    model.setSqlFile(sql_file)
    # write file: any file named 'report*.*' in the current working directory
    # will be copied to the ./reports/ folder as 'reports/<measure_class_name>_<filename>.html'
    File.write("./report_ruby_without.txt", "OK")

    # close the sql file
    sql_file.close

    return true
  end
end

# register the measure to be used by the application
RubyReportingMeasureWithoutModelOutputRequests.new.registerWithApplication
