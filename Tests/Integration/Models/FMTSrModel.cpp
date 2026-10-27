/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
    #include "FMTModelParser.h"
    #include "FMTModel.h"
    #include "FMTLpModel.h"
    #include "FMTLpSolver.h"
    #include "FMTGraphStats.h"
    #include "FMTSchedule.h"
    #include "FMTOutput.h"
    #include "FMTAction.h"
    #include "FMTActualDevelopment.h"
    #include "FMTDevelopment.h"
    #include "FMTException.h"
#endif
#include "TestTools.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// FMTSrModel, through FMTLpModel, with its real solver: MOSEK when FMT is built with it, CLP otherwise. Each case
// reads a scenario of TWD_land and builds its graph and its matrix over five periods.
//
// buildPeriod: the vertices, edges, rows and columns that each period adds, on the scenarios of the STATS rows of
// PlanScenario (Tests/System/Cpp/basetests.csv), whose fifth period gives the vertices and edges of those rows, and on
// harvestfromperiod3, whose harvest enters the matrix at the period 3. Each model is presolved first, as doPlanning
// does. The values of the period 1 are counted by hand in section 5.14 of Tests/ETAT.md.
//
// setSolution, setSolutionByLp and boundSolution: the optimal schedules of LP, set again period by period, give the
// outputs and the optimum of the optimization; nothing to maintain, the optimized model is its own reference.
//
// The disabled cases fail on a behavior of FMTlib, named above each of them.

#ifdef FMTWITHOSI
namespace
{
    // The primary file of TWD_land, from bin/<build type>, the working directory of the tests.
    const std::string TWD_LAND_PRIMARY_FILE = "../../../../Examples/Models/TWD_land/TWD_land.pri";
    const int LENGTH = 5;
    // The tolerance of a schedule set in a matrix, as in SetSolution and PlanScenario.
    const double SCHEDULE_TOLERANCE = 0.01;
    // The tolerance of boundSolution, relative to the value of each variable.
    const double BOUND_TOLERANCE = 0.0001;

    Models::FMTModel readScenario(const std::string& p_scenario)
    {
        Parser::FMTModelParser parser;
        parser.setDefaultExceptionHandler();
        return parser.readproject(TWD_LAND_PRIMARY_FILE, std::vector<std::string>(1, p_scenario)).at(0);
    }

    Models::FMTLpModel getModel(const Models::FMTModel& p_model)
    {
        #ifdef FMTWITHMOSEK
            Models::FMTLpModel model(p_model, Models::FMTSolverInterface::MOSEK);
        #else
            Models::FMTLpModel model(p_model, Models::FMTSolverInterface::CLP);
        #endif
        model.setParameter(Models::FMTintmodelparameters::LENGTH, LENGTH);
        return model;
    }

    // Returns the model of a scenario presolved, as doPlanning presolves it before building its graph.
    std::unique_ptr<Models::FMTModel> getPresolvedModel(const std::string& p_scenario)
    {
        return getModel(readScenario(p_scenario)).preSolve();
    }

    std::string toString(const Graph::FMTGraphStats& p_stats)
    {
        return std::to_string(p_stats.vertices) + "|" + std::to_string(p_stats.edges) + "|"
            + std::to_string(p_stats.rows) + "|" + std::to_string(p_stats.cols);
    }

    // Builds the periods of a model one by one and returns <vertices>|<edges>|<rows>|<columns> after each.
    std::vector<std::string> buildPeriodByPeriod(Models::FMTModel& p_model)
    {
        Models::FMTLpModel& model = dynamic_cast<Models::FMTLpModel&>(p_model);
        std::vector<std::string> stats;
        for (int period = 1; period <= LENGTH; ++period)
        {
            stats.push_back(toString(model.buildPeriod()));
        }
        return stats;
    }

    // Returns a scenario built and solved without presolve, so that the matrix holds the variables of the model.
    Models::FMTLpModel getSolvedModel(const std::string& p_scenario)
    {
        Models::FMTLpModel model = getModel(readScenario(p_scenario));
        model.build();
        model.solve();
        return model;
    }

