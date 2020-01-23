/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "FMTexceptionhandler.h"

namespace Exception

{

void FMTCPLErrorHandler(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg)
	{
	FMTexceptionhandler* handler = reinterpret_cast<FMTexceptionhandler*>(CPLGetErrorHandlerUserData());
	handler->handelCPLerror(eErrClass, nError, pszErrorMsg);
	}

void FMTexceptionhandler::passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger)
	{
	_logger = logger;
	}

FMTexceptionhandler* FMTexceptionhandler::getCPLdata()
	{
		return this;
	}

void FMTexceptionhandler::handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg)
	{
	char buffer[COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE];
	bool log = false;
	if (eErrClass == CE_Debug)
		{
		snprintf(buffer, sizeof(pszErrorMsg), "%s\n", pszErrorMsg);
		log = true;
		}
	else if (eErrClass == CE_Warning)
		{
		snprintf(buffer, sizeof(pszErrorMsg), "Warning %d: %s\n", nError, pszErrorMsg);
		log = true;
		}
	else
		{
		snprintf(buffer, sizeof(pszErrorMsg), "ERROR %d: %s\n", nError, pszErrorMsg);
		log = true;
		}
	if (log)
		{
		*_logger << buffer;
		}
	}

FMTexceptionhandler& FMTexceptionhandler::operator = (const FMTexceptionhandler& rhs)
{
	if (this != &rhs)
	{
		_level = rhs._level;
		_exception = rhs._exception;
		_errorcount = rhs._errorcount;
		_warningcount = rhs._warningcount;
		_logger = rhs._logger;
	}
	return *this;
}

void FMTexceptionhandler::throw_nested(const FMTexception& texception, int level = 0)
{
	*_logger << string(level, ' ') << texception.what() << "\n";
	try {
		std::rethrow_if_nested(texception);
	}
	catch (const FMTexception& texception)
	{
		throw_nested(texception, level + 1);
	}
	catch (...) {}
}

FMTlev FMTexceptionhandler::raise(FMTexc lexception, FMTwssect lsection, string text,
	const int& line, const string& file)
{
	FMTexception excp;
	if (lsection == FMTwssect::Empty)
	{
		excp = FMTexception(lexception, updatestatus(lexception, text));
	}
	else {
		excp = FMTexception(lexception, lsection, updatestatus(lexception, text));
	}
	if (_level == FMTlev::FMT_Warning)
	{
		throw FMTwarning(excp);
	}
	else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range) {
		throw FMTerror(excp);
	}
	return _level;
}

FMTexceptionhandler::FMTexceptionhandler(const FMTexceptionhandler& rhs) :
	_level(rhs._level),
	_exception(rhs._exception),
	_errorcount(rhs._errorcount),
	_warningcount(rhs._warningcount),
	_logger(rhs._logger)
	{

	}

FMTexceptionhandler::FMTexceptionhandler() : _level(FMTlev::FMT_None),
		_exception(FMTexc::None),
		_errorcount(0),
		_warningcount(0),
		_logger()

		{

		}

string FMTexceptionhandler::updatestatus(const FMTexc lexception, const string message)
{
	_exception = lexception;
	string msg;
	switch (_exception)
	{
	case FMTexc::WSconstants_replacement:
		msg += "Replaced Constants: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSfutur_types:
		msg += "Detected futur types: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WScomma_replacement:
		msg += "Replaced comma: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSinvalid_theme:
		msg += "Invalid Themes: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSinvalid_aggregate:
		msg += "Invalid Aggregates: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSinvalid_maskrange:
		msg += "Invalid Mask: " + message;
		_level = FMTlev::FMT_range;
		++_errorcount;
		break;
	case FMTexc::WSinvalid_number:
		msg += "Invalid number: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTinvalid_path:
		msg += "Invalid path: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WStheme_redefinition:
		msg += "Theme redefinition: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSaggregate_redefinition:
		msg += "Aggregate redefinition: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSempty_theme:
		msg += "Empty Theme: " + message;
		_level = FMTlev::FMT_range;
		++_errorcount;
		break;
	case FMTexc::WSempty_aggregate:
		msg += "Empty Aggregate: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSundefined_aggregate_value:
		msg += "Undefined aggregate value: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSundefined_attribute:
		msg += "Undefined attribute value: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSempty_action:
		msg += "Empty Action: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSwrong_partial:
		msg += "Wrong *Partial usage: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSinvalid_yield:
		msg += "Invalid Yield: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSpreexisting_yield:
		msg += "Pre-existing Yield: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSunsupported_yield:
		msg += "Unsupported Yield: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTinvalidband:
		msg += "Invalid raster band: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTinvaliddataset:
		msg += "Invalid GDAL dataset: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTinvalidrasterblock:
		msg += "Invalid GDAL block: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTinvalidlayer:
		msg += "Invalid GDAL layer: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTmissingfield:
		msg += "Missing field: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTinvalidoverview:
		msg += "Invalid GDAL overview: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTmissingrasterattribute:
		msg += "Missing raster cells attributes: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSunsupported_transition:
		msg += "Unsupported Transition: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSundefined_action:
		msg += "Undefined Action: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSempty_transition:
		msg += "Empty Transition: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSundefined_output:
		msg += "Undefined Output: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSunsupported_output:
		msg += "Unsupported Output: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSinvalid_transition_case:
		msg += "Invalid transition case: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTinvaliddriver:
		msg += "Invalid driver: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTinvalidAandT:
		msg += "Invalid number of Actions and Transitions: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSleakingtransition:
		msg += "Transition leaking area: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSundefineddeathaction:
		msg += "Undefined _death action: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSundefineddeathtransition:
		msg += "Undefined _death transition: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSignore:
		msg += "Ignoring: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::FMTmissingyield:
		msg += "Missing yield: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSattribute_redefinition:
		msg += "Attribute redefinition: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::WSundefined_constant:
		msg += "Undefined constant: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTmissingdevelopement:
		msg += "Missing developement: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTmissingobjective:
		msg += "Missing objective function: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTunsupported_objective:
		msg += "Unsupported objective function: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTinvalid_constraint:
		msg += "Invalid constraint: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::WSemptybound:
		msg += "Invalid @ bounds: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::FMTunboundedperiod:
		msg += "Unbounded replanning period: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	case FMTexc::FMTnonaddedconstraint:
		msg += "Constraint not added to matrix: " + message;
		_level = FMTlev::FMT_Warning;
		++_warningcount;
		break;
	case FMTexc::FMTmissinglicense:
		msg += "License error: " + message;
		_level = FMTlev::FMT_logic;
		++_errorcount;
		break;
	default:
		_exception = FMTexc::None;
		_level = FMTlev::FMT_None;
		break;
	};
	return msg;
}


}

