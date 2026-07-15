#include <vector>
#ifdef FMTWITHONNXR
	#ifdef FMTWITHGDAL
		#include "FMTlpmodel.h"
		#include "FMTmodelparser.h"
		#include "FMTversion.h"
		#include "FMToutput.h"
	#endif
#endif
#include "FMTdefaultlogger.h"

	int main()
	{
#ifdef FMTWITHONNXR
	#ifdef FMTWITHGDAL
			const std::string modellocation = "../../../../Examples/Models/TWD_land/";
			const std::string	primarylocation = modellocation + "TWD_land.pri";
			Parser::FMTmodelparser mparser;
			const std::vector<std::string>scenarios(1, "Predictors");
			const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
			Models::FMTlpmodel optimizationmodel(models.at(0),Models::FMTsolverinterface::CLP);
			optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, 10);
			if (optimizationmodel.doPlanning(true))
				{
				for (const Core::FMToutput& output : optimizationmodel.getOutputs())
					{
					if (output.getName().find("_") != std::string::npos)
						{
						for (int period = 1; period < 11; ++period)
							{
							Logging::FMTdefaultlogger() << "output value "<< output.getName() <<" " << optimizationmodel.getOutput(output, period, Core::FMToutputlevel::totalonly).at("Total") << " at period " << period << "\n";
							}
						//break;
						}
					}
				}
			#endif
#endif
			return 0;
	}
        