    // An optimized scenario and its optimal schedules, from the period 1 to LENGTH.
    struct Optimization
    {
        Models::FMTModel read;
        Models::FMTLpModel optimized;
        std::vector<Core::FMTSchedule> schedules;
    };

    Optimization optimize(const std::string& p_scenario)
    {
        const Models::FMTModel READ = readScenario(p_scenario);
        Optimization optimization{ READ, getModel(READ), {} };
        optimization.optimized.doPlanning(true);
        for (int period = 1; period <= LENGTH; ++period)
        {
            optimization.schedules.push_back(optimization.optimized.getSolution(period));
        }
        return optimization;
    }

    // Builds the graph of the schedules only (partial build), period by period, as a replay does.
    void buildSchedules(Models::FMTLpModel& p_model, const std::vector<Core::FMTSchedule>& p_schedules)
    {
        for (const Core::FMTSchedule& schedule : p_schedules)
        {
            p_model.buildPeriod(schedule, true);
        }
    }

    // Expects every output of every period to have the same value in both models, to p_relativeTolerance of the
    // value, or of 1 below 1. An output without a finite value, such as the ratio of two null outputs, must give the
    // same non-finite value.
    void expectSameOutputs(const Models::FMTLpModel& p_expected, const Models::FMTLpModel& p_actual,
        double p_relativeTolerance)
    {
        size_t comparisons = 0;
        for (const Core::FMTOutput& output : p_expected.getOutputs())
        {
            for (int period = 1; period <= LENGTH; ++period)
            {
                const double EXPECTED =
                    p_expected.getOutput(output, period, Core::FMToutputlevel::totalonly).at("Total");
                const double ACTUAL = p_actual.getOutput(output, period, Core::FMToutputlevel::totalonly).at("Total");
                if (std::isnan(EXPECTED) || std::isinf(EXPECTED))
                {
                    EXPECT_TRUE((std::isnan(EXPECTED) && std::isnan(ACTUAL)) || EXPECTED == ACTUAL)
                        << output.getName() << " at period " << period << ": " << EXPECTED << " then " << ACTUAL;
                }
                else
                {
                    EXPECT_NEAR(ACTUAL, EXPECTED, p_relativeTolerance * (std::max)(1.0, std::abs(EXPECTED)))
                        << output.getName() << " at period " << period;
                }
                ++comparisons;
            }
        }
        EXPECT_GT(comparisons, 0u);
    }

    Core::FMTAction getAction(const Models::FMTModel& p_model, const std::string& p_name)
    {
        const std::vector<Core::FMTAction> ACTIONS = p_model.getActions();
        return *std::find_if(ACTIONS.begin(), ACTIONS.end(), Core::FMTActionComparator(p_name));
    }

    // Returns the development of the area whose mask and age are given, at the period 1.
    Core::FMTDevelopment getDevelopmentAtPeriodOne(const Models::FMTModel& p_model, const std::string& p_mask,
        int p_age)
    {
        for (const Core::FMTActualDevelopment& actual : p_model.getArea())
        {
            if (std::string(actual.getMask()) == p_mask && actual.getAge() == p_age)
            {
                Core::FMTDevelopment development(actual);
                development.setPeriod(1);
                return development;
            }
        }
        throw std::invalid_argument("No development " + p_mask + " of age " + std::to_string(p_age) + " in the area");
    }

    // A schedule of the period 1 that operates one development.
    Core::FMTSchedule getScheduleOfPeriodOne(const Core::FMTDevelopment& p_development, double p_area,
        const Core::FMTAction& p_action)
    {
        Core::FMTSchedule schedule;
        schedule.setPeriod(1);
        schedule.addEvent(p_development, p_area, p_action);
        return schedule;
    }

