/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTexceptionhandler.h"
#include "FMTexception.h"
#include "FMTwarning.h"
#include "FMTerror.h"
#include "FMTcplhandler.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>


#if defined FMTWITHR
	#include "Rcpp.h"
#endif

#if defined FMTWITHPYTHON
	#include <boost/python.hpp>
#endif

#if defined FMTWITHOSI
	#include "CoinError.hpp"
#endif

#if defined _WIN32
	#include "FMTseException.h"
#endif




namespace Exception

{
	


boost::thread::id FMTexceptionhandler::mainthreadid = boost::this_thread::get_id();

boost::thread::id FMTexceptionhandler::crashedthreadid = boost::this_thread::get_id();

#if defined _WIN32
FMTScopedSeTranslator FMTexceptionhandler::m_SeTranslator = FMTScopedSeTranslator(FMTexceptionhandler::translateStructuralWIN32Exceptions);

void FMTexceptionhandler::translateStructuralWIN32Exceptions(unsigned int p_u, EXCEPTION_POINTERS*)
	{
		throw FMTseException(p_u);
	}
#endif

bool FMTexceptionhandler::ismainthread() const
	{
	return (boost::this_thread::get_id() == mainthreadid);
	}

bool FMTexceptionhandler::isthrowedonthread() const
	{
	return (crashedthreadid != mainthreadid);
	}

bool FMTexceptionhandler::isthisthreadthrowed() const
	{
	return (!ismainthread() && boost::this_thread::get_id() == crashedthreadid);
	}

void FMTexceptionhandler::checksignals() const
	{
	if (FMTexceptionhandler::ismainthread())
		{
		#if defined FMTWITHPYTHON
				if (PyErr_CheckSignals() == -1)
				{
					exit(1);
				}
		#endif
		#if defined FMTWITHR
				Rcpp::checkUserInterrupt();
		#endif
		}
	}

#if defined  FMTWITHGDAL

void FMTexceptionhandler::handelCPLerror(int eErrClass,int nError, const char * pszErrorMsg)
	{
	//CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg
	
	CPLErr theerrorclass = static_cast<CPLErr>(eErrClass);
    if (theerrorclass == CE_Failure || theerrorclass == CE_Fatal)
        {

        raise(FMTexc::FMTGDALerror,std::string(pszErrorMsg),"FMTdefaultexceptionhandler::handelCPLerror",__LINE__, __FILE__);
        }else if(theerrorclass == CE_Warning)
            {
            raise(FMTexc::FMTGDALwarning,std::string(pszErrorMsg),"FMTdefaultexceptionhandler::handelCPLerror",__LINE__, __FILE__);
            }
	}
#endif

FMTexceptionhandler* FMTexceptionhandler::getCPLdata()
{
	return this;
}

void FMTexceptionhandler::passinlogger(const std::unique_ptr<Logging::FMTlogger>& logger)
{
	_logger = logger.get();
}

FMTexceptionhandler& FMTexceptionhandler::operator = (const FMTexceptionhandler& rhs)
{
	if (this != &rhs)
	{
		//std::lock(mtx, rhs.mtx);
		boost::lock(mtx, rhs.mtx);
		boost::lock_guard<boost::recursive_mutex> self_lock(mtx, boost::adopt_lock/*std::adopt_lock*/);
		boost::lock_guard<boost::recursive_mutex> other_lock(rhs.mtx, boost::adopt_lock/*std::adopt_lock*/);
		_exception = rhs._exception;
		maxwarningsbeforesilenced = rhs.maxwarningsbeforesilenced;
		_logger = rhs._logger;
		usenestedexceptions = rhs.usenestedexceptions;
		errorstowarnings = rhs.errorstowarnings;
		_specificwarningcount = rhs._specificwarningcount;
		registered_threads = rhs.registered_threads;
		threadcrashexception = rhs.threadcrashexception;
	}
	return *this;
}


void FMTexceptionhandler::throw_nested(const std::exception& texception, int& level,bool rethrow)
{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		gutsofexceptionlog(texception, level);
		try {
			const auto _Nested = dynamic_cast<const std::nested_exception*>(&texception);
			if (_Nested && _Nested->nested_ptr())
			{
				std::rethrow_if_nested(texception);
				
			}
		}
		catch (const  std::exception& texception)
		{
			++level;
			throw_nested(texception, level, false);
		}
		catch (...)
		{
		#if defined FMTWITHR
		#else
			throw;
		#endif
					
		}
	if (rethrow)
		{
		#if defined FMTWITHR
		#else
		const std::exception_ptr expointer = std::current_exception();
		if (expointer)
		{
			throw;
		}
				
		#endif
		}
}

void FMTexceptionhandler::enablenestedexceptions()
	{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	usenestedexceptions = true;
	}

void FMTexceptionhandler::disablenestedexceptions()
	{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	usenestedexceptions = false;
	}

bool FMTexceptionhandler::needtorethrow() const
	{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	if (!usenestedexceptions &&
		(_exception == FMTexc::FMTfunctionfailed || _exception == FMTexc::FMTunhandlederror))
		{
		const std::exception_ptr expointer = std::current_exception();
		if (expointer)
			{
			std::rethrow_exception(expointer);
			return true;
			}
		}
	return false;
	}


FMTexception FMTexceptionhandler::raise(FMTexc lexception, std::string text,
	const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection,bool throwit)
{
	
	FMTexception excp;
	const FMTlev LEVEL = getLevel(lexception);
	if (lsection == Core::FMTsection::Empty)
	{
		excp = FMTexception(lexception, updatestatus(lexception, text), method, file, line);
	}
	else {
		excp = FMTexception(lexception, lsection, updatestatus(lexception, text),method,file,line);
	}
	if (throwit && !needtorethrow())
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		if (LEVEL == FMTlev::FMT_Warning)
			{
				std::throw_with_nested(FMTwarning(excp));
			}
		else if (LEVEL == FMTlev::FMT_logic || LEVEL == FMTlev::FMT_range)
			{
			std::throw_with_nested(FMTerror(excp));
			}
		}
	return excp;
}

