#include <vector>
#ifdef FMTWITHGDAL
#include "FMTlpmodel.h"
#include "FMTsesmodel.h"
#include "FMTmodelparser.h"
#include "FMTareaparser.h"
#include "FMTforest.h"
#include "FMTversion.h"
#include "FMToutput.h"
#include "FMTfreeexceptionhandler.h"
#endif

int main(int argc, char* argv[])
{
#ifdef FMTWITHGDAL
	const std::string primarylocation = argv[1];
	const std::string scenario = argv[2];
	const std::string maplocation = argv[3];
	/*const std::string maplocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/02471/Carte/PC_9377_COS_02471_08_09_2020t_feu_TBE.shp";
	const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/02471/PC_9377_U02471_4_Vg2_2023_vRP1f.pri";
	const std::string scenario = "spatial";*/
	const std::string writelocation = "../../tests/testsesevaluation/";
	const int length = 6;
	Parser::FMTmodelparser mparser;
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
	errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
	errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
	mparser.seterrorstowarnings(errors);
	const std::vector<std::string>scenarios(1, scenario);
	const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
	Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
	optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, length);
	if (optimizationmodel.doplanning(true))
	{
		std::cout << "OBJECTIVE VALUE " << optimizationmodel.getObjValue() << "\n";
		Models::FMTsesmodel simmodel(optimizationmodel);
		std::vector<Core::FMTtransition>newtransitions;
		for (const Core::FMTtransition& transition : simmodel.gettransitions())
		{
			newtransitions.push_back(transition.single());
		}
		simmodel.settransitions(newtransitions);
		Parser::FMTareaparser areaparser;
		const Spatial::FMTforest forest = areaparser.vectormaptoFMTforest(maplocation, 1420, simmodel.getthemes(), "AGE", "SUPERFICIE", 1, 0.0001, "", 0.0,"", false);
		simmodel.setinitialmapping(forest);
		for (int period = 1; period <= length; ++period)
		{
			simmodel.greedyreferencebuild(optimizationmodel.getsolution(period),
				100,
				0,
				0.1);
			const Spatial::FMTspatialschedule spschedule = simmodel.getspschedule();
			for (const Core::FMTconstraint& constraint : simmodel.getconstraints())
			{
				if (constraint.isspatial())
				{
				if (spschedule.getconstraintevaluation(constraint, simmodel) > 0)
					{
					std::cout << std::string(constraint) << "\n";
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value on " + std::string(constraint),
						"testsesevaluation", __LINE__, primarylocation);
					}
				
				double lower = 0;
				double upper = 0;
				constraint.getbounds(lower, upper);
				if (lower>0 && lower < std::numeric_limits<double>::max())
				{
					Core::FMTconstraint bindingconstraint(constraint);
					lower += 10000000000;
					bindingconstraint.setrhs(lower,upper);
					const double penalty = spschedule.getconstraintevaluation(bindingconstraint, simmodel);
					if (penalty == 0)
					{
						std::cout << std::string(bindingconstraint) << "\n";
						Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value on " + std::string(constraint),
							"testsesevaluation", __LINE__, primarylocation);
					}else {
						std::cout << std::string(bindingconstraint)<< "Penalty of "<< penalty << "\n";
					}
				}
				
				}

				}
			}
		

			}
		#endif
		return 0;
}
        
