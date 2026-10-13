from FMT import Core


def testCoreEnumsAreExposed():
    assert Core.FMTsection.Control is not None
    assert Core.FMToutputlevel.totalonly is not None
    assert Core.FMTconstrainttype.FMTstandard is not None


def testCoreDefaultObjectsCanBeConstructed():
    constructors = {
        "FMTmask": Core.FMTmask,
        "FMTdevelopment": Core.FMTdevelopment,
        "FMTagebounds": Core.FMTagebounds,
        "FMTperbounds": Core.FMTperbounds,
        "FMTyldbounds": Core.FMTyldbounds,
        "FMTlockbounds": Core.FMTlockbounds,
        "FMTspec": Core.FMTspec,
        "FMTactualdevelopment": Core.FMTactualdevelopment,
        "FMTaction": Core.FMTaction,
        "FMTlifespans": Core.FMTlifespans,
        "FMTfork": Core.FMTfork,
        "FMTyieldhandler": Core.FMTyieldhandler,
        "FMTyields": Core.FMTyields,
        "FMTtransition": Core.FMTtransition,
        "FMToperator": Core.FMToperator,
        "FMToutput": Core.FMToutput,
        "FMTtheme": Core.FMTtheme,
        "FMTschedule": Core.FMTschedule,
        "FMTconstraint": Core.FMTconstraint,
    }
    for name, constructor in constructors.items():
        assert constructor() is not None


def testSchedulePeriodRoundTrip():
    schedule = Core.FMTschedule()
    schedule.setperiod(3)
    assert schedule.getperiod() == 3