FMTexceptionhandler::FMTexceptionhandler(const FMTexceptionhandler& rhs)
	{
		boost::lock_guard<boost::recursive_mutex> lock(rhs.mtx);
		_exception=rhs._exception;
		maxwarningsbeforesilenced = rhs.maxwarningsbeforesilenced;
		_logger=rhs._logger;
		usenestedexceptions=rhs.usenestedexceptions;
		errorstowarnings = rhs.errorstowarnings;
		_specificwarningcount = rhs._specificwarningcount;
		registered_threads = rhs.registered_threads;
		threadcrashexception = rhs.threadcrashexception;
	}

FMTexceptionhandler::FMTexceptionhandler(const std::unique_ptr<Logging::FMTlogger>& logger) : 
	_specificwarningcount(),
	_exception(FMTexc::None),
	maxwarningsbeforesilenced(10),
	_logger(logger.get()),
	usenestedexceptions(true),
	errorstowarnings(),
	registered_threads(),
	threadcrashexception()
{

}


FMTexceptionhandler::FMTexceptionhandler() : 
		_specificwarningcount(),
		_exception(FMTexc::None),
		maxwarningsbeforesilenced(10),
		_logger(),
		usenestedexceptions(true),
		errorstowarnings(),
		registered_threads(),
		threadcrashexception()
		
		{

		}


void FMTexceptionhandler::seterrorstowarnings(const std::vector<Exception::FMTexc>& errors)
	{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	errorstowarnings = errors;
	}

void FMTexceptionhandler::setmaxwarningsbeforesilenced(const size_t& maxwarningcount)
	{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		maxwarningsbeforesilenced = maxwarningcount;
	}