    // Calls p_call and returns the type of each FMT exception it raises, outermost first: none when it raises
    // nothing. FMT raises an error, then each method it goes through wraps it in an FMTfunctionfailed.
    std::vector<Exception::FMTexc> getRaisedTypes(const std::function<void()>& p_call)
    {
        std::vector<Exception::FMTexc> types;
        try
        {
            p_call();
        }
        catch (const std::exception& error)
        {
            Testing::visitNested(error, [&types](const std::exception& p_nested, std::size_t)
                {
                const Exception::FMTException* FMT_EXCEPTION = dynamic_cast<const Exception::FMTException*>(&p_nested);
                if (FMT_EXCEPTION != nullptr)
                {
                    types.push_back(FMT_EXCEPTION->getType());
                }
                });
        }
        return types;
    }

    bool hasType(const std::vector<Exception::FMTexc>& p_types, Exception::FMTexc p_type)
    {
        return std::find(p_types.begin(), p_types.end(), p_type) != p_types.end();
    }

    std::vector<double> getColumnLowerBounds(const Models::FMTLpModel& p_model)
    {
        const Models::FMTLpSolver* SOLVER = p_model.getConstSolverPtr();
        return std::vector<double>(SOLVER->getColLower(), SOLVER->getColLower() + SOLVER->getNumCols());
    }
}
#endif

