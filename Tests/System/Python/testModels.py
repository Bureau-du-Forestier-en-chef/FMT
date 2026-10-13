import pytest
from FMT import Models, Parser
from conftest import requireFeature


def testUnconditionalModelExports():
    constructors = {
        "FMTmodel": Models.FMTmodel,
        "FMTsemodel": Models.FMTsemodel,
        "FMTsesmodel": Models.FMTsesmodel,
        "FMTsamodel": Models.FMTsamodel,
    }
    for name, constructor in constructors.items():
        assert constructor() is not None


def testModelParameterEnumsAreExposed():
    assert Models.FMTintmodelparameters.LENGTH is not None
    assert Models.FMTdblmodelparameters.TOLERANCE is not None
    assert Models.FMTboolmodelparameters.POSTSOLVE is not None


@pytest.mark.requiresOsi
def testOsiModelExports(modelPath):
    requireFeature("OSI")
    assert hasattr(Models, "FMTsolverinterface")
    assert hasattr(Models, "FMTlpmodel")
    parser = Parser.FMTmodelparser()
    parser.setquietlogger()
    models = parser.readproject(str(modelPath), ["LP"])
    model = Models.FMTlpmodel(models[0], Models.FMTsolverinterface.CLP)
    assert model is not None
    assert len(model.getoutputs()) > 0


@pytest.mark.requiresMosek
def testMosekEnumMatchesCompiledFeature():
    requireFeature("MOSEK")
    assert hasattr(Models.FMTsolverinterface, "MOSEK")