FMTlev FMTexceptionhandler::getLevel(const FMTexc p_exception) const
	{
	FMTlev level = FMTlev::FMT_None;
	if (std::find(errorstowarnings.begin(), errorstowarnings.end(), p_exception)
		!= errorstowarnings.end())
	{
		level = FMTlev::FMT_Warning;
	}
	else {
		switch (p_exception)
		{
		case FMTexc::FMTconstants_replacement:
			level = FMTlev::FMT_Debug;
			break;
		case FMTexc::FMTfutur_types:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTcomma_replacement:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTinvalid_theme:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalid_aggregate:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalid_maskrange:
			level = FMTlev::FMT_range;
			break;
		case FMTexc::FMTinvalid_number:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalid_path:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTtheme_redefinition:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTaggregate_redefinition:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTempty_theme:
			level = FMTlev::FMT_range;
			break;
		case FMTexc::FMTempty_aggregate:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTundefined_aggregate_value:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTundefined_attribute:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTempty_action:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTwrong_partial:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalid_yield:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTpreexisting_yield:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTunsupported_yield:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalidband:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvaliddataset:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalidrasterblock:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalidlayer:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTmissingfield:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalidoverview:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTmissingrasterattribute:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTunsupported_transition:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTundefined_action:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTempty_transition:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTundefined_output:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTunsupported_output:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalid_transition_case:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvaliddriver:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalidAandT:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTleakingtransition:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTundefineddeathaction:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTundefineddeathtransition:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTignore:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTmissingyield:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTattribute_redefinition:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTundefined_constant:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTmissingdevelopement:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTmissingobjective:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTunsupported_objective:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalid_constraint:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTemptybound:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTunboundedperiod:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTnonaddedconstraint:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTinfeasibleconstraint:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTmissinglicense:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTfunctionfailed:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTcoinerror:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTboostgrapherror:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTunhandlederror:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTnotlinegraph:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTrangeerror:
			level = FMTlev::FMT_range;
			break;
		case FMTexc::FMTGDALerror:
			level = FMTlev::FMT_range;
			break;
		case FMTexc::FMTGDALwarning:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTthematic_output_diff:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMToutput_missing_operator:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMToutput_too_much_operator:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTgdal_constructor_error:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalid_geometry:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTinvalidyield_number:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTundefinedoutput_attribute:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTempty_schedules:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTmissing_scenarios:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTschemefailed:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTmskerror:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMToveridedyield:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTmissing_parameter:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTdeathwithlock:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTreplanningwarning:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTyieldmodelprediction:
			level = FMTlev::FMT_Warning;
			break;
		case FMTexc::FMTsourcetotarget_transition:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTsame_transitiontargets:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTunclosedforloop:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTthreadcrash:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMToutofrangeyield:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTEmptyOA:
			level = FMTlev::FMT_logic;
			break;
		case FMTexc::FMTWIN32Error:
			level = FMTlev::FMT_logic;
			break;
		default:
			level = FMTlev::FMT_logic;
			break;
		};
	}
	return level;
	}


