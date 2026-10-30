/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
    #include "PickleArchive.h"
    #include "ParserRoundTrip.h"
    #include "FMTLpModel.h"
    #include "FMTModel.h"
    #include "FMTOutput.h"
    #include "FMTsolverinterface.h"
    #include "FMTutility.h"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <string>

// Models::FMTLpModel through the archive of the pickle of Python, as Python pickles an FMTlpmodel, with its real
// solver: MOSEK when FMT is built with it, CLP otherwise. A model saves its sections, its solver, which keeps its matrix
// and its solution (Tests/Integration/Serialization/FMTLpSolver.cpp), and its graph, which a test reaches only through
// its model (Documentation/Architecture.md, Known Architectural Debt). The scenario LP of TWD_land is
// built and solved over five periods; its optimum and its outputs are compared to the tolerance of PlanScenario, 1 %
// of the value, or of 1 below 1. Each case fails on several defects of Tests/ETAT.md (section 2.7), named above it in
// the order in which the case meets them.

#ifdef FMTWITHOSI
namespace
{
    const int LENGTH = 5;

    Models::FMTLpModel getModel(const std::string& p_scenario)
    {
        #ifdef FMTWITHMOSEK
            Models::FMTLpModel model(Testing::readScenario(p_scenario), Models::FMTSolverInterface::MOSEK);
        #else
            Models::FMTLpModel model(Testing::readScenario(p_scenario), Models::FMTSolverInterface::CLP);
        #endif
        model.setParameter(Models::FMTintmodelparameters::LENGTH, LENGTH);
        return model;
    }

    // The scenario LP built and solved without presolve, so that the matrix holds the variables of the model.
    Models::FMTLpModel getSolvedModel()
    {
        Models::FMTLpModel model = getModel("LP");
        model.build();
        model.solve();
        return model;
    }

    bool isNear(double p_actual, double p_expected)
    {
        if (!std::isfinite(p_expected) || !std::isfinite(p_actual))
        {
            return std::isnan(p_expected) ? std::isnan(p_actual) : p_actual == p_expected;
        }
        return std::abs(p_actual - p_expected) <= 0.01 * (std::max)(1.0, std::abs(p_expected));
    }

    // Expects every output of every period to have the same value in both models, to the tolerance above.
    void expectSameOutputs(const Models::FMTLpModel& p_expected, const Models::FMTLpModel& p_actual)
    {
        for (const Core::FMTOutput& output : p_expected.getOutputs())
        {
            for (int period = 1; period <= LENGTH; ++period)
            {
                const double EXPECTED = p_expected.getOutput(output, period, Core::FMToutputlevel::totalonly).at("Total");
                const double ACTUAL = p_actual.getOutput(output, period, Core::FMToutputlevel::totalonly).at("Total");
                EXPECT_TRUE(isNear(ACTUAL, EXPECTED)) << output.getName() << " at period " << period << ": " << ACTUAL
                    << " unpickled, " << EXPECTED << " pickled";
            }
        }
    }
}

// Defects 26 (the yields do not save), 27 (the empty matrix of a model not built does not save), 28 (reading a model
// passes the logger to a solver that is not read yet) and 25 (the lists do not read what they save).
TEST(FMTLpModel, DISABLED_GivenAnLpModelOfTWDLandNotBuiltWhenPickledThenUnpicklesTheSameModel)
{
    // Arrange
    const Models::FMTLpModel model = getModel("LP");

    // Act
    const Models::FMTLpModel unpickled = Testing::pickleAndUnpickle(model);

    // Assert
    Testing::expectSameModel(model, unpickled);
    EXPECT_EQ(unpickled.getParameter(Models::FMTintmodelparameters::LENGTH), LENGTH);
}

// Defects 26, 28, 25, then 31: the sense of the objective, _MAXMIN, is not saved, and a solver read back minimizes.
TEST(FMTLpModel, DISABLED_GivenAnLpModelOfTWDLandSolvedWhenPickledThenSolvingItAgainGivesTheSameOptimumAndOutputs)
{
    // Arrange
    const Models::FMTLpModel model = getSolvedModel();
    Models::FMTLpModel unpickled = Testing::pickleAndUnpickle(model);

    // Act
    unpickled.initialSolve();

    // Assert
    ASSERT_TRUE(model.isOptimal());
    ASSERT_TRUE(unpickled.isOptimal());
    EXPECT_TRUE(isNear(unpickled.getObjValue(), model.getObjValue()));
    expectSameOutputs(model, unpickled);
}

// Defects 26, 28 and 25. The solver of a model saves its solution with its matrix (FMTSerializableMatrix): the outputs
// of the unpickled model are those of the solution, without solving it again.
TEST(FMTLpModel, DISABLED_GivenAnLpModelOfTWDLandSolvedWhenPickledThenKeepsTheOutputsOfItsSolution)
{
    // Arrange
    const Models::FMTLpModel model = getSolvedModel();

    // Act
    const Models::FMTLpModel unpickled = Testing::pickleAndUnpickle(model);

    // Assert
    ASSERT_TRUE(model.isOptimal());
    expectSameOutputs(model, unpickled);
}
#endif
