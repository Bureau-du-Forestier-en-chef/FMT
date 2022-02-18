#include <vector>
#ifdef FMTWITHGDAL
	#include "FMTlpmodel.hpp"
	#include "FMTmodelparser.hpp"
	#include "FMTversion.hpp"
#endif

int main()
{
#ifdef FMTWITHGDAL
		const std::string modellocation = "../../../../Examples/Models/TWD_land/";
		const std::string	primarylocation = modellocation + "TWD_land.pri";
		Parser::FMTmodelparser mparser;
		const std::vector<std::string>scenarios(1, "LPPredictors");
		const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0),Models::FMTsolverinterface::CLP);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, 10);
		optimizationmodel.doplanning(true);
		#endif
		return 0;
}
        
