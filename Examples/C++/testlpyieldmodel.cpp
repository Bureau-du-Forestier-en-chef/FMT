#include <vector>
#ifdef FMTWITHONNXR
	#ifdef FMTWITHGDAL
		#include "FMTLpModel.h"
		#include "FMTModelParser.h"
		#include "FMTVersion.h"
		#include "FMTOutput.h"
	#endif
#endif
#include "FMTDefaultLogger.h"

	int main()
	{
#ifdef FMTWITHONNXR
	#ifdef FMTWITHGDAL
			const std::string modellocation = "../../../../Examples/Models/TWD_land/";
			const std::string	primarylocation = modellocation + "TWD_land.pri";
			Parser::FMTModelParser mparser;
			const std::vector<std::string>scenarios(1, "Predictors");
			const std::vector<Models::FMTModel> models = mparser.readproject(primarylocation, scenarios);
			Models::FMTLpModel optimizationmodel(models.at(0),Models::FMTSolverInterface::CLP);
			optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, 10);
			if (optimizationmodel.doPlanning(true))
				{
				for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
					{
					if (output.getName().find("_") != std::string::npos)
						{
						for (int period = 1; period < 11; ++period)
							{
							Logging::FMTDefaultLogger() << "output value "<< output.getName() <<" " << optimizationmodel.getOutput(output, period, Core::FMToutputlevel::totalonly).at("Total") << " at period " << period << "\n";
							}
						//break;
						}
					}
				}
			#endif
#endif
			return 0;
	}
        
