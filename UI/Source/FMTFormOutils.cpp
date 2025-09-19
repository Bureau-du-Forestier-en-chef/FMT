#include "stdafx.h"
#include <string>
#include "FMTconstraint.h"
#include "FMTmodelparser.h"
#include "FMToutput.h"
#include "FMTparser.h"
#include "FMTlpmodel.h"
#include "FMTareaparser.h"
#include "FMTscheduleparser.h"
#include <msclr\marshal_cppstd.h>
#include "FMTForm.h"
#include "FMTexceptionhandlerwarning.h"
#include "FmtFormCache.h"
#include "Tools.h"

void Wrapper::FMTForm::raisefromcatch(std::string text,
	const std::string& method, const int& line, const std::string& fil)
{
	Wrapper::FMTexceptionhandlerwarning* exhandler = Cache->getformhandler();
	const std::string errorstack = exhandler->geterrorstack(text, method, line, fil);
	FeedBack("*************************************************************", gcnew System::EventArgs());
	const std::string message = "FMT - ERROR " + errorstack;
	FeedBack(gcnew System::String(message.c_str()), gcnew System::EventArgs());
	exhandler->tryfileopener(errorstack);
}
 
System::Collections::Generic::List<System::String^>^ Wrapper::FMTForm::ObtenirListeContraintes(
	int indexScenario)
{
	System::Collections::Generic::List<System::String^>^ retour = gcnew System::Collections::Generic::List<System::String^>();
	try
	{
		//FeedBack("FMT Event Spatialy Explicit Simulation c++ - > ObtientListeContraintes", gcnew System::EventArgs());
		for (const Core::FMTconstraint& constraint : ObtenirArrayContraintes(indexScenario))
		{
			retour->Add(gcnew System::String(std::string(constraint).c_str()));
		}
	}
	catch (...)
	{
		retour->Clear();
		raisefromcatch("", "Wrapper::FMTForm::ObtenirListeContraintes", __LINE__, __FILE__);
	}

	return retour;
}

std::vector<Core::FMTconstraint> Wrapper::FMTForm::ObtenirArrayContraintes(
	int indexScenario)
{
	std::vector<Core::FMTconstraint> retour;
	try
	{
		//FeedBack("FMT Event Spatialy Explicit Simulation c++ - > ObtenirArrayContraintes", gcnew System::EventArgs());
		retour = Cache->getmodel(indexScenario).getconstraints();
	}
	catch (...)
	{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::ObtenirArrayContraintes", __LINE__, __FILE__);
	}

	return retour;
}

std::vector<Core::FMTconstraint> Wrapper::FMTForm::ObtenirArrayContraintesSelectionnees(
	std::vector<Core::FMTconstraint> contraitesBase, 
	System::Collections::Generic::List<System::String^>^ contraintesSelection)
{
	std::vector<Core::FMTconstraint> retour;
	try
	{
		//FeedBack("FMT Event Spatialy Explicit Simulation c++ - > ObtenirArrayContraintesSelectionnees", gcnew System::EventArgs());
		for (const Core::FMTconstraint& contrainte : contraitesBase)
		{
			for each (System::String^ selection in contraintesSelection)
			{
				std::string stdSelection = msclr::interop::marshal_as<std::string>(selection);
				std::string stdContraite = std::string(contrainte);
				stdSelection.erase(std::remove(stdSelection.begin(), stdSelection.end(), '\n'), stdSelection.cend());
				stdContraite.erase(std::remove(stdContraite.begin(), stdContraite.end(), '\n'), stdContraite.cend());
				stdSelection.erase(std::remove(stdSelection.begin(), stdSelection.end(), '\r'), stdSelection.cend());
				stdContraite.erase(std::remove(stdContraite.begin(), stdContraite.end(), '\r'), stdContraite.cend());
				System::String^ selectiontexte = (gcnew System::String(stdSelection.c_str()))->TrimEnd();
				System::String^ contraiteTexte = (gcnew System::String(stdContraite.c_str()))->TrimEnd();
	
				if (selectiontexte == contraiteTexte)
				{
					retour.push_back(contrainte);
				}
			}
		}
	}
	catch (...)
	{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::ObtenirArrayContraintesSelectionnees", __LINE__, __FILE__);
	}

	return retour;
}

