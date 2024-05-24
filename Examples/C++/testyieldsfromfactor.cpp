#include <vector>
#ifdef FMTWITHOSI
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTmodelparser.h"
    #include "FMTlpmodel.h"
    #include "FMTconstraint.h"
    #include "FMTfreeexceptionhandler.h"
    #include "FMTexception.h"
#endif

int main()
	{
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primarylocation = folder + "TWD_land.pri";
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	modelparser.seterrorstowarnings(errors);
	const std::vector<std::string>scenarios(1,"LP");
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
	Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
	optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, 5);
	optimizationmodel.FMTmodel::setparameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
	optimizationmodel.FMTmodel::setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	optimizationmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 10);
	Core::FMToutput out;
	for (const Core::FMToutput& output : optimizationmodel.getoutputs())
		{
			if (output.getname() == "OVOLREC")
			{
				out = output;
				break;
			}
		}
	Models::FMTlpmodel lowerboundmodel, upperboundmodel;
	lowerboundmodel = upperboundmodel = optimizationmodel;
	//Select the yields to change.
	std::vector<std::string>yieldstochange;
	yieldstochange.push_back("VOLUMETOTAL");
	//Change the Yields...
	const Core::FMTyields baseyields = optimizationmodel.getyields();
	const Core::FMTyields lowerboundyields = baseyields.getfromfactor(0.9, yieldstochange);//-10%
	lowerboundmodel.setyields(lowerboundyields);
	const Core::FMTyields upperboundyields = baseyields.getfromfactor(1.1, yieldstochange);//+10%
	upperboundmodel.setyields(upperboundyields);
	//Solve all models
	lowerboundmodel.doplanning(true);
	upperboundmodel.doplanning(true);
	optimizationmodel.doplanning(true);
	const double basevalue = optimizationmodel.getoutput(out, 1, Core::FMToutputlevel::totalonly).at("Total");
	const double lowervalue = lowerboundmodel.getoutput(out, 1, Core::FMToutputlevel::totalonly).at("Total");
	const double uppervalue = upperboundmodel.getoutput(out, 1, Core::FMToutputlevel::totalonly).at("Total");
	Logging::FMTdefaultlogger() << "LOWER: "<< lowervalue << "BASE: "<< basevalue << "UPPER: "<< uppervalue << "\n";
	if ((lowervalue>uppervalue)||(basevalue<lowervalue)||(basevalue > uppervalue))
	{
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
			"presolvetest", __LINE__, primarylocation);
	}
	#endif
	return 0;
	}

