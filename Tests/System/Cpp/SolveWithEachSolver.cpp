#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
	#include "FMTLpSolver.h"
	#include "FMTModelParser.h"
#endif
#include "FMTVersion.h"
#include "FMTDefaultLogger.h"


#ifdef FMTWITHOSI
int testLoggerLifetime()
{
	const std::string PRIMARY_LOCATION = "../../../../Examples/Models/TWD_land/TWD_land.pri";
	const std::vector<Models::FMTSolverInterface> SOLVERS = Models::FMTLpSolver::getAvailableSolverInterface();
	const auto CLP = std::find(SOLVERS.begin(), SOLVERS.end(), Models::FMTSolverInterface::CLP);
	if (CLP == SOLVERS.end())
		{
		std::cerr << "CLP solver is unavailable" << std::endl;
		return 1;
		}

	Parser::FMTModelParser modelParser;
	Models::FMTModel model = modelParser.readproject(PRIMARY_LOCATION, { "LP3" }).at(0);
	model.setParameter(Models::FMTintmodelparameters::LENGTH, 5);
	Models::FMTLpModel solver(model, *CLP);
	if (!solver.doPlanning(true))
		{
		return 1;
		}

	Parser::FMTModelParser quietParser;
	quietParser.setQuietLogger();
	for (int copyIndex = 0; copyIndex < 2000; ++copyIndex)
		{
		Models::FMTLpModel copy(solver);
		}
	return 0;
}
#endif

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	if (argc > 1 && std::string(argv[1]) == "loggerlifetime")
		{
		return testLoggerLifetime();
		}
#endif
bool failed = true;
#ifdef FMTWITHOSI
	failed = false;
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTVersion().hasFeature("OSI"))
	{
		const std::string PRIMARY_LOCATION = "../../../../Examples/Models/TWD_land/TWD_land.pri";
		const std::string SCENARIO = "LP3";
		Parser::FMTModelParser modelParser;
		//modelParser.setQuietLogger();
		const std::vector<std::string>scenarios(1, SCENARIO);
		Models::FMTModel Model = modelParser.readproject(PRIMARY_LOCATION, scenarios).at(0);
		Model.setParameter(Models::FMTintmodelparameters::LENGTH, 3);
		for (const Models::FMTSolverInterface SOLVER_TYPE : Models::FMTLpSolver::getAvailableSolverInterface())
			{
			Models::FMTLpModel OptModel(Model, SOLVER_TYPE);
			const bool SOLVED = OptModel.doPlanning(true);
			const std::string SOLVER_NAME = OptModel.getConstSolverPtr()->getSolverName();
			if (!SOLVED)
				{
				std::cout << "Cant solve with " << SOLVER_NAME << std::endl;
				failed = true;
			}else{
				std::cout << "Solved successfully with " << SOLVER_NAME << " objective of " << OptModel.getObjectiveValue() << std::endl;
				}
			}

	}else {
		failed = true;
		std::cout << "FMT needs to be compiled with OSI" << std::endl;
		}
#endif 
	return failed ? 1 : 0;
}