std::vector<Core::FMToutput> Wrapper::FMTForm::ObtenirArrayOutputsSelectionnees(std::vector<Core::FMToutput> outputsBase, System::Collections::Generic::List<System::String^>^ outputsSelection)
{
	std::vector<Core::FMToutput> retour;
	try
	{
		for (const Core::FMToutput& fmtOutput : outputsBase)
		{
			if (outputsSelection->Contains(gcnew System::String(fmtOutput.getname().c_str())))
			{
				retour.push_back(fmtOutput);
			}
		}
	}
	catch (...)
	{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::ObtenirArrayOutputsSelectionnees", __LINE__, __FILE__);
	}

	return retour;

}

Core::FMToutput Wrapper::FMTForm::ObtenirOutputSelectionnee(std::vector<Core::FMToutput> outputsBase, System::String^ outputSelection)
{
	Core::FMToutput retour;
	try
	{
		for (const Core::FMToutput& fmtOutput : outputsBase)
		{
			if (outputSelection == gcnew System::String(fmtOutput.getname().c_str()))
			{
				retour = fmtOutput;
				break;
			}
		}
	}
	catch (...)
	{
		Cache->getformhandler()->raisefromcatch("", "Wrapper::FMTForm::ObtenirOutputSelectionnee", __LINE__, __FILE__);
	}

	return retour;

}


System::Collections::Generic::List<System::String^>^ Wrapper::FMTForm::ObtenirListeExtentionsSorties()
{
	System::Collections::Generic::List<System::String^>^ retour = gcnew System::Collections::Generic::List<System::String^>();

	try
	{		
		Parser::FMTmodelparser Modelparser;
		const std::vector<std::vector<std::string>> listeExtensions = Modelparser.getGDALvectordriverextensions();
		const std::vector<std::string> listeDrivers = Modelparser.getGDALvectordrivernames();

		for (int index = 0; index < listeExtensions.size(); ++index)
		{
			for (int indexExtension = 0; indexExtension < listeExtensions.at(index).size(); ++indexExtension)
			{
				retour->Add(gcnew System::String((listeDrivers.at(index) + "|*." + listeExtensions.at(index).at(indexExtension)).c_str()));
			}
		}
	}
	catch (...)
	{
		retour = gcnew System::Collections::Generic::List<System::String^>();
		raisefromcatch("", "Wrapper::FMTForm::ObtenirListeExtentionsSorties", __LINE__, __FILE__);
	}

	return retour;
}

System::Collections::Generic::List<int>^ Wrapper::FMTForm::ObtenirListeSolvers()
{
	System::Collections::Generic::List<int>^ retour = gcnew System::Collections::Generic::List<int>();

	try
	{
		for (Models::FMTsolverinterface solver : Models::FMTsrmodel::getavailablesolverinterface())
		{
			retour->Add(static_cast<int>(solver));
		}
	}
	catch (...)
	{
		retour = gcnew System::Collections::Generic::List<int>();
		raisefromcatch("", "Wrapper::FMTForm::ObtenirListeSolvers", __LINE__, __FILE__);
	}

	return retour;
}

void Wrapper::FMTForm::InscrireLigneFichierTexte(
	System::String^ nomFichier, 
	System::String^ message, 
	bool indicateurFeedback,
	bool nouveaufichier)
{
	try
	{
	if (indicateurFeedback)
	{
		FeedBack(message, gcnew System::EventArgs());
	}

	const std::string ficher = msclr::interop::marshal_as<std::string>(nomFichier);
	std::ofstream fichierResultat;
	/*if (!(bool)std::ifstream(ficher))
	{
		fichierResultat = std::ofstream(ficher);
	}*/
	if (!nouveaufichier)
	{
		fichierResultat.open(ficher, std::ios_base::app);
	}
	else {
		fichierResultat.open(ficher);
	}
	
	fichierResultat << msclr::interop::marshal_as<std::string>(message + "\n");
	fichierResultat.close();
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::InscrireLigneFichierTexte", __LINE__, __FILE__);
	}
}

