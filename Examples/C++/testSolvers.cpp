#include <vector>
#include <iostream>
#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
	#include "FMTModelParser.h"
#endif
#include "FMTVersion.h"
#include "FMTDefaultLogger.h"


int main(int argc, char* argv[])
{
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
		std::vector<Models::FMTSolverInterface>SolversToTest;
		SolversToTest.push_back(Models::FMTSolverInterface::CLP);
		SolversToTest.push_back(Models::FMTSolverInterface::GLPK);
		#ifdef FMTWITHMOSEK
			SolversToTest.push_back(Models::FMTSolverInterface::MOSEK);
		#endif
		for (const Models::FMTSolverInterface SOLVER_TYPE : SolversToTest)
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