std::string FMTexceptionhandler::updatestatus(const FMTexc lexception, const std::string message)
{
	std::string msg;
	bool gotWarning = false;
	bool gotException = false;
	switch (lexception)
	{
	case FMTexc::FMTconstants_replacement:
		msg += "Replaced Constants: " + message;
		gotWarning = true;
		break;
	case FMTexc::FMTfutur_types:
		msg += "Detected futur types: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTcomma_replacement:
		msg += "Replaced comma: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTinvalid_theme:
		msg += "Invalid Themes: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalid_aggregate:
		msg += "Invalid Aggregates: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalid_maskrange:
		msg += "Invalid Mask: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalid_number:
		msg += "Invalid number: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalid_path:
		msg += "Invalid path: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTtheme_redefinition:
		msg += "Theme redefinition: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTaggregate_redefinition:
		msg += "Aggregate redefinition: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTempty_theme:
		msg += "Empty Theme: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTempty_aggregate:
		msg += "Empty Aggregate: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTundefined_aggregate_value:
		msg += "Undefined aggregate value: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTundefined_attribute:
		msg += "Undefined attribute value: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTempty_action:
		msg += "Empty Action: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTwrong_partial:
		msg += "Wrong *Partial usage: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalid_yield:
		msg += "Invalid Yield: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTpreexisting_yield:
		msg += "Pre-existing Yield: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTunsupported_yield:
		msg += "Unsupported Yield: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalidband:
		msg += "Invalid raster band: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvaliddataset:
		msg += "Invalid GDAL dataset: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalidrasterblock:
		msg += "Invalid GDAL block: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalidlayer:
		msg += "Invalid GDAL layer: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTmissingfield:
		msg += "Missing field: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalidoverview:
		msg += "Invalid GDAL overview: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTmissingrasterattribute:
		msg += "Missing raster cells attributes: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTunsupported_transition:
		msg += "Unsupported Transition: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTundefined_action:
		msg += "Undefined Action: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTempty_transition:
		msg += "Empty Transition: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTundefined_output:
		msg += "Undefined Output: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTunsupported_output:
		msg += "Unsupported Output: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalid_transition_case:
		msg += "Invalid transition case: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvaliddriver:
		msg += "Invalid driver: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalidAandT:
		msg += "Invalid number of Actions and Transitions: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTleakingtransition:
		msg += "Transition leaking area: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTundefineddeathaction:
		msg += "Undefined _death action: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTundefineddeathtransition:
		msg += "Undefined _death transition: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTignore:
		msg += "Ignoring: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTmissingyield:
		msg += "Missing yield: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTattribute_redefinition:
		msg += "Attribute redefinition: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTundefined_constant:
		msg += "Undefined constant: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTmissingdevelopement:
		msg += "Missing developement: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTmissingobjective:
		msg += "Missing objective function: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTunsupported_objective:
		msg += "Unsupported objective function: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTinvalid_constraint:
		msg += "Invalid constraint: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTemptybound:
		msg += "Invalid @ bounds: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTunboundedperiod:
		msg += "Unbounded replanning period: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTnonaddedconstraint:
		msg += "Constraint not added to matrix: " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTinfeasibleconstraint:
		msg += "Infeasible constraint: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTmissinglicense:
		msg += "License error: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTfunctionfailed:
		msg += "Function failed: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTcoinerror:
		msg +=  message;
		
		gotException=true;
		break;
	case FMTexc::FMTboostgrapherror:
		msg +=  message;
		
		gotException=true;
		break;
	case FMTexc::FMTunhandlederror:
		msg += "Unhandled error: " + message;
		
		gotException=true;
		break;
    case FMTexc::FMTnotlinegraph:
		msg += "FMTlinegragh error: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTrangeerror:
		msg += "FMTrange error: " + message;
		
		gotException=true;
		break;
    case FMTexc::FMTGDALerror:
        msg += "GDAL error: " + message;
		
		gotException=true;
        break;
    case FMTexc::FMTGDALwarning:
        msg += "GDAL warning: " + message;
		
		gotWarning = true;
        break;
    case FMTexc::FMTthematic_output_diff:
		msg += "Differences in output thematic: " + message;
		
		gotWarning = true;
		break;
    case FMTexc::FMToutput_missing_operator:
   		msg += "Missing operator for the output: " + message;
   		
   		gotException=true;
   		break;
    case FMTexc::FMToutput_too_much_operator:
   		msg += "Too much operator in the output definition " + message;
   		
   		gotException=true;
   		break;
	case FMTexc::FMTgdal_constructor_error:
   		msg += "Error while contructing gdal object : " + message;
   		
   		gotException=true;
   		break;
	case FMTexc::FMTinvalid_geometry:
   		msg += "Invalid geometry " + message;
   		
   		gotException=true;
   		break;
	case FMTexc::FMTinvalidyield_number:
		msg += "Non valid yield number: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTundefinedoutput_attribute:
		msg += "Undefined output attribute value: " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTempty_schedules:
		msg += "No schedule found at : " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTmissing_scenarios:
		msg += "Cannot find scenarios : " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTschemefailed:
		msg += message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTmskerror:
		msg += "Mosek error : " + message;
		
		gotException=true;
		break;
	case FMTexc::FMToveridedyield:
		msg += "Overided yield : " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTmissing_parameter:
		msg += "Missing parameter : " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTdeathwithlock:
		msg += "The given FMTactualdevelopement will be dead before the lock expire : " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTreplanningwarning:
		msg += "While replanning " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTyieldmodelprediction:
		msg += "When predicting " + message;
		
		gotWarning = true;
		break;
	case FMTexc::FMTsourcetotarget_transition:
		msg += "Transition source can return target " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTsame_transitiontargets:
		msg += "Equivalent transition target " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTunclosedforloop:
		msg += "Unclosed for loops " + message;
		
		gotException=true;
		break;
	case FMTexc::FMTthreadcrash:
		msg += "Crash on thread " + message;
		
		gotException=true;
		break;
	case FMTexc::FMToutofrangeyield:
		msg += "Out of range yield" + message;
		
		gotException=true;
		break;
	case FMTexc::FMTEmptyOA:
		msg += "Empty Operating Area " + message;
		gotException=true;
		break;
	case FMTexc::FMTWIN32Error:
		msg += message;
		gotException = true;
		break;
	default:
		gotException = true;
		break;
	};
	if (std::find(errorstowarnings.begin(), errorstowarnings.end(), lexception)
		!= errorstowarnings.end())
		{
		msg += "Ignoring: " + message;
		gotException = false;
		gotWarning = true;
		}
		if (gotException)
		{
			boost::lock_guard<boost::recursive_mutex> guard(mtx);
			_exception = lexception;
			if (!ismainthread() && !isthrowedonthread())
			{
				crashedthreadid = boost::this_thread::get_id();
			}
		}
	return msg;
}

