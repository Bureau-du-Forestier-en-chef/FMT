import math
import pytest
from FMT import Models, Parser
from conftest import requireFeature


@pytest.mark.requiresOsi
def testClpPlanningWorkflow(modelPath):
    requireFeature("OSI")
    parser = Parser.FMTmodelparser()
    parser.setquietlogger()
    models = parser.readproject(str(modelPath), ["LP"])
    assert len(models) == 1

    model = Models.FMTlpmodel(models[0], Models.FMTsolverinterface.CLP)
    assert model.setparameter(Models.FMTintmodelparameters.LENGTH, 2)
    solved = model.doplanning(True)
    assert solved

    objective = model.getobjectivevalue()
    assert math.isfinite(objective)
    outputs = model.getoutputs()
    assert len(outputs) > 0