System::Collections::Generic::List<System::String^>^ Wrapper::FMTForm::ObtenirListeOutputs(
	int indexScenario)
{
	System::Collections::Generic::List<System::String^>^ retour = gcnew System::Collections::Generic::List<System::String^>();

	try
	{
		if (!Cache->empty())
		{
			for (const Core::FMToutput& output : Cache->getmodel(indexScenario).getoutputs())
			{
				retour->Add(gcnew System::String(std::string(output.getname()).c_str()));
			}
		}
	}
	catch (...)
	{
		retour = gcnew System::Collections::Generic::List<System::String^>();
		raisefromcatch("", "Wrapper::FMTForm::ObtenirListeOutputs", __LINE__, __FILE__);
	}

	return retour;
}

int Wrapper::FMTForm::ObtenirNombreThemes(
	int indexScenario)
{
	int retour = 0;

	try
	{
		retour = Cache->getmodel(indexScenario).getthemes().size();
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::ObtenirNombreThemes", __LINE__, __FILE__);
	}

	return retour;
}

int Wrapper::FMTForm::ObtenirNombrePeriodes(
	System::String^ nomFichierPri,
	int indexScenario)
{
	int retour = 0;

	try
	{
		retour = ObtenirSEQ(nomFichierPri, indexScenario).back().getperiod();
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::ObtenirNombrePeriodes", __LINE__, __FILE__);
	}

	return retour;
}

std::vector<Core::FMTschedule> Wrapper::FMTForm::ObtenirSEQ(
	System::String^ nomFichierPri,
	int indexScenario)
{
	std::vector<Core::FMTschedule> retour;
	try
	{
		Parser::FMTscheduleparser schedulerparser;
		Parser::FMTmodelparser Modelparser;
		std::vector<Models::FMTmodel> models;
		models.push_back(Cache->getmodel(indexScenario));
		std::vector<Core::FMTschedule> liste = Modelparser.readschedules(msclr::interop::marshal_as<std::string>(nomFichierPri), models).at(0);
		if (liste.size() > 0)
		{
			retour = liste;
		}
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::ObtenirSEQ", __LINE__, __FILE__);
	}

	return retour;
}

System::Collections::Generic::List<System::String^>^ Wrapper::FMTForm::getActionsNames(int p_index)
{
	System::Collections::Generic::List<System::String^>^ actionsNames = gcnew System::Collections::Generic::List<System::String^>();
	try
	{
		if (Cache->empty())
		{
			throw std::out_of_range("Invalid model index");
		}
		const Models::FMTmodel MODEL = Cache->getmodel(p_index);
		const std::vector<Core::FMTaction> ACTIONS = MODEL.getactions();
		// On it�re sur les actions pour obbtenir leurs noms
		for (int i = 0; i < ACTIONS.size(); ++i)
		{
			System::String^ name = msclr::interop::marshal_as<System::String^>(ACTIONS[i].getname());
			actionsNames->Add(name);
		}
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTModelWrapper::getActionsNames", __LINE__, __FILE__);
	}
	return actionsNames;
}
System::Collections::Generic::List<System::String^>^ Wrapper::FMTForm::getAggregates(int p_modelIndex)
{
	System::Collections::Generic::List<System::String^>^ aggregatesList = gcnew System::Collections::Generic::List<System::String^>();
	try
	{
		if (Cache->empty())
		{
			throw std::out_of_range("Invalid model index");
		}
		const Models::FMTmodel MODEL = Cache->getmodel(p_modelIndex);
		const std::vector<Core::FMTaction> ACTIONS = MODEL.getactions();
		std::set<std::string> uniqueAggregates;

		for (int i = 0; i < ACTIONS.size(); ++i)
		{
			std::vector<std::string> agg = ACTIONS[i].getaggregates();
			for (const auto& aggregate: agg)
			{
				uniqueAggregates.insert(aggregate);
			}
		}
		for (const auto& aggregate : uniqueAggregates)
		{
			aggregatesList->Add(gcnew System::String(aggregate.c_str()));
		}
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::getAggregates", __LINE__, __FILE__);
	}
	return aggregatesList;
}
System::Collections::Generic::List<System::String^>^ Wrapper::FMTForm::getYields(int p_index)
{
	// nullptr ici au cas o� �a rentre dans le catch
	System::Collections::Generic::List<System::String^>^ yieldsNamesConverted = gcnew System::Collections::Generic::List<System::String^>();
	try
	{
		// En entr� l'index du sc�nario???
		if (Cache->empty())
		{
			throw std::out_of_range("Invalid model index");
		}
		const Models::FMTmodel& MODEL = Cache->getmodel(p_index);

		const Core::FMTyields YIELDS = MODEL.getyields();

		std::vector<std::string> yieldsNames = YIELDS.getallyieldnames();

		for (int i = 0; i < yieldsNames.size(); ++i)
		{
			System::String^ convertedString = gcnew System::String(yieldsNames[i].c_str());
			yieldsNamesConverted->Add(convertedString);
		}
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::getYields", __LINE__, __FILE__);
	}
	return yieldsNamesConverted;
}

