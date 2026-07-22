#include <vector>
#ifdef FMTWITHONNXR
	#ifdef FMTWITHGDAL
		#include "FMTLpModel.h"
		#include "FMTModelParser.h"
		#include "FMTVersion.h"
	#endif
#endif

	int main()
	{
	#ifdef FMTWITHONNXR
		#ifdef FMTWITHGDAL
				const std::string modellocation = "../../../../Examples/Models/TWD_land/";
				const std::string	primarylocation = modellocation + "TWD_land.pri";
				Parser::FMTModelParser mparser;
				const std::vector<std::string>scenarios(1, "LPPredictors");
				const std::vector<Models::FMTModel> models = mparser.readproject(primarylocation, scenarios);
				Models::FMTLpModel optimizationmodel(models.at(0),Models::FMTsolverinterface::CLP);
				optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, 10);
				optimizationmodel.doPlanning(true);
		#endif
	#endif  
	return 0;
	}
