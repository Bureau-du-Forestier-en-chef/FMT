#include "FMTExceptionFactory.h"

#include "FMTConstantsReplacement.h"
#include "FMTCommaReplacement.h"
#include "FMTFuturTypes.h"
#include "FMTInvalidTheme.h"
#include "FMTInvalidAggregate.h"
#include "FMTInvalidMaskrange.h"
#include "FMTInvalidYield.h"
#include "FMTInvalidAction.h"
#include "FMTInvalidTransition.h"
#include "FMTInvalidTransitionCase.h"
#include "FMTInvalidNumber.h"
#include "FMTInvalidPath.h"
#include "FMTThemeRedefinition.h"
#include "FMTAggregateRedefinition.h"
#include "FMTEmptyTheme.h"
#include "FMTEmptyAggregate.h"
#include "FMTUndefinedAggregateValue.h"
#include "FMTUndefinedAttribute.h"
#include "FMTEmptyAction.h"
#include "FMTWrongPartial.h"
#include "FMTPreexistingYield.h"
#include "FMTUnsupportedYield.h"
#include "FMTInvalidDataset.h"
#include "FMTInvalidBand.h"
#include "FMTInvalidRasterBlock.h"
#include "FMTInvalidLayer.h"
#include "FMTMissingField.h"
#include "FMTInvalidOverview.h"
#include "FMTMissingRasterAttribute.h"
#include "FMTUnsupportedTransition.h"
#include "FMTUndefinedAction.h"
#include "FMTEmptyTransition.h"
#include "FMTUndefinedOutput.h"
#include "FMTUnsupportedOutput.h"
#include "FMTInvalidDriver.h"
#include "FMTInvalidAandT.h"
#include "FMTLeakingTransition.h"
#include "FMTUndefinedDeathAction.h"
#include "FMTUndefinedDeathTransition.h"
#include "FMTIgnore.h"
#include "FMTMissingYield.h"
#include "FMTAttributeRedefinition.h"
#include "FMTUndefinedConstant.h"
#include "FMTMissingDevelopment.h"
#include "FMTMissingObjective.h"
#include "FMTUnsupportedObjective.h"
#include "FMTInvalidConstraint.h"
#include "FMTEmptyBound.h"
#include "FMTUnboundedPeriod.h"
#include "FMTNonAddedConstraint.h"
#include "FMTMissingLicense.h"
#include "FMTFunctionFailed.h"
#include "FMTCoinError.h"
#include "FMTBoostGraphError.h"
#include "FMTUnhandledError.h"
#include "FMTNotLineGraph.h"
#include "FMTRangeError.h"
#include "FMTGDALError.h"
#include "FMTGDALWarning.h"
#include "FMTInfeasibleConstraint.h"
#include "FMTThematicOutputDiff.h"
#include "FMTOutputMissingOperator.h"
#include "FMTOutputTooMuchOperator.h"
#include "FMTInvalidYieldNumber.h"
#include "FMTGDALConstructorError.h"
#include "FMTInvalidGeometry.h"
#include "FMTUndefinedOutputAttribute.h"
#include "FMTEmptySchedules.h"
#include "FMTMissingScenarios.h"
#include "FMTSchemeFailed.h"
#include "FMTMSKError.h"
#include "FMTOveridedYield.h"
#include "FMTMissingParameter.h"
#include "FMTDeathWithLock.h"
#include "FMTReplanningWarning.h"
#include "FMTYieldModelPrediction.h"
#include "FMTSourceToTargetTransition.h"
#include "FMTSameTransitionTargets.h"
#include "FMTUnclosedForLoop.h"
#include "FMTThreadCrash.h"
#include "FMTOutOfRangeYield.h"
#include "FMTEmptyOA.h"
#include "FMTWIN32Error.h"
#include "FMTMSKNumericalProblem.h"
#include "FMTUnreachableThreshold.h"


