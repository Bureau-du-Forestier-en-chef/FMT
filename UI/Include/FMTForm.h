#ifndef FMTFORM_HEADER
#define FMTFORM_HEADER
#include "stdafx.h"
#include <vector>
#include <string>

namespace Core {
	class FMTSchedule;
	class FMTConstraint;
	class FMTOutput;
}

namespace Models {
	class FMTSeModel;
}
namespace FMTWrapperCore {
	struct SESResults;
}
namespace Wrapper
{
	// DocString: FMTForm
	/**
	@brief Main .NET wrapper interface for the FMT framework.

	This class exposes model loading, planning, optimization,
	simulation, scheduling, rasterization and analysis features to
	managed client applications.

	Logging and exception handling are delegated to the wrapper
	infrastructure through FMTFormCache.
	*/
	public ref class FMTForm
	{
	public:

		// DocString: FMTForm::FeedBack
		/**
		@brief Event raised when a feedback or log message is available.
		*/
		event System::EventHandler^ FeedBack;

		// DocString: FMTForm::RetourJson
		/**
		@brief Event raised when JSON formatted results are available.
		*/
		event System::EventHandler^ RetourJson;

		// DocString: FMTForm::Cache_AjouterScenarios
		/**
		@brief Loads a scenario and adds the resulting model to the cache.

		@param[in] fichierPriSystem Path to the project file.
		@param[in] scenarioSystem Name of the scenario to load.

		@return True if the scenario was loaded successfully.
		*/
		bool Cache_AjouterScenarios(
			System::String^ fichierPriSystem,
			System::String^ scenarioSystem);

		// DocString: FMTForm::Cache_Vider
		/**
		@brief Removes all models from the cache.
		*/
		void Cache_Vider();

		// DocString: FMTForm::Cache_InitialiserModelParser
		/**
		@brief Configures warning handling for model parsing.

		This method is retained for backward compatibility and forwards
		its arguments to SetErrorsToWarnings().

		@param[in] listeWarnings List of exception identifiers to treat as warnings.
		@param[in] maxWarnings Maximum number of warnings before warning
		messages are silenced.
		*/
		void Cache_InitialiserModelParser(
			System::Collections::Generic::List<int>^ listeWarnings,
			int maxWarnings);

		// DocString: FMTForm::SetErrorsToWarnings
		/**
		@brief Configures exceptions that should be treated as warnings.

		@param[in] listeWarnings List of exception identifiers to treat as warnings.
		@param[in] maxWarnings Maximum number of warnings before warning
		messages are silenced.
		*/
		void SetErrorsToWarnings(
			System::Collections::Generic::List<int>^ listeWarnings,
			int maxWarnings);

		// DocString: FMTForm::Cache_AssignerNomLogger
		/**
		@brief Initializes the logger.

		@param[in] nomFichierLogger Path to the log file.
		*/
		void Cache_AssignerNomLogger(
			System::String^ nomFichierLogger);

		// DocString: FMTForm::RecoverFromCrash
		/**
		@brief Rebuilds logging and exception handling resources after a crash.

		A new managed callback delegate is created and the logger and
		exception handler are reconstructed using the configuration
		stored in the cache.
		*/
		void RecoverFromCrash();

		// DocString: FMTForm::CloseLogger
		/**
		@brief Closes the active logger.
		*/
		void CloseLogger();

		// DocString: FMTForm::Cache_EnleverModel
		/**
		@brief Removes a model from the cache.

		@param[in] indexScenario Index of the model to remove.

		@return True if the model was successfully removed.
		*/
		bool Cache_EnleverModel(
			int indexScenario);

		// DocString: FMTForm::ObtenirListeContraintes
		/**
		@brief Returns the list of constraints available in a model.

		@param[in] indexScenario Model index.

		@return List of constraint names.
		*/
		System::Collections::Generic::List<System::String^>^ ObtenirListeContraintes(
			int indexScenario);

		// DocString: FMTForm::ObtenirListeOutputs
		/**
		@brief Returns the list of outputs available in a model.

		@param[in] indexScenario Model index.

		@return List of output names.
		*/
		System::Collections::Generic::List<System::String^>^ ObtenirListeOutputs(
			int indexScenario);

		// DocString: FMTForm::ObtenirListeExtentionsSorties
		/**
		@brief Returns the list of supported output file extensions.

		@return List of output extensions.
		*/
		System::Collections::Generic::List<System::String^>^ ObtenirListeExtentionsSorties();

		// DocString: FMTForm::ObtenirListeSolvers
		/**
		@brief Returns the list of available optimization solvers.

		@return List of solver identifiers.
		*/
		System::Collections::Generic::List<int>^ ObtenirListeSolvers();

		// DocString: FMTForm::ObtenirNombreThemes
		/**
		@brief Returns the number of themes defined in a model.

		@param[in] indexScenario Model index.

		@return Number of themes.
		*/
		int ObtenirNombreThemes(
			int indexScenario);

		// DocString: FMTForm::ObtenirNombrePeriodes
		/**
		@brief Returns the number of periods defined for a scenario.

		@param[in] nomFichierPri Project file path.
		@param[in] indexScenario Model index.

		@return Number of planning periods.
		*/
		int ObtenirNombrePeriodes(
			System::String^ nomFichierPri,
			int indexScenario);
		// DocString: FMTForm::Plannification
		/**
		@brief Runs a strategic planning optimization.

		A planning model is solved using the selected solver and the
		generated outputs are written to the specified destination.

		@param[in] fichierPri Project file path.
		@param[in] scenarios List of scenario identifiers.
		@param[in] solver Solver identifier.
		@param[in] period Planning horizon.
		@param[in] nbreProcessus Number of worker processes.
		@param[in] outputs Outputs to generate.
		@param[in] outputLevel Output verbosity level.
		@param[in] etanduSortiesMin First output period.
		@param[in] etanduSortiesMax Last output period.
		@param[in] cheminSorties Output directory.
		@param[in] providerGdal GDAL provider to use.
		@param[in] playback Playback options.

		@return True if the planning operation completed successfully.
		*/
		bool Plannification(
			System::String^ fichierPri,
			System::Collections::Generic::List<int>^ scenarios,
			int solver,
			int period,
			int nbreProcessus,
			System::Collections::Generic::List<System::String^>^ outputs,
			int outputLevel,
			int etanduSortiesMin,
			int etanduSortiesMax,
			System::String^ cheminSorties,
			System::String^ providerGdal,
			System::Collections::Generic::List<bool>^ playback);

		// DocString: FMTForm::Replanification
		/**
		@brief Runs a replanning workflow using strategic, stochastic and tactical scenarios.

		@param[in] indexScenStrategique Strategic scenario index.
		@param[in] indexScenStochastique Stochastic scenario index.
		@param[in] indexScenTactique Tactical scenario index.
		@param[in] solver Solver identifier.
		@param[in] period Planning horizon.
		@param[in] periodReplannif Replanning period.
		@param[in] variabilite Variability factor.
		@param[in] nbreProcessus Number of worker processes.
		@param[in] nombreReplicasMin Minimum number of replicas.
		@param[in] nombreReplicasMax Maximum number of replicas.
		@param[in] outputs Outputs to generate.
		@param[in] outputLevel Output verbosity level.
		@param[in] cheminSorties Output directory.
		@param[in] providerGdal GDAL provider to use.
		@param[in] taskLogLevel Logging level for worker tasks.
		@param[in] indProduireSolution Indicates whether a solution should be produced.
		@param[in] p_writeSchedule Indicates whether schedules should be written.

		@return True if the replanning operation completed successfully.
		*/
		bool Replanification(
			int indexScenStrategique,
			int indexScenStochastique,
			int indexScenTactique,
			int solver,
			int period,
			int periodReplannif,
			double variabilite,
			int nbreProcessus,
			int nombreReplicasMin,
			int nombreReplicasMax,
			System::Collections::Generic::List<System::String^>^ outputs,
			int outputLevel,
			System::String^ cheminSorties,
			System::String^ providerGdal,
			int taskLogLevel,
			bool indProduireSolution,
			bool p_writeSchedule);

		// DocString: FMTForm::SimulationSpatialeExplicite
		/**
		@brief Runs an explicit spatial simulation.

		@param[in] fichierPri Project file path.
		@param[in] cheminRasters Raster directory.
		@param[in] scenario Scenario identifier.
		@param[in] contraintes Selected constraints.
		@param[in] periodes Number of periods to simulate.
		@param[in] greedySearch Greedy search mode.
		@param[in] outputs Outputs to generate.
		@param[in] indicateurStanlock Indicates whether spatial locking is enabled.
		@param[in] outputLevel Output verbosity level.
		@param[in] etanduSortiesMin First output period.
		@param[in] etanduSortiesMax Last output period.
		@param[in] cheminSorties Output directory.
		@param[in] indGenererEvents Indicates whether event layers should be generated.
		@param[in] indSortiesSpatiales Indicates whether spatial outputs should be generated.
		@param[in] providerGdal GDAL provider to use.
		@param[in] indCarbon Indicates whether carbon outputs are enabled.
		@param[in] predictoryields Predictor yields used during simulation.
		@param[in] growththemes Growth themes used during simulation.

		@return True if the simulation completed successfully.
		*/
		bool SimulationSpatialeExplicite(
			System::String^ fichierPri,
			System::String^ cheminRasters,
			int scenario,
			System::Collections::Generic::List<System::String^>^ contraintes,
			int periodes,
			int greedySearch,
			System::Collections::Generic::List<System::String^>^ outputs,
			bool indicateurStanlock,
			int outputLevel,
			int etanduSortiesMin,
			int etanduSortiesMax,
			System::String^ cheminSorties,
			bool indGenererEvents,
			bool indSortiesSpatiales,
			System::String^ providerGdal,
			bool indCarbon,
			System::Collections::Generic::List<System::String^>^ predictoryields,
			System::Collections::Generic::List<int>^ growththemes);

		// DocString: FMTForm::OptimisationSpatialeExplicite
		/**
		@brief Runs an explicit spatial optimization.

		@param[in] fichierPri Project file path.
		@param[in] cheminRasters Raster directory.
		@param[in] scenario Scenario identifier.
		@param[in] contraintes Selected constraints.
		@param[in] periodes Number of periods.
		@param[in] p_MaxMoves Maximum number of moves.
		@param[in] p_MaxAcceptedMoves Maximum number of accepted moves.
		@param[in] p_MaxCycleMoves Maximum number of cycle moves.
		@param[in] outputs Outputs to generate.
		@param[in] indicateurStanlock Indicates whether spatial locking is enabled.
		@param[in] outputLevel Output verbosity level.
		@param[in] etanduSortiesMin First output period.
		@param[in] etanduSortiesMax Last output period.
		@param[in] cheminSorties Output directory.
		@param[in] indGenererEvents Indicates whether event layers should be generated.
		@param[in] indSortiesSpatiales Indicates whether spatial outputs should be generated.
		@param[in] providerGdal GDAL provider to use.

		@return True if the optimization completed successfully.
		*/
		bool OptimisationSpatialeExplicite(
			System::String^ fichierPri,
			System::String^ cheminRasters,
			int scenario,
			System::Collections::Generic::List<System::String^>^ contraintes,
			int periodes,
			int p_MaxMoves,
			int p_MaxAcceptedMoves,
			int p_MaxCycleMoves,
			System::Collections::Generic::List<System::String^>^ outputs,
			bool indicateurStanlock,
			int outputLevel,
			int etanduSortiesMin,
			int etanduSortiesMax,
			System::String^ cheminSorties,
			bool indGenererEvents,
			bool indSortiesSpatiales,
			System::String^ providerGdal);

		// DocString: FMTForm::InitialAreaVariability
		/**
		@brief Computes initial area variability metrics.

		@param[in] fichierPri Project file path.
		@param[in] scenario Scenario identifier.
		@param[in] solver Solver identifier.
		@param[in] contraintes Selected constraints.
		@param[in] period Planning period.
		@param[in] outputs Outputs to generate.
		@param[in] outputLevel Output verbosity level.
		@param[in] etanduSortiesMin First output period.
		@param[in] etanduSortiesMax Last output period.
		@param[in] cheminSorties Output directory.
		@param[in] providerGdal GDAL provider to use.
		@param[in] ListeInformations Additional information to collect.

		@return True if the computation completed successfully.
		*/
		bool InitialAreaVariability(
			System::String^ fichierPri,
			int scenario,
			int solver,
			System::Collections::Generic::List<System::String^>^ contraintes,
			int period,
			System::Collections::Generic::List<System::String^>^ outputs,
			int outputLevel,
			int etanduSortiesMin,
			int etanduSortiesMax,
			System::String^ cheminSorties,
			System::String^ providerGdal,
			System::Collections::Generic::List<System::Collections::Generic::List<System::String^>^>^ ListeInformations);

		// DocString: FMTForm::Raterisation
		/**
		@brief Converts vector data into raster data.

		@param[in] fichierPri Project file path.
		@param[in] scenario Scenario identifier.
		@param[in] fichierShp Input shapefile.
		@param[in] repertoireSortie Output directory.
		@param[in] resolution Target raster resolution.
		@param[in] nomChampAge Age field name.
		@param[in] nomChampSuperficie Area field name.
		@param[in] nomChampStanlock Spatial lock field name.

		@return True if rasterization completed successfully.
		*/
		bool Raterisation(
			System::String^ fichierPri,
			int scenario,
			System::String^ fichierShp,
			System::String^ repertoireSortie,
			int resolution,
			System::String^ nomChampAge,
			System::String^ nomChampSuperficie,
			System::String^ nomChampStanlock);
		// DocString: FMTForm::OperatingAreaScheduling
		/**
		@brief Runs an operating area scheduling optimization.

		@param[in] fichierPri Project file path.
		@param[in] scenario Scenario identifier.
		@param[in] fichierShp Input shapefile.
		@param[in] solver Solver identifier.
		@param[in] nombrePeriodes Number of planning periods.
		@param[in] nombreThread Number of worker threads.
		@param[in] numeroTheme Theme used to define operating areas.
		@param[in] tempsMaximum Maximum execution time.
		@param[in] nombreIteration Maximum number of iterations.
		@param[in] nomChampAge Age field name.
		@param[in] nomChampSuperficie Area field name.
		@param[in] nomChampStanlock Spatial lock field name.
		@param[in] cheminParametres Operating area parameter file.
		@param[in] nomFichierResultat Output file path.
		@param[in] periodeMiseAjour Status update period.
		@param[in] returnTimeOutput File used to record execution times.

		@return True if scheduling completed successfully.
		*/
		bool OperatingAreaScheduling(
			System::String^ fichierPri,
			int scenario,
			System::String^ fichierShp,
			int solver,
			int nombrePeriodes,
			int nombreThread,
			int numeroTheme,
			int tempsMaximum,
			int nombreIteration,
			System::String^ nomChampAge,
			System::String^ nomChampSuperficie,
			System::String^ nomChampStanlock,
			System::String^ cheminParametres,
			System::String^ nomFichierResultat,
			int periodeMiseAjour,
			System::String^ returnTimeOutput);

		// DocString: FMTForm::aggregateAllActions
		/**
		@brief Aggregates actions in a schedule.

		The generated project contains actions aggregated according to
		the specified aggregate definitions and ordering rules.

		@param[in] p_modelIndex Model index.
		@param[in] p_aggregates Aggregate definitions.
		@param[in] p_order Aggregation order.
		@param[in] p_schedulePri Schedule project file.
		@param[in] p_outputDirPath Output directory.
		@param[in] p_scenario_name Scenario name.
		@param[in] p_pri_name Project name.

		@return True if the aggregation completed successfully.
		*/
		bool aggregateAllActions(
			const int p_modelIndex,
			System::Collections::Generic::List<System::String^>^ p_aggregates,
			System::Collections::Generic::List<System::String^>^ p_order,
			System::String^ p_schedulePri,
			System::String^ p_outputDirPath,
			System::String^ p_scenario_name,
			System::String^ p_pri_name);

		// DocString: FMTForm::splitActions
		/**
		@brief Splits actions using thematic masks.

		@param[in] p_modelIndex Model index.
		@param[in] p_schedulePri Schedule project file.
		@param[in] p_splitted Actions to split.
		@param[in] p_splitted_mask Masks used to perform the split.
		@param[in] p_outputDirPath Output directory.
		@param[in] p_scenario_name Scenario name.
		@param[in] p_pri_name Project name.

		@return True if the operation completed successfully.
		*/
		bool splitActions(
			const int p_modelIndex,
			System::String^ p_schedulePri,
			System::Collections::Generic::List<System::String^>^ p_splitted,
			System::Collections::Generic::List<System::String^>^ p_splitted_mask,
			System::String^ p_outputDirPath,
			System::String^ p_scenario_name,
			System::String^ p_pri_name);

		// DocString: FMTForm::getActionsNames
		/**
		@brief Returns the names of all actions defined in a model.

		@param[in] p_modelIndex Model index.

		@return List of action names.
		*/
		System::Collections::Generic::List<System::String^>^ getActionsNames(
			int p_modelIndex);

		// DocString: FMTForm::getAggregates
		/**
		@brief Returns the aggregate names defined in a model.

		@param[in] p_modelIndex Model index.

		@return List of aggregate names.
		*/
		System::Collections::Generic::List<System::String^>^ getAggregates(
			int p_modelIndex);

		// DocString: FMTForm::getYields
		/**
		@brief Returns the names of all yields defined in a model.

		@param[in] p_modelIndex Model index.

		@return List of yield names.
		*/
		System::Collections::Generic::List<System::String^>^ getYields(
			int p_modelIndex);
		// DocString: FMTForm::getYield
		/**
		@brief Evaluates a yield value for a given mask and age.

		@param[in] p_modelIndex Model index.
		@param[in] p_mask Thematic mask used for the evaluation.
		@param[in] p_yield Yield name.
		@param[in] p_age Evaluation age.

		@return Yield value.
		*/
		double getYield(
			int p_modelIndex,
			System::String^ p_mask,
			System::String^ p_yield,
			int p_age);

		// DocString: FMTForm::getMaxAge
		/**
		@brief Returns the maximum age represented in a model.

		@param[in] p_modelIndex Model index.

		@return Maximum age.
		*/
		double getMaxAge(
			int p_modelIndex);

		// DocString: FMTForm::buildAction
		/**
		@brief Builds an action from a target yield specification.

		@param[in] p_modelIndex Model index.
		@param[in] p_actionName Action name.
		@param[in] p_targetYield Target yield expression.
		@param[in] p_schedulePri Schedule project file.
		@param[in] p_outputDirPath Output directory.
		@param[in] p_scenario_name Scenario name.
		@param[in] p_pri_name Project name.

		@return True if the action was successfully generated.
		*/
		bool buildAction(
			const int p_modelIndex,
			System::String^ p_actionName,
			System::String^ p_targetYield,
			System::String^ p_schedulePri,
			System::String^ p_outputDirPath,
			System::String^ p_scenario_name,
			System::String^ p_pri_name);

		// DocString: FMTForm::getAttributes
		/**
		@brief Returns the attributes associated with a theme.

		@param[in] p_modelIndex Model index.
		@param[in] p_themeIndex Theme index.

		@return List of attribute values associated with the theme.
		*/
		System::Collections::Generic::List<System::String^>^ getAttributes(
			const int p_modelIndex,
			const int p_themeIndex);

		// DocString: FMTForm::validateMask
		/**
		@brief Validates a thematic mask.

		@param[in] p_modelIndex Model index.
		@param[in] p_mask Mask to validate.

		@return True if the mask is valid.
		*/
		bool validateMask(
			const int p_modelIndex,
			System::String^ p_mask);

		// DocString: FMTForm::getAllMasks
		/**
		@brief Returns all masks matching the supplied themes.

		@param[in] p_modelIndex Model index.
		@param[in] p_periods Number of periods considered.
		@param[in] p_themesNumber Theme identifiers used for filtering.
		@param[in] cheminRasters Raster directory.

		@return List of matching masks.
		*/
		System::Collections::Generic::List<System::String^>^ getAllMasks(
			const int p_modelIndex,
			const int p_periods,
			System::Collections::Generic::List<int>^ p_themesNumber,
			System::String^ cheminRasters);

		// DocString: FMTForm::writetoprojectfromcache
		/**
		@brief Writes all cached models to a project directory.

		The first cached model becomes the root project model and the
		remaining models are written as project scenarios.

		@param[in] p_destinationDirectory Destination directory.

		@return True if the project was successfully written.
		*/
		bool writetoprojectfromcache(
			System::String^ p_destinationDirectory);

		// DocString: FMTForm::getChangeLog
		/**
		@brief Returns the framework change log.

		@return Change log text.
		*/
		System::String^ getChangeLog();

		// DocString: FMTForm::getExceptionDescription
		/**
		@brief Returns the localized description of an FMT exception.

		@param[in] p_exceptionId Exception identifier.

		@return Description associated with the exception.
		*/
		System::String^ getExceptionDescription(int p_exceptionId);
	private:
		// DocString: FMTForm::ManagedFeed
		/**
		@brief Delegate type used to forward native log messages to managed code.
		*/
		delegate void ManagedFeed(const char*);

		// DocString: FMTForm::m_managedFeed
		/**
		@brief Managed delegate used by the logger callback system.
		*/
		ManagedFeed^ m_managedFeed;

		// DocString: FMTForm::m_unmanagedFeed
		/**
		@brief Native function pointer associated with the managed delegate.
		*/
		System::IntPtr m_unmanagedFeed;

		// DocString: FMTForm::_toFeedback
		/**
		@brief Raises the feedback event using a native message.
		...
		*/
		void _toFeedback(const char* p_message);

		// DocString: FMTForm::_raiseFromCatch
		/**
		@brief Converts an unhandled exception into an FMT exception.
		...
		*/
		void _raiseFromCatch(
			std::string p_text,
			const std::string& p_method,
			const int& p_line,
			const std::string& p_file);
		// DocString: FMTForm::_ObtenirArrayContraintes
		/**
		@brief Gets all constraints available in a model.

		@param[in] indexScenario Model index.

		@return Collection of model constraints.
		*/
		std::vector<Core::FMTConstraint> _ObtenirArrayContraintes(
			int indexScenario);

		// DocString: FMTForm::_ObtenirArrayContraintesSelectionnees
		/**
		@brief Filters constraints according to the user selection.

		@param[in] contraitesBase Source constraint collection.
		@param[in] contraintesSelection Selected constraint names.

		@return Collection containing only the selected constraints.
		*/
		std::vector<Core::FMTConstraint> _ObtenirArrayContraintesSelectionnees(
			std::vector<Core::FMTConstraint> contraitesBase,
			System::Collections::Generic::List<System::String^>^ contraintesSelection);

		// DocString: FMTForm::_ObtenirOutputSelectionnee
		/**
		@brief Retrieves an output definition by name.

		@param[in] outputsBase Available outputs.
		@param[in] outputSelection Selected output name.

		@return Matching output definition.
		*/
		Core::FMTOutput _ObtenirOutputSelectionnee(
			std::vector<Core::FMTOutput> outputsBase,
			System::String^ outputSelection);

		// DocString: FMTForm::_ObtenirArrayOutputsSelectionnees
		/**
		@brief Filters outputs according to the user selection.

		@param[in] outputsBase Available outputs.
		@param[in] outputsSelection Selected output names.

		@return Collection containing only the selected outputs.
		*/
		std::vector<Core::FMTOutput> _ObtenirArrayOutputsSelectionnees(
			std::vector<Core::FMTOutput> outputsBase,
			System::Collections::Generic::List<System::String^>^ outputsSelection);

		// DocString: FMTForm::_InscrireLigneFichierTexte
		/**
		@brief Writes a message to a text file.

		Optionally creates a new file and can forward the message through
		the feedback mechanism.

		@param[in] nomFichier File path.
		@param[in] message Message to write.
		@param[in] indicateurFeedback Indicates whether feedback should be generated.
		@param[in] nouveaufichier Indicates whether a new file should be created.
		*/
		void _InscrireLigneFichierTexte(
			System::String^ nomFichier,
			System::String^ message,
			bool indicateurFeedback,
			bool nouveaufichier);

		// DocString: FMTForm::_ObtenirSEQ
		/**
		@brief Reads schedules associated with a scenario.

		@param[in] nomFichierPri Project file path.
		@param[in] indexScenario Scenario index.

		@return Collection of schedules.
		*/
		std::vector<Core::FMTSchedule> _ObtenirSEQ(
			System::String^ nomFichierPri,
			int indexScenario);
		// DocString: FMTForm::_EnvoyerResultatsInterface
		/**
		@brief Sends SES results to the managed interface.

		The results are formatted and forwarded through the
		RetourJson event for consumption by the graphical user interface.

		@param[in] results SES execution results.
		@param[in] indCarbon Indicates whether carbon outputs are included.
		*/
		void _EnvoyerResultatsInterface(
			const FMTWrapperCore::SESResults& results,
			bool indCarbon);
	};
}
#endif