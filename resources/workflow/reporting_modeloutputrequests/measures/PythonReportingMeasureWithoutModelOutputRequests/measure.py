"""insert your copyright here.

# see the URL below for information on how to write OpenStudio measures
# http://nrel.github.io/OpenStudio-user-documentation/reference/measure_writing_guide/
"""

from pathlib import Path

import openstudio


class PythonReportingMeasureWithoutModelOutputRequests(openstudio.measure.ReportingMeasure):
    """An ReportingMeasure."""

    def name(self):
        """Returns the human readable name.

        Measure name should be the title case of the class name.
        The measure name is the first contact a user has with the measure;
        it is also shared throughout the measure workflow, visible in the OpenStudio Application,
        PAT, Server Management Consoles, and in output reports.
        As such, measure names should clearly describe the measure's function,
        while remaining general in nature
        """
        return "Reporting Measure without Model Output Requests"

    def description(self):
        """Human readable description.

        The measure description is intended for a general audience and should not assume
        that the reader is familiar with the design and construction practices suggested by the measure.
        """
        return "An older reporting measure that does not have modelOutputRequests"

    def modeler_description(self):
        """Human readable description of modeling approach.

        The modeler description is intended for the energy modeler using the measure.
        It should explain the measure's intent, and include any requirements about
        how the baseline model must be set up, major assumptions made by the measure,
        and relevant citations or references to applicable modeling resources
        """
        return "This is a measure that was added before 3.10.0"

    def arguments(self, model: openstudio.model.Model):
        """Prepares user arguments for the measure.

        Measure arguments define which -- if any -- input parameters the user may set before running the measure.
        """
        args = openstudio.measure.OSArgumentVector()

        report_drybulb_temp = openstudio.measure.OSArgument.makeBoolArgument("report_drybulb_temp", True)
        report_drybulb_temp.setDisplayName("Add output variables for Drybulb Temperature")
        report_drybulb_temp.setDescription("Will add drybulb temp and report min/max values in html.")
        report_drybulb_temp.setValue(True)
        args.append(report_drybulb_temp)

        return args

    def outputs(self):
        """Define the outputs that the measure will create."""
        outs = openstudio.measure.OSOutputVector()

        # this measure does not produce machine readable outputs with registerValue, return an empty list

        return outs

    def energyPlusOutputRequests(
        self, runner: openstudio.measure.OSRunner, user_arguments: openstudio.measure.OSArgumentMap
    ):
        """Returns a vector of IdfObject's to request EnergyPlus objects needed by the run method."""
        super().energyPlusOutputRequests(runner, user_arguments)  # Do **NOT** remove this line

        result = openstudio.IdfObjectVector()

        # To use the built-in error checking we need the model...
        # get the last model and sql file
        model = runner.lastOpenStudioModel()
        if not model.is_initialized():
            runner.registerError("Cannot find last model.")
            return result

        model = model.get()

        # use the built-in error checking
        if not runner.validateUserArguments(self.arguments(model), user_arguments):
            return result

        if runner.getBoolArgumentValue("report_drybulb_temp", user_arguments):
            request = openstudio.IdfObject.load(
                "Output:Variable, , Site Outdoor Air Drybulb Temperature, Hourly;"
            ).get()
            result.append(request)

        return result

    def run(
        self,
        runner: openstudio.measure.OSRunner,
        user_arguments: openstudio.measure.OSArgumentMap,
    ):
        """Defines what happens when the measure is run."""
        super().run(runner, user_arguments)

        # get the last model and sql file
        model = runner.lastOpenStudioModel()
        if not model.is_initialized():
            runner.registerError("Cannot find last model.")
            return False
        model = model.get()

        # use the built-in error checking (need model)
        if not runner.validateUserArguments(self.arguments(model), user_arguments):
            return False

        # load sql file
        sql_file = runner.lastEnergyPlusSqlFile()
        if not sql_file.is_initialized():
            runner.registerError("Cannot find last sql file.")
            return False

        sql_file = sql_file.get()
        model.setSqlFile(sql_file)
        # write file: any file named 'report*.*' in the current working directory
        # will be copied to the ./reports/ folder as 'reports/<measure_class_name>_<filename>.html'
        Path("report_python_without.txt").write_text("OK")

        # Close the sql file
        sql_file.close()

        return True


# register the measure to be used by the application
PythonReportingMeasureWithoutModelOutputRequests().registerWithApplication()