void FMTexceptionhandler::raisefromthreadcatch(std::string text,
	const std::string& method, const int& line, const std::string& file,
	Core::FMTsection lsection)
{
	if (isthisthreadthrowed())
	{
		try {
			raisefromcatch(text, method, line, file, lsection);
		}catch (...)
		{
			threadcrashexception = std::current_exception();
		}
	}else if (ismainthread())
	{
		raisefromcatch(text, method, line, file, lsection);
	}
	//Do nothing if you are a thread an you have not thrown...all your exceptions are lost
}

void FMTexceptionhandler::registerworkerthread()
{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	registered_threads.insert(boost::this_thread::get_id());
}

bool FMTexceptionhandler::isthreadregistered() const
{
return (registered_threads.find(boost::this_thread::get_id()) != registered_threads.end());
}



void FMTexceptionhandler::reraiseifthreadcrash()
{
	
	if (isthrowedonthread() && !isthisthreadthrowed() && !isthreadregistered())
	{
		registerworkerthread();
		//registered_threads.insert(boost::this_thread::get_id());
		//Raise a dumy exception to make sure the main thread and the slave thread are aware of crash
		raise(Exception::FMTexc::FMTthreadcrash,"","FMTexceptionhandler::reraiseifthreadcrash",__LINE__,__FILE__);
	}
}

FMTexception FMTexceptionhandler::raisefromcatch(std::string text,
	const std::string& method, const int& line, const std::string& file,
	Core::FMTsection lsection)
{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	FMTexc lexception = FMTexc::FMTfunctionfailed;
	const std::exception_ptr expointer = std::current_exception();
	if (expointer)
	{
		try {
			std::rethrow_exception(expointer);
		}
		catch (const FMTexception& tcexception)
		{
			return this->raise(lexception,text, method, line, file,lsection);
		}
		#if defined FMTWITHOSI
			catch (const CoinError& coinexception)
			{

				updatestatus(FMTexc::FMTcoinerror,coinexception.message());
				try {
					std::throw_with_nested(FMTerror(coinexception));
				}catch (...)
					{
					return this->raise(lexception, text, method, line, file, lsection);
					}
			}
		#endif
		catch (const boost::bad_graph& grapherror)
		{
			updatestatus(FMTexc::FMTboostgrapherror,grapherror.what());
			try {
				std::throw_with_nested(FMTerror(grapherror));
			}
			catch (...)
			{
				return this->raise(lexception,text, method, line, file, lsection);
			}
		}
		#if defined _WIN32
			catch (const FMTseException& seError)
			{
				const std::string CAUGHTWIN32 = "Win32 Error number " + std::to_string(seError.getSeNumber());
				updatestatus(FMTexc::FMTWIN32Error, CAUGHTWIN32);
				try {
					std::throw_with_nested(FMTerror(seError));
				}
				catch (...)
				{
					return this->raise(lexception, text, method, line, file, lsection);
				}
			}
		#endif
		catch (...)
		{
			lexception = FMTexc::FMTunhandlederror;
			return this->raise(lexception,text, method, line, file, lsection);
		}
	}
	return this->raise(lexception,text, method, line, file, lsection);
}

