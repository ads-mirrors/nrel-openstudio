########################################################################################################################
#  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
#  See also https://openstudio.net/license
########################################################################################################################

require 'openstudio'
require 'minitest/autorun'

class SwigJSONTest < Minitest::Test

  def test_swig_json_special_chars
    workflow = OpenStudio::WorkflowJSON.new()
    runner = OpenStudio::Measure::OSRunner.new(workflow)

    step1 = OpenStudio::MeasureStep.new("MeasureName1")
    step1.setArgument("Argument1", 100)

    workflow_step_result1 = OpenStudio::WorkflowStepResult.new()
    workflow_step_result1.setMeasureName("measure_name_1")

    name = "name_(|)'*$£*µ%é"
    val = "val_(|)'*$£*µ%é"
    stepValue1 = OpenStudio::WorkflowStepValue.new(name, val)
    assert_equal(name, stepValue1.name)
    assert_equal(val, stepValue1.valueAsString)
    assert_equal(val, stepValue1.valueAsJSON)
    workflow_step_result1.addStepValue(stepValue1)

    workflow_step_result1.setStepResult('Success'.to_StepResult)
    workflow_step_result1.setCompletedAt(OpenStudio::DateTime.nowUTC())
    step1.setResult(workflow_step_result1)

    workflow.setWorkflowSteps([step1])

    # workflow.saveAs("OSRunner_getPastStepValues_special_chars.osw")

    h = runner.getPastStepValuesForMeasure("MeasureName1")
    assert_equal(1, h.size)
    assert_equal(name.to_sym, h.keys.first)
    assert_equal(val, h.values.first)
  end

end
