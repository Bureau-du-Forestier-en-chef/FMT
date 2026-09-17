from FMT import Spatial


def testCoordinateRoundTrip():
    coordinate = Spatial.FMTcoordinate(4, 7)
    assert coordinate.getx() == 4
    assert coordinate.gety() == 7


def testSpatialObjectsCanBeConstructed():
    for name, constructor in {
        "FMTforest": Spatial.FMTforest,
        "FMTeventcontainer": Spatial.FMTeventcontainer,
        "FMTSpatialSchedule": Spatial.FMTSpatialSchedule,
    }.items():
        assert constructor() is not None
