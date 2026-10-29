/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
    #include "PickleArchive.h"
    #include "FMTLpSolver.h"
    #include "FMTsolverinterface.h"
#endif

#include <gtest/gtest.h>

#include <limits>
#include <vector>

// Models::FMTLpSolver through the archive of the pickle of Python: the solver of a model saves its matrix, its bounds,
// its objective and its solution in an FMTSerializableMatrix (FMTLpSolver::save), with its real solver, MOSEK when FMT
// is built with it, CLP otherwise. The problem is built in code:
//     maximize x + y subject to x + 2y <= 4 and 3x + y <= 6, x and y >= 0,
// whose optimum, 2.8, is at x = 1.6 and y = 1.2.

#ifdef FMTWITHOSI
namespace
{
    const double TOLERANCE = 1e-6;
    const double NO_LOWER_BOUND = -(std::numeric_limits<double>::max)();

    // A solver without any parameter file, as a model without the parameters SOLVER_COLD_START and SOLVER_WARM_START.
    Models::FMTLpSolver getSolver()
    {
        #ifdef FMTWITHMOSEK
            return Models::FMTLpSolver(Models::FMTSolverInterface::MOSEK, "", "", "");
        #else
            return Models::FMTLpSolver(Models::FMTSolverInterface::CLP, "", "", "");
        #endif
    }

    // The problem above, solved.
    Models::FMTLpSolver getSolvedProblem()
    {
        Models::FMTLpSolver solver = getSolver();
        solver.addCol(0, nullptr, nullptr, 0.0, (std::numeric_limits<double>::max)(), 1.0);
        solver.addCol(0, nullptr, nullptr, 0.0, (std::numeric_limits<double>::max)(), 1.0);
        const std::vector<int> COLUMNS({ 0, 1 });
        const std::vector<double> FIRST_ROW({ 1.0, 2.0 });
        const std::vector<double> SECOND_ROW({ 3.0, 1.0 });
        solver.addRow(2, COLUMNS.data(), FIRST_ROW.data(), NO_LOWER_BOUND, 4.0);
        solver.addRow(2, COLUMNS.data(), SECOND_ROW.data(), NO_LOWER_BOUND, 6.0);
        solver.setObjSense(-1.0);
        solver.initialSolve();
        return solver;
    }
}

TEST(FMTLpSolver, GivenASolvedProblemWhenPickledThenUnpicklesTheSameProblemAndItsSolution)
{
    // Arrange
    const Models::FMTLpSolver solver = getSolvedProblem();

    // Act
    const Models::FMTLpSolver unpickled = Testing::pickleAndUnpickle(solver);

    // Assert
    ASSERT_TRUE(solver.isProvenOptimal());
    ASSERT_EQ(unpickled.getNumCols(), 2);
    ASSERT_EQ(unpickled.getNumRows(), 2);
    for (int column = 0; column < 2; ++column)
    {
        EXPECT_DOUBLE_EQ(unpickled.getObjCoefficients()[column], 1.0);
        EXPECT_DOUBLE_EQ(unpickled.getColLower()[column], 0.0);
    }
    EXPECT_NEAR(unpickled.getColSolution()[0], 1.6, TOLERANCE);
    EXPECT_NEAR(unpickled.getColSolution()[1], 1.2, TOLERANCE);
}

// Defect 31 of Tests/ETAT.md (section 2.7): the sense of the objective is not saved; the unpickled problem is loaded in a
// new solver interface, which minimizes: solved again, it gives 0 instead of 2.8.
TEST(FMTLpSolver, DISABLED_GivenAMaximizedProblemWhenPickledThenSolvingItAgainGivesTheSameOptimum)
{
    // Arrange
    const Models::FMTLpSolver solver = getSolvedProblem();
    Models::FMTLpSolver unpickled = Testing::pickleAndUnpickle(solver);

    // Act
    unpickled.initialSolve();

    // Assert
    ASSERT_TRUE(unpickled.isProvenOptimal());
    EXPECT_NEAR(unpickled.getObjValue(), 2.8, TOLERANCE);
}

// Defect 27 of Tests/ETAT.md (section 2.7): FMTSerializableMatrix reads one element of an empty matrix
// (std::max(1, getNumElements())), whose array is null: saving a solver without any column is an access violation.
TEST(FMTLpSolver, DISABLED_GivenASolverWithoutAnyColumnWhenPickledThenUnpicklesASolverWithoutAnyColumn)
{
    // Arrange
    const Models::FMTLpSolver solver = getSolver();

    // Act
    const Models::FMTLpSolver unpickled = Testing::pickleAndUnpickle(solver);

    // Assert
    EXPECT_EQ(unpickled.getNumCols(), 0);
    EXPECT_EQ(unpickled.getNumRows(), 0);
}
#endif
