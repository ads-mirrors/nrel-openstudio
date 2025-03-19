import pytest

from workflow_helpers import run_workflow

@pytest.mark.parametrize(
    "language, is_labs",
    [
        ["ruby", False],
        ["ruby", True],
        # ["python", False], // Not possible
        ["python", True],
    ],
)

def test_reportingmeasure_model_output_requests(osclipath, language: str, is_labs: bool):
    suffix = "labs" if is_labs else "classic"
    base_osw_name = f"{language}.osw"

    runDir, r = run_workflow(
        osclipath=osclipath,
        base_osw_name=base_osw_name,
        suffix=suffix,
        is_labs=is_labs,
        verbose=False,
        debug=True,
        post_process_only=False,
    )
    r.check_returncode()
    assert r.returncode == 0