namespace Exception
{

#if defined FMTWITHOSI
	std::unique_ptr<FMTException> FMTExceptionFactory::createCoinException(const CoinError & p_CoinException)
		{
		return std::make_unique<FMTCoinError>(p_CoinException);
		}
	#endif
	std::unique_ptr<FMTException> FMTExceptionFactory::createBoostGraphException(const boost::bad_graph& p_exception)
		{
		return std::make_unique<FMTBoostGraphError>(p_exception);
		}
#if defined _MSC_VER
	std::unique_ptr<FMTException> FMTExceptionFactory::createSeException(const FMTSeException& p_SeException)
		{
		return std::make_unique<FMTWIN32Error>(p_SeException);
		}
#endif

	std::unique_ptr<FMTException> FMTExceptionFactory::createUnhandledException(const std::exception& p_exception)
	{
		return std::make_unique<FMTUnhandledError>(p_exception);
	}


	std::unique_ptr<FMTException>
	FMTExceptionFactory::create(
		FMTexc p_exception,
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line)
	{
		switch(p_exception)
		{

		case FMTexc::FMTconstants_replacement:
			return std::make_unique<FMTConstantsReplacement>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTcomma_replacement:
			return std::make_unique<FMTCommaReplacement>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTfutur_types:
			return std::make_unique<FMTFuturTypes>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_theme:
			return std::make_unique<FMTInvalidTheme>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_aggregate:
			return std::make_unique<FMTInvalidAggregate>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_maskrange:
			return std::make_unique<FMTInvalidMaskrange>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_yield:
			return std::make_unique<FMTInvalidYield>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_action:
			return std::make_unique<FMTInvalidAction>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_transition:
			return std::make_unique<FMTInvalidTransition>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_transition_case:
			return std::make_unique<FMTInvalidTransitionCase>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_number:
			return std::make_unique<FMTInvalidNumber>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_path:
			return std::make_unique<FMTInvalidPath>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTtheme_redefinition:
			return std::make_unique<FMTThemeRedefinition>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTaggregate_redefinition:
			return std::make_unique<FMTAggregateRedefinition>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTempty_theme:
			return std::make_unique<FMTEmptyTheme>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTempty_aggregate:
			return std::make_unique<FMTEmptyAggregate>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTundefined_aggregate_value:
			return std::make_unique<FMTUndefinedAggregateValue>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTundefined_attribute:
			return std::make_unique<FMTUndefinedAttribute>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTempty_action:
			return std::make_unique<FMTEmptyAction>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTwrong_partial:
			return std::make_unique<FMTWrongPartial>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTpreexisting_yield:
			return std::make_unique<FMTPreexistingYield>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTunsupported_yield:
			return std::make_unique<FMTUnsupportedYield>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvaliddataset:
			return std::make_unique<FMTInvalidDataset>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalidband:
			return std::make_unique<FMTInvalidBand>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalidrasterblock:
			return std::make_unique<FMTInvalidRasterBlock>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalidlayer:
			return std::make_unique<FMTInvalidLayer>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTmissingfield:
			return std::make_unique<FMTMissingField>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalidoverview:
			return std::make_unique<FMTInvalidOverview>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTmissingrasterattribute:
			return std::make_unique<FMTMissingRasterAttribute>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTunsupported_transition:
			return std::make_unique<FMTUnsupportedTransition>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTundefined_action:
			return std::make_unique<FMTUndefinedAction>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTempty_transition:
			return std::make_unique<FMTEmptyTransition>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTundefined_output:
			return std::make_unique<FMTUndefinedOutput>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTunsupported_output:
			return std::make_unique<FMTUnsupportedOutput>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvaliddriver:
			return std::make_unique<FMTInvalidDriver>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalidAandT:
			return std::make_unique<FMTInvalidAandT>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTleakingtransition:
			return std::make_unique<FMTLeakingTransition>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTundefineddeathaction:
			return std::make_unique<FMTUndefinedDeathAction>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTundefineddeathtransition:
			return std::make_unique<FMTUndefinedDeathTransition>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTignore:
			return std::make_unique<FMTIgnore>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTmissingyield:
			return std::make_unique<FMTMissingYield>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTattribute_redefinition:
			return std::make_unique<FMTAttributeRedefinition>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTundefined_constant:
			return std::make_unique<FMTUndefinedConstant>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTmissingdevelopment:
			return std::make_unique<FMTMissingDevelopment>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTmissingobjective:
			return std::make_unique<FMTMissingObjective>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTunsupported_objective:
			return std::make_unique<FMTUnsupportedObjective>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_constraint:
			return std::make_unique<FMTInvalidConstraint>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTemptybound:
			return std::make_unique<FMTEmptyBound>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTunboundedperiod:
			return std::make_unique<FMTUnboundedPeriod>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTnonaddedconstraint:
			return std::make_unique<FMTNonAddedConstraint>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTmissinglicense:
			return std::make_unique<FMTMissingLicense>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTfunctionfailed:
			return std::make_unique<FMTFunctionFailed>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTcoinerror:
			return std::make_unique<FMTCoinError>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTboostgrapherror:
			return std::make_unique<FMTBoostGraphError>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTunhandlederror:
			return std::make_unique<FMTUnhandledError>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTnotlinegraph:
			return std::make_unique<FMTNotLineGraph>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTrangeerror:
			return std::make_unique<FMTRangeError>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTGDALerror:
			return std::make_unique<FMTGDALError>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTGDALwarning:
			return std::make_unique<FMTGDALWarning>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinfeasibleconstraint:
			return std::make_unique<FMTInfeasibleConstraint>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTthematic_output_diff:
			return std::make_unique<FMTThematicOutputDiff>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMToutput_missing_operator:
			return std::make_unique<FMTOutputMissingOperator>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMToutput_too_much_operator:
			return std::make_unique<FMTOutputTooMuchOperator>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalidyield_number:
			return std::make_unique<FMTInvalidYieldNumber>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTgdal_constructor_error:
			return std::make_unique<FMTGDALConstructorError>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTinvalid_geometry:
			return std::make_unique<FMTInvalidGeometry>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTundefinedoutput_attribute:
			return std::make_unique<FMTUndefinedOutputAttribute>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTempty_schedules:
			return std::make_unique<FMTEmptySchedules>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTmissing_scenarios:
			return std::make_unique<FMTMissingScenarios>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTschemefailed:
			return std::make_unique<FMTSchemeFailed>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTmskerror:
			return std::make_unique<FMTMSKError>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMToveridedyield:
			return std::make_unique<FMTOveridedYield>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTmissing_parameter:
			return std::make_unique<FMTMissingParameter>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTdeathwithlock:
			return std::make_unique<FMTDeathWithLock>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTreplanningwarning:
			return std::make_unique<FMTReplanningWarning>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTyieldmodelprediction:
			return std::make_unique<FMTYieldModelPrediction>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTsourcetotarget_transition:
			return std::make_unique<FMTSourceToTargetTransition>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTsame_transitiontargets:
			return std::make_unique<FMTSameTransitionTargets>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTunclosedforloop:
			return std::make_unique<FMTUnclosedForLoop>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTthreadcrash:
			return std::make_unique<FMTThreadCrash>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMToutofrangeyield:
			return std::make_unique<FMTOutOfRangeYield>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTEmpty_OA:
			return std::make_unique<FMTEmptyOA>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTWIN32_Error:
			return std::make_unique<FMTWIN32Error>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTMSKnumerical_problem:
			return std::make_unique<FMTMSKNumericalProblem>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

		case FMTexc::FMTunreachable_threshold:
			return std::make_unique<FMTUnreachableThreshold>(
				p_section,
				p_message,
				p_method,
				p_file,
				p_line);

			default:
				return std::make_unique<FMTUnhandledError>(
					p_section,
					p_message,
					p_method,
					p_file,
					p_line);
		}
	}
}