from FMT import Parallel


def testParallelObjectsCanBeConstructed():
    constructors = {
        "FMTtask": Parallel.FMTtask,
        "FMTreplanningtask": Parallel.FMTreplanningtask,
        "FMTplanningtask": Parallel.FMTplanningtask,
        "FMTtaskhandler": Parallel.FMTtaskhandler,
    }
    for name, constructor in constructors.items():
        assert constructor() is not None
