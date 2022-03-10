#include <vector>
#ifdef FMTWITHGDAL
	#include "FMTlpmodel.hpp"
	#include "FMTmodelparser.hpp"
	#include "FMTversion.hpp"
	#include "FMToutput.hpp"
#endif

int main()
{
#ifdef FMTWITHGDAL
		const std::string modellocation = "../../../../Examples/Models/TWD_land/";
		const std::string	primarylocation = modellocation + "TWD_land.pri";
		Parser::FMTmodelparser mparser;
		const std::vector<std::string>scenarios(1, "Predictors");
		const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0),Models::FMTsolverinterface::CLP);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, 10);
		if (optimizationmodel.doplanning(true))
			{
			for (const Core::FMToutput& output : optimizationmodel.getoutputs())
				{
				if (output.getname().find("_") != std::string::npos)
					{
					for (int period = 1; period < 11; ++period)
						{
						Logging::FMTlogger() << "output value " << optimizationmodel.getoutput(output,period,Core::FMToutputlevel::totalonly).at("Total") << " at period " << period << "\n";
						}
					break;
					}
				}
			}
		#endif
		return 0;
}
        