void FMTexceptionhandler::gutsofprintexceptions(std::string text,
	const std::string& method, const int& line, const std::string& fil,int& levelreference,
	Core::FMTsection lsection, bool logfirstlevel)
{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	FMTexc lexception = FMTexc::FMTfunctionfailed;
	const std::exception_ptr expointer = std::current_exception();
	bool rethrowing = false;
#if defined FMTWITHR
	if (_errorcount == 0)
	{
		rethrowing = true;
	}
#endif
	const FMTexception newexception = this->raise(lexception, text, method, line, fil, lsection, rethrowing);
	if (_logger)
	{
		_logger->setstreamflush(true);
	}
	const bool keepit =  ((!ismainthread() && isthisthreadthrowed()));// && !print);
	//Keep it est a vrai sur le main mais devrait être à faux...
	bool needtolog = logfirstlevel;

	/*if (keepit || (levelreference > 0))
	{
		needtolog = false;
	}*/
	
	if (expointer)
	
	{
		//levelreference = 0;
		try {

			std::rethrow_exception(expointer);
			
		}
		catch (const FMTexception& tcexception)
		{
			if (usenestedexceptions)
			{
				if (needtolog)
				{
					*_logger << newexception.what() << "\n";

				}
				++levelreference;
				//levelreference = 1;
			}
			this->throw_nested(tcexception, levelreference);
		}
#if defined FMTWITHOSI
		catch (const CoinError& coinexception)
		{
			if (usenestedexceptions)
			{
				if (needtolog)
				{
					*_logger << newexception.what() << "\n";

				}
				++levelreference;
				//levelreference = 1;
			}
			this->throw_nested(Exception::FMTerror(coinexception), levelreference);
		}
#endif
		catch (const boost::bad_graph& grapherror)
		{
			if (usenestedexceptions)
			{
				if (needtolog)
				{
					*_logger << newexception.what() << "\n";

				}
				++levelreference;
				//levelreference = 1;
			}
			this->throw_nested(Exception::FMTerror(grapherror), levelreference);
		}
		#if defined _WIN32
		catch (const FMTseException& seError)
		{
			if (usenestedexceptions)
			{
				if (needtolog)
				{
					*_logger << newexception.what() << "\n";

				}
				++levelreference;
			}
			this->throw_nested(Exception::FMTerror(seError), levelreference);
		}
	#endif
		catch (...)
		{
			lexception = FMTexc::FMTunhandlederror;
			const FMTexception newexception = this->raise(lexception, text, method, line, fil, lsection, false);
			this->throw_nested(newexception, levelreference );//0);
		}
	}
	else {
		++levelreference;
		this->throw_nested(newexception, levelreference);//1);
	}
}

void FMTexceptionhandler::gutsofexceptionlog(const std::exception& texception, const int& level)
{
	const std::string linereplacement = "\n" + std::string(level, ' ');
	std::string message = texception.what();
	boost::replace_all(message, "\n", linereplacement);
	*_logger << std::string(level, ' ') << message << "\n";
	#if defined FMTWITHR
		const std::nested_exception* nested = dynamic_cast<const std::nested_exception*>(&texception);
		const std::exception_ptr  excp = nested->nested_ptr();
		if (excp == nullptr || (message.find("FMTexc(56)") != std::string::npos))//If last element just get out of c++ and get back to R
		{
			throw(Rcpp::exception(message.c_str()));
		}
	#endif
}


void FMTexceptionhandler::printexceptions(std::string text,
	const std::string& method, const int& line, const std::string& fil,
	Core::FMTsection lsection)
{
	int levelofprint = 0;
	try {
		gutsofprintexceptions(text, method, line, fil, levelofprint,lsection);
	}catch (...)
	{
		if (threadcrashexception)
		{
			*_logger << std::string(levelofprint,' ') + "Crash on thread " << boost::lexical_cast<std::string>(crashedthreadid) << "\n";
			try {
				std::rethrow_exception(threadcrashexception);
			}
			catch (...)
			{
				
				gutsofprintexceptions(text, method, line, fil, levelofprint, lsection,false);
			}
		}
		throw;
	}
}
}



BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTexceptionhandler)