double Wrapper::FMTForm::getYield(int p_modelIndex, System::String^ p_mask, System::String^ p_yield, int p_age)
{
	double result = NULL;
	try
	{
		if (Cache->empty())
		{
			throw std::out_of_range("Invalid model index");
		}
		const std::string CONVERTEDSTRING = msclr::interop::marshal_as<std::string>(p_mask);
		const std::string CONVERTEDYIELD = msclr::interop::marshal_as<std::string>(p_yield);
		const Models::FMTmodel& MODEL = Cache->getmodel(p_modelIndex);

		result = FMTWrapperCore::Tools::getYield(MODEL, CONVERTEDSTRING, CONVERTEDYIELD, p_age);
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::getYield", __LINE__, __FILE__);
	}
	return result;
}

double Wrapper::FMTForm::getMaxAge(int p_modelIndex)
{
	double result = 0;
	try
	{
		if (Cache->empty())
		{
			throw std::out_of_range("Invalid model index");
		}
		const Models::FMTmodel MODEL = Cache->getmodel(p_modelIndex);

		result = static_cast<double>(FMTWrapperCore::Tools::getMaxAge(MODEL));

	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTModelWrapper::getActionsNames", __LINE__, __FILE__);
	}
	return result;
}

void Wrapper::FMTForm::ToFeedBack(const char* message)
{
	System::String^ newstr = gcnew System::String(message);
	System::String^ cleaned = newstr->Replace("\n", "");
	cleaned->Trim();
	if (cleaned->Length>0)
	{
		FeedBack(cleaned, gcnew System::EventArgs());
	}
}

bool Wrapper::FMTForm::validateMask(const int p_modelIndex, System::String^ p_mask)
{
	bool result = false;
	try
	{
		if (Cache->empty())
		{
			throw std::out_of_range("Invalid model index");
		}
		// changer le str de C# en str applicable dans C++
		std::string MASK = msclr::interop::marshal_as<std::string>(p_mask);
		// On va chercher le mod�le
		const Models::FMTmodel& MODEL = Cache -> getmodel(p_modelIndex);
		// On va chercher tous les th�mes dans le mod�le
		const std::vector<Core::FMTtheme> THEMES = MODEL.getthemes();
		// On call validate
		result = Core::FMTtheme::validate(THEMES, MASK);
	}

	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm::validateMask", __LINE__, __FILE__);
	}
	return result;
}
System::Collections::Generic::List<System::String^>^ Wrapper::FMTForm::getAllMasks(int p_modelIndex, const int p_periods, System::Collections::Generic::List<int>^ p_themesNumbers, System::String^ p_cheminRasters)
{
	System::Collections::Generic::List<System::String^>^ result = gcnew System::Collections::Generic::List<System::String^>();
	try
	{
		if (Cache->empty())
		{
			throw std::out_of_range("Invalid model index");
		}
		const Models::FMTmodel& MODEL = Cache->getmodel(p_modelIndex);
		std::vector<int> themes;
		for each(int theme in p_themesNumbers)
		{
			themes.push_back(theme);
		}

		std::string rasterPath = msclr::interop::marshal_as<std::string>(p_cheminRasters);

		std::set<std::string> masks = FMTWrapperCore::Tools::getAllMasks(MODEL, p_periods, themes, rasterPath);
		for (const std::string& mask : masks)
		{
			result->Add(gcnew System::String(mask.c_str()));
		}
	}
	catch (...)
	{
		raisefromcatch("", "Wrapper::FMTForm:getAllMasks", __LINE__, __FILE__);
	}
	return result;
}