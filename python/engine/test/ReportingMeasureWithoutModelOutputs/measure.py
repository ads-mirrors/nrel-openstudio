"""insert your copyright here.

# see the URL below for information on how to write OpenStudio measures
# http://nrel.github.io/OpenStudio-user-documentation/reference/measure_writing_guide/
"""

from pathlib import Path

import openstudio


class ReportingMeasureWithoutModelOutputs(openstudio.measure.ReportingMeasure):
    """An ReportingMeasure."""

    def name(self):
        return "ReportingMeasureWithoutModelOutputs"

    def description(self):
        return "DESCRIPTION_TEXT"

    def modeler_description(self):
        return "MODELER_DESCRIPTION_TEXT"

    def arguments(self, model: openstudio.model.Model):
        args = openstudio.measure.OSArgumentVector()
        return args

    def outputs(self):
        outs = openstudio.measure.OSOutputVector()
        return outs

    def energyPlusOutputRequests(
        self, runner: openstudio.measure.OSRunner, user_arguments: openstudio.measure.OSArgumentMap
    ):
        result = openstudio.IdfObjectVector()
        return result

    def run(
        self,
        runner: openstudio.measure.OSRunner,
        user_arguments: openstudio.measure.OSArgumentMap,
    ):
        super().run(runner, user_arguments)
        return True
