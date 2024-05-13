from pathlib import Path

import openstudio


def test_swig_json_special_chars():
    workflow = openstudio.WorkflowJSON()
    runner = openstudio.measure.OSRunner(workflow)

    step1 = openstudio.MeasureStep("MeasureName1")
    step1.setArgument("Argument1", 100)

    workflow_step_result1 = openstudio.WorkflowStepResult()
    workflow_step_result1.setMeasureName("measure_name_1")

    name = "name_(|)'*$£*µ%é"
    val = "val_(|)'*$£*µ%é"
    stepValue1 = openstudio.WorkflowStepValue(name, val)
    assert name == stepValue1.name()
    assert val == stepValue1.valueAsString()
    assert val == stepValue1.valueAsJSON()

    workflow_step_result1.addStepValue(stepValue1)

    workflow_step_result1.setStepResult(openstudio.StepResult('Success'))
    workflow_step_result1.setCompletedAt(openstudio.DateTime.nowUTC())
    step1.setResult(workflow_step_result1)

    workflow.setWorkflowSteps([step1])

    d = runner.getPastStepValuesForMeasure("MeasureName1")
    assert len(d) == 1
    k, v = next(iter(d.items()))
    assert k == name
    assert v == val
