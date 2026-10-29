# Copyright (c) 2019 Gouvernement du Québec
#
# SPDX-License-Identifier: LiLiQ-R-1.1
# License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt

"""The pickle of Python, through FMT.pyd: pickle.dumps then pickle.loads of an object of each class that declares a
pickle suite (def_pickle in Include/PYexport*.h, Python::FMT_pickle_suite of Include/FMTpythonpickle.h), and
copy.deepcopy, which goes through the same suite. The C++ part of the suite, the archive, is tested class by class in
Tests/Integration/Serialization.

A case that reveals a defect of Tests/ETAT.md (section 2.7) is marked xfail, strict, with the exception that the
defect raises: it must fail for that reason, and turns red on the day the defect is fixed. pytest --runxfail runs
them as ordinary cases.
"""

import copy
import pickle

import pytest
from FMT import Core, Heuristics, Models, Parser
from conftest import hasFeature, twdLandPrimary

# Defect 29: FMT_pickle_suite::getstate hands the archive, compressed by zlib, to Python as a std::string, which
# Boost.Python converts to a str by decoding it as UTF-8; the header of zlib, 78 9C, is not UTF-8.
DEFECT_29 = pytest.mark.xfail(strict=True, raises=UnicodeDecodeError,
                              reason="defect 29: the state of a pickle is converted to a str, not to bytes")
# Defect 26: the yields are saved through a pointer to FMTYieldHandler, whose derived classes are not exported to
# Boost.Serialization: saving throws "unregistered class" before defect 29.
DEFECT_26 = pytest.mark.xfail(strict=True, raises=RuntimeError,
                              reason="defect 26: the yield handlers are not exported to Boost.Serialization")
# Defect 30: FMToutput declares the pickle suite of FMTTransition, whose functions do not take an FMToutput.
DEFECT_30 = pytest.mark.xfail(strict=True, raises=TypeError,
                              reason="defect 30: FMToutput declares the pickle suite of FMTTransition")


def readRoot() -> Models.FMTmodel:
    parser = Parser.FMTmodelparser()
    parser.setquietlogger()
    return parser.readproject(str(twdLandPrimary()), ["ROOT"])[0]


def getLpModel(model: Models.FMTmodel) -> Models.FMTlpmodel:
    if not hasFeature("OSI"):
        pytest.skip("FMT was compiled without OSI support")
    return Models.FMTlpmodel(model, Models.FMTsolverinterface.CLP)


def describeModel(model) -> tuple:
    return (model.getname(), len(model.getthemes()), len(model.getactions()), len(model.gettransitions()),
            len(model.getoutputs()), len(model.getconstraints()), len(model.getarea()))


# Each case: the class, the object built from the root of TWD_land, and what the unpickled object must keep.
CASES = [
    pytest.param(lambda model: model.getarea()[0].getmask(), lambda value: value, id="FMTmask", marks=DEFECT_29),
    pytest.param(lambda model: Core.FMTdevelopment(model.getarea()[0]), lambda value: value, id="FMTdevelopment",
                 marks=DEFECT_29),
    pytest.param(lambda model: model.getactions()[0], lambda value: value, id="FMTaction", marks=DEFECT_29),
    pytest.param(lambda model: Core.FMTageyieldhandler(model.getarea()[0].getmask()), str, id="FMTageyieldhandler",
                 marks=DEFECT_29),
    pytest.param(lambda model: Core.FMTtimeyieldhandler(model.getarea()[0].getmask()), str, id="FMTtimeyieldhandler",
                 marks=DEFECT_29),
    pytest.param(lambda model: model.getyields(), lambda value: sorted(value.getallyieldnames()), id="FMTyields",
                 marks=DEFECT_26),
    pytest.param(lambda model: model.gettransitions()[0], lambda value: value, id="FMTtransition", marks=DEFECT_29),
    pytest.param(lambda model: model.getoutputs()[0], lambda value: value, id="FMToutput", marks=DEFECT_30),
    pytest.param(lambda model: model.getthemes()[0], lambda value: value, id="FMTtheme", marks=DEFECT_29),
    pytest.param(lambda model: Core.FMTschedule(), lambda value: value, id="FMTschedule", marks=DEFECT_29),
    pytest.param(lambda model: model, describeModel, id="FMTmodel", marks=DEFECT_26),
    pytest.param(lambda model: Models.FMTsemodel(model), describeModel, id="FMTsemodel", marks=DEFECT_26),
    pytest.param(lambda model: Models.FMTsesmodel(model), describeModel, id="FMTsesmodel", marks=DEFECT_26),
    pytest.param(lambda model: Models.FMTnssmodel(model, 0), describeModel, id="FMTnssmodel", marks=DEFECT_26),
    pytest.param(getLpModel, describeModel, id="FMTlpmodel", marks=DEFECT_26),
    pytest.param(lambda model: Heuristics.FMToperatingarea(model.getarea()[0].getmask(), 12.5),
                 lambda value: (str(value.getmask()), value.getneihgborsperimeter()), id="FMToperatingarea",
                 marks=DEFECT_29),
]


@pytest.mark.parametrize("build, keep", CASES)
def testGivenAnObjectOfAClassWithAPickleSuiteWhenPickledThenUnpicklesTheSameObject(build, keep):
    # Arrange
    value = build(readRoot())

    # Act
    unpickled = pickle.loads(pickle.dumps(value))

    # Assert
    assert type(unpickled) is type(value)
    assert keep(unpickled) == keep(value)


@DEFECT_29
def testGivenAMaskWhenDeepCopiedThenCopiesTheSameMask():
    # Arrange
    mask = readRoot().getarea()[0].getmask()

    # Act
    copied = copy.deepcopy(mask)

    # Assert
    assert copied == mask
    assert str(copied) == str(mask)
