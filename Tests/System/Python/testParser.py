from FMT import Parser


def testParserClassesAreExposedAndConstructible():
    constructors = {
        "FMTparser": Parser.FMTparser,
        "FMTareaparser": Parser.FMTareaparser,
        "FMTlandscapeparser": Parser.FMTlandscapeparser,
        "FMTactionparser": Parser.FMTactionparser,
        "FMTtransitionparser": Parser.FMTtransitionparser,
        "FMTconstantparser": Parser.FMTconstantparser,
        "FMTlifespanparser": Parser.FMTlifespanparser,
        "FMTyieldparser": Parser.FMTyieldparser,
        "FMToutputparser": Parser.FMToutputparser,
        "FMToptimizationparser": Parser.FMToptimizationparser,
        "FMTmodelparser": Parser.FMTmodelparser,
        "FMTscheduleparser": Parser.FMTscheduleparser,
    }
    for name, constructor in constructors.items():
        assert constructor() is not None


def testTwdLandProjectLoads(modelPath):
    parser = Parser.FMTmodelparser()
    parser.setquietlogger()
    models = parser.readproject(str(modelPath), ["LP"])
    assert len(models) == 1
    assert models[0].getname() is not None