// LP keeps its fire at 0 (Burnedarea = 0): the presolve removes the fire, its salvage and the death of the burnt
// stands, and leaves the harvest, the caribou action and the plantation.
TEST(FMTSrModel, GivenAMaxMinHarvestWhenBuildPeriodThenEachPeriodAddsTheKnownGraphAndMatrix)
{
#ifdef FMTWITHOSI
    // Arrange
    const std::unique_ptr<Models::FMTModel> model = getPresolvedModel("LP");
    const std::vector<std::string> EXPECTED = { "44|42|7|28", "89|96|25|67", "146|174|59|134", "215|276|105|225",
        "296|395|156|326" };

    // Act
    const std::vector<std::string> stats = buildPeriodByPeriod(*model);

    // Assert
    EXPECT_EQ(stats, EXPECTED);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// division, as equation, keeps every action of the root: its graph is the graph of the root.
TEST(FMTSrModel, GivenAConstraintOnARatioOfOutputsWhenBuildPeriodThenEachPeriodAddsTheKnownGraphAndMatrix)
{
#ifdef FMTWITHOSI
    // Arrange
    const std::unique_ptr<Models::FMTModel> model = getPresolvedModel("division");
    const std::vector<std::string> EXPECTED = { "62|69|22|57", "126|173|63|150", "208|320|122|286", "308|510|199|465",
        "426|730|294|674" };

    // Act
    const std::vector<std::string> stats = buildPeriodByPeriod(*model);

    // Assert
    EXPECT_EQ(stats, EXPECTED);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

TEST(FMTSrModel, GivenAnObjectiveOnAnEquationYieldWhenBuildPeriodThenEachPeriodAddsTheKnownGraphAndMatrix)
{
#ifdef FMTWITHOSI
    // Arrange
    const std::unique_ptr<Models::FMTModel> model = getPresolvedModel("equation");
    const std::vector<std::string> EXPECTED = { "62|69|22|57", "126|173|63|150", "208|320|122|286", "308|510|199|465",
        "426|730|294|674" };

    // Act
    const std::vector<std::string> stats = buildPeriodByPeriod(*model);

    // Assert
    EXPECT_EQ(stats, EXPECTED);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// harvestfromperiod3 is the root with COUPEtotale operable from the period 3 only: the periods 1 and 2 have neither
// its 7 edges and columns nor the stands it cuts, 12 vertices fewer than division at the period 1. The period 2 adds
// fewer vertices than the period 1; from the period 3, the harvest makes each period add more.
TEST(FMTSrModel, GivenAHarvestOperableFromThePeriodThreeWhenBuildPeriodThenTheHarvestEntersTheMatrixAtThePeriodThree)
{
#ifdef FMTWITHOSI
    // Arrange
    const std::unique_ptr<Models::FMTModel> model = getPresolvedModel("harvestfromperiod3");
    const std::vector<std::string> EXPECTED = { "50|56|22|50", "96|128|56|116", "166|239|103|216", "254|393|168|359",
        "360|583|251|538" };

    // Act
    const std::vector<std::string> stats = buildPeriodByPeriod(*model);

    // Assert
    EXPECT_EQ(stats, EXPECTED);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// doPlanning builds every period, then adds the rows and the columns of the constraints and the objective: the
// vertices, the edges and the transfer rows are those of the last period built one by one.
TEST(FMTSrModel, GivenAMaxMinHarvestWhenBuildPeriodOneByOneThenBuildsTheGraphOfDoPlanning)
{
#ifdef FMTWITHOSI
    // Arrange
    const std::unique_ptr<Models::FMTModel> presolved = getPresolvedModel("LP");
    Models::FMTLpModel& model = dynamic_cast<Models::FMTLpModel&>(*presolved);
    Models::FMTLpModel planned = getModel(readScenario("LP"));
    planned.doPlanning(false);

    // Act
    Graph::FMTGraphStats lastPeriod;
    for (int period = 1; period <= LENGTH; ++period)
    {
        lastPeriod = model.buildPeriod();
    }

    // Assert
    const Graph::FMTGraphStats PLANNED = planned.getStats();
    EXPECT_EQ(lastPeriod.vertices, PLANNED.vertices);
    EXPECT_EQ(lastPeriod.edges, PLANNED.edges);
    EXPECT_EQ(lastPeriod.transfer_rows, PLANNED.transfer_rows);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// setSolution leaves out the areas of a lock-exempt action below its tolerance: with CLP, a plantation of 0.007 ha of
// the optimal schedule is not set again. The outputs are compared to the tolerance of PlanScenario.
TEST(FMTSrModel, GivenTheOptimalSchedulesWhenSetSolutionPeriodByPeriodThenGivesTheOptimizedOutputs)
{
#ifdef FMTWITHOSI
    // Arrange
    const Optimization OPTIMIZATION = optimize("LP");
    Models::FMTLpModel replay = getModel(OPTIMIZATION.read);
    buildSchedules(replay, OPTIMIZATION.schedules);

    // Act
    std::vector<bool> set;
    for (int period = 1; period <= LENGTH; ++period)
    {
        set.push_back(replay.setSolution(period, OPTIMIZATION.schedules.at(period - 1), SCHEDULE_TOLERANCE));
    }

    // Assert
    EXPECT_EQ(set, std::vector<bool>(LENGTH, true));
    expectSameOutputs(OPTIMIZATION.optimized, replay, 0.01);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// Each boundSolution resolves the matrix: the periods not bound yet may take another optimal solution, whose
// objective is the same (Tests/ETAT.md, section 1.4, rule 7). Only the optimum is compared.
TEST(FMTSrModel, GivenTheOptimalSchedulesSetInTheWholeMatrixWhenBoundSolutionThenResolvesToTheOptimum)
{
#ifdef FMTWITHOSI
    // Arrange
    const Optimization OPTIMIZATION = optimize("LP");
    Models::FMTLpModel replay = getModel(OPTIMIZATION.read);
    replay.build();
    for (int period = 1; period <= LENGTH; ++period)
    {
        replay.setSolution(period, OPTIMIZATION.schedules.at(period - 1), SCHEDULE_TOLERANCE);
    }

    // Act
    std::vector<bool> resolved;
    for (int period = 1; period <= LENGTH; ++period)
    {
        resolved.push_back(replay.boundSolution(period, BOUND_TOLERANCE));
    }

    // Assert
    EXPECT_EQ(resolved, std::vector<bool>(LENGTH, true));
    ASSERT_TRUE(replay.isOptimal());
    const double OPTIMUM = OPTIMIZATION.optimized.getObjValue();
    EXPECT_NEAR(replay.getObjValue(), OPTIMUM, 1e-6 * (std::max)(1.0, std::abs(OPTIMUM)));
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// setSolutionByLp bounds each variable of the schedule to its area, plus or minus the tolerance, and solves: its
// outputs may differ from those of setSolution by the tolerance on each variable, hence the tolerance of PlanScenario.
TEST(FMTSrModel, GivenTheOptimalSchedulesWhenSetSolutionByLpThenGivesTheOutputsOfSetSolution)
{
#ifdef FMTWITHOSI
    // Arrange
    const Optimization OPTIMIZATION = optimize("LP");
    Models::FMTLpModel bySetSolution = getModel(OPTIMIZATION.read);
    Models::FMTLpModel byLp = getModel(OPTIMIZATION.read);
    buildSchedules(bySetSolution, OPTIMIZATION.schedules);
    buildSchedules(byLp, OPTIMIZATION.schedules);
    for (int period = 1; period <= LENGTH; ++period)
    {
        bySetSolution.setSolution(period, OPTIMIZATION.schedules.at(period - 1), SCHEDULE_TOLERANCE);
    }

    // Act
    std::vector<bool> set;
    for (int period = 1; period <= LENGTH; ++period)
    {
        set.push_back(byLp.setSolutionByLp(period, OPTIMIZATION.schedules.at(period - 1), SCHEDULE_TOLERANCE));
    }

    // Assert
    EXPECT_EQ(set, std::vector<bool>(LENGTH, true));
    expectSameOutputs(bySetSolution, byLp, 0.01);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// The stand of 403.28 ha cut on 503.28 ha. With SETSOLUTION_THROW off, as FMTLpModel::trySetSolution sets it before
// it falls back on setSolutionByLp, setSolution returns false, as it documents.
TEST(FMTSrModel, GivenMoreAreaThanItsDevelopmentWhenSetSolutionWithoutThrowingThenReturnsFalse)
{
#ifdef FMTWITHOSI
    // Arrange
    const Models::FMTModel READ = readScenario("LP");
    Models::FMTLpModel model = getModel(READ);
    model.build();
    model.setParameter(Models::FMTboolmodelparameters::SETSOLUTION_THROW, false);
    const Core::FMTSchedule SCHEDULE = getScheduleOfPeriodOne(
        getDevelopmentAtPeriodOne(READ, "UNITE1 PEUPLEMENT1 UTR1", 12), 503.28, getAction(READ, "COUPETOTALE"));

    // Act
    const bool SET = model.setSolution(1, SCHEDULE, SCHEDULE_TOLERANCE);

    // Assert
    EXPECT_FALSE(SET);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// SETSOLUTION_THROW is on by default: the negative growth left by the schedule is an invalid number.
TEST(FMTSrModel, GivenMoreAreaThanItsDevelopmentWhenSetSolutionThenRaisesAnInvalidNumber)
{
#ifdef FMTWITHOSI
    // Arrange
    const Models::FMTModel READ = readScenario("LP");
    Models::FMTLpModel model = getModel(READ);
    model.build();
    const Core::FMTSchedule SCHEDULE = getScheduleOfPeriodOne(
        getDevelopmentAtPeriodOne(READ, "UNITE1 PEUPLEMENT1 UTR1", 12), 503.28, getAction(READ, "COUPETOTALE"));

    // Act
    const std::vector<Exception::FMTexc> TYPES = getRaisedTypes([&]()
        {
        model.setSolution(1, SCHEDULE, SCHEDULE_TOLERANCE);
        });

    // Assert
    EXPECT_TRUE(hasType(TYPES, Exception::FMTexc::FMTinvalid_number));
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// aCaribou is operable on UC (unite1 and unite2) from 8 years: not on the stand of unite3 of 8 years, which is in the
// graph at the period 1.
TEST(FMTSrModel, GivenAnActionNotOperableOnItsDevelopmentWhenSetSolutionWithoutThrowingThenReturnsFalse)
{
#ifdef FMTWITHOSI
    // Arrange
    const Models::FMTModel READ = readScenario("LP");
    Models::FMTLpModel model = getModel(READ);
    model.build();
    model.setParameter(Models::FMTboolmodelparameters::SETSOLUTION_THROW, false);
    const Core::FMTSchedule SCHEDULE = getScheduleOfPeriodOne(
        getDevelopmentAtPeriodOne(READ, "UNITE3 PEUPLEMENT3 UTR3", 8), 10.0, getAction(READ, "ACARIBOU"));

    // Act
    const bool SET = model.setSolution(1, SCHEDULE, SCHEDULE_TOLERANCE);

    // Assert
    EXPECT_FALSE(SET);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

TEST(FMTSrModel, GivenAnActionNotOperableOnItsDevelopmentWhenSetSolutionThenRaisesAnInvalidNumber)
{
#ifdef FMTWITHOSI
    // Arrange
    const Models::FMTModel READ = readScenario("LP");
    Models::FMTLpModel model = getModel(READ);
    model.build();
    const Core::FMTSchedule SCHEDULE = getScheduleOfPeriodOne(
        getDevelopmentAtPeriodOne(READ, "UNITE3 PEUPLEMENT3 UTR3", 8), 10.0, getAction(READ, "ACARIBOU"));

    // Act
    const std::vector<Exception::FMTexc> TYPES = getRaisedTypes([&]()
        {
        model.setSolution(1, SCHEDULE, SCHEDULE_TOLERANCE);
        });

    // Assert
    EXPECT_TRUE(hasType(TYPES, Exception::FMTexc::FMTinvalid_number));
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// Disabled: unboundSolution gives the variables of the period the bounds -max and +max of a double, not the bounds
// they had (0 and infinity), so that the areas may be negative and the matrix is no longer optimal (#399, defect 19
// of section 2.7 of Tests/ETAT.md).
TEST(FMTSrModel, DISABLED_GivenBoundSolutionsWhenUnboundSolutionThenResolvesToTheModelBefore)
{
#ifdef FMTWITHOSI
    // Arrange
    Models::FMTLpModel model = getSolvedModel("LP");
    const double OPTIMUM = model.getObjValue();
    const std::vector<double> LOWER_BOUNDS = getColumnLowerBounds(model);
    for (int period = 1; period <= LENGTH; ++period)
    {
        model.boundSolution(period, BOUND_TOLERANCE);
    }

    // Act
    for (int period = 1; period <= LENGTH; ++period)
    {
        model.unboundSolution(period);
    }
    const bool RESOLVED = model.resolve();

    // Assert
    EXPECT_EQ(getColumnLowerBounds(model), LOWER_BOUNDS);
    EXPECT_TRUE(RESOLVED);
    ASSERT_TRUE(model.isOptimal());
    EXPECT_NEAR(model.getObjValue(), OPTIMUM, 1e-6 * (std::max)(1.0, std::abs(OPTIMUM)));
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

TEST(FMTSrModel, GivenBoundSolutionsWhenIsPeriodBoundedThenReturnsTrue)
{
#ifdef FMTWITHOSI
    // Arrange
    Models::FMTLpModel model = getSolvedModel("LP");
    model.boundSolution(1, BOUND_TOLERANCE);

    // Act
    const bool BOUNDED = model.isPeriodBounded(1);

    // Assert
    EXPECT_TRUE(BOUNDED);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// Disabled: isPeriodBounded counts only the bounded variables of the period, among all and among the bounded ones,
// so that it returns true for any period of the graph (#400, defect 20 of section 2.7 of Tests/ETAT.md).
TEST(FMTSrModel, DISABLED_GivenAnOptimizedModelWhenIsPeriodBoundedThenReturnsFalse)
{
#ifdef FMTWITHOSI
    // Arrange
    Models::FMTLpModel model = getSolvedModel("LP");

    // Act
    const bool BOUNDED = model.isPeriodBounded(1);

    // Assert
    EXPECT_FALSE(BOUNDED);
#else
    GTEST_SKIP() << "FMT is built without OSI";
#endif
}
