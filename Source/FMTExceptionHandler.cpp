/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTExceptionHandler.h"
#include "FMTException.h"
#include "FMTcplhandler.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include "FMTExceptionFactory.h"
#include "FMTWarning.h"
#include <boost/graph/exception.hpp>


#if defined FMTWITHR
	#include "Rcpp.h"
#endif

#if defined FMTWITHPYTHON
	#include <boost/python.hpp>
#endif

#if defined FMTWITHOSI
	#include "CoinError.hpp"
#endif

#if defined _MSC_VER
	#include "FMTSeException.h"
#endif




namespace Exception

{
	


boost::thread::id FMTExceptionHandler::m_mainthreadid = boost::this_thread::get_id();

boost::thread::id FMTExceptionHandler::m_crashedthreadid = boost::this_thread::get_id();

#if defined _MSC_VER
FMTScopedSeTranslator FMTExceptionHandler::m_SeTranslator = FMTScopedSeTranslator(FMTExceptionHandler::translateStructuralWIN32Exceptions);

void FMTExceptionHandler::translateStructuralWIN32Exceptions(unsigned int p_u, EXCEPTION_POINTERS*)
	{
		throw FMTSeException(p_u);
	}
#endif


std::string FMTExceptionHandler::getErrorDescription(bool p_french, FMTexc p_type)
	{
	std::unique_ptr<FMTException> EXCEPTION = _createException(p_type, Core::FMTsection::Empty,
		"", "FMTExceptionHandler::getErrorDescription", __FILE__, __LINE__);
	return EXCEPTION->getDescription(p_french);
	}

bool FMTExceptionHandler::_isMainThread() const
	{
	return (boost::this_thread::get_id() == m_mainthreadid);
	}

bool FMTExceptionHandler::_isThrowedOnThread() const
	{
	return (m_crashedthreadid != m_mainthreadid);
	}

bool FMTExceptionHandler::_isThisThreadThrowed() const
	{
	return (!_isMainThread() && boost::this_thread::get_id() == m_crashedthreadid);
	}

void FMTExceptionHandler::checkSignals() const
	{
	if (FMTExceptionHandler::_isMainThread())
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

void FMTExceptionHandler::handelCplError(int eErrClass,int nError, const char * pszErrorMsg)
	{
	//CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg
	
	CPLErr theerrorclass = static_cast<CPLErr>(eErrClass);
    if (theerrorclass == CE_Failure || theerrorclass == CE_Fatal)
        {

        raise(FMTexc::FMTGDALerror,std::string(pszErrorMsg),"FMTDefaultExceptionHandler::handelCplError",__LINE__, __FILE__);
        }else if(theerrorclass == CE_Warning)
            {
            raise(FMTexc::FMTGDALwarning,std::string(pszErrorMsg),"FMTDefaultExceptionHandler::handelCplError",__LINE__, __FILE__);
            }
	}
#endif

FMTExceptionHandler* FMTExceptionHandler::getCplData()
{
	return this;
}

void FMTExceptionHandler::passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger)
{
	_logger = logger.get();
}

FMTExceptionHandler& FMTExceptionHandler::operator = (const FMTExceptionHandler& rhs)
{
	if (this != &rhs)
	{
		//std::lock(mtx, rhs.mtx);
		boost::lock(m_mtx, rhs.m_mtx);
		boost::lock_guard<boost::recursive_mutex> self_lock(m_mtx, boost::adopt_lock/*std::adopt_lock*/);
		boost::lock_guard<boost::recursive_mutex> other_lock(rhs.m_mtx, boost::adopt_lock/*std::adopt_lock*/);
		_exception = rhs._exception;
		m_maxwarningsbeforesilenced = rhs.m_maxwarningsbeforesilenced;
		_logger = rhs._logger;
		m_usenestedexceptions = rhs.m_usenestedexceptions;
		m_errorstowarnings = rhs.m_errorstowarnings;
		m_specificwarningcount = rhs.m_specificwarningcount;
		m_registered_threads = rhs.m_registered_threads;
		m_threadcrashexception = rhs.m_threadcrashexception;
	}
	return *this;
}


void FMTExceptionHandler::throwNested(const std::exception& texception, int& level,bool rethrow)
{
		boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
		_gutsOfExceptionLog(texception, level);
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
			throwNested(texception, level, false);
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

void FMTExceptionHandler::enableNestedExceptions()
	{
	boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
	m_usenestedexceptions = true;
	}

void FMTExceptionHandler::disableNestedExceptions()
	{
	boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
	m_usenestedexceptions = false;
	}

bool FMTExceptionHandler::_needToRethrow() const
	{
	boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
	if (!m_usenestedexceptions &&
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


FMTException FMTExceptionHandler::raise(FMTexc lexception, std::string text,
	const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection,bool throwit)
{
	
	std::unique_ptr<FMTException> newException = _createException(lexception, lsection,
									text, method, file, line);
	_updateStatus(newException);
	if (throwit && !_needToRethrow())
		{
		boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
		if (newException->getLevel() != FMTlev::FMT_None &&
			newException->getLevel() != FMTlev::FMT_Debug)
			{
			std::throw_with_nested(*newException);
			}
		}
	return *newException;
}

FMTExceptionHandler::FMTExceptionHandler(const FMTExceptionHandler& rhs)
	{
		boost::lock_guard<boost::recursive_mutex> lock(rhs.m_mtx);
		_exception=rhs._exception;
		m_maxwarningsbeforesilenced = rhs.m_maxwarningsbeforesilenced;
		_logger=rhs._logger;
		m_usenestedexceptions=rhs.m_usenestedexceptions;
		m_errorstowarnings = rhs.m_errorstowarnings;
		m_specificwarningcount = rhs.m_specificwarningcount;
		m_registered_threads = rhs.m_registered_threads;
		m_threadcrashexception = rhs.m_threadcrashexception;
	}

FMTExceptionHandler::FMTExceptionHandler(const std::unique_ptr<Logging::FMTLogger>& logger) : 
	_exception(FMTexc::None),
	m_maxwarningsbeforesilenced(10),
	_logger(logger.get()),
	m_usenestedexceptions(true),
	m_errorstowarnings(),
	m_registered_threads(),
	m_threadcrashexception(),
	m_specificwarningcount()
{

}


FMTExceptionHandler::FMTExceptionHandler() : 
		_exception(FMTexc::None),
		m_maxwarningsbeforesilenced(10),
		_logger(),
		m_usenestedexceptions(true),
		m_errorstowarnings(),
		m_registered_threads(),
		m_threadcrashexception(),
		m_specificwarningcount()
		{

		}


void FMTExceptionHandler::setErrorsToWarnings(const std::vector<Exception::FMTexc>& errors)
	{
	boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
	m_errorstowarnings = errors;
	}

void FMTExceptionHandler::setMaxWarningsBeforeSilenced(const size_t& maxwarningcount)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
		m_maxwarningsbeforesilenced = maxwarningcount;
	}


	std::unique_ptr<FMTException> FMTExceptionHandler::_createException(
		FMTexc p_exception, 
		Core::FMTsection p_section, 
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file, 
		int p_line)
	{
		return FMTExceptionFactory::create(p_exception, p_section, p_message, p_method, p_file, p_line);
	}


void FMTExceptionHandler::_updateStatus(std::unique_ptr<FMTException>& p_exception)
{
	bool gotException = p_exception->isFatal();
	if (std::find(m_errorstowarnings.begin(), m_errorstowarnings.end(), p_exception->getType())
		!= m_errorstowarnings.end())
		{
		p_exception->setIgnore();
		gotException = false;
		}
	if (gotException)
		{
			boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
			_exception = p_exception->getType();
			if (!_isMainThread() && !_isThrowedOnThread())
			{
				m_crashedthreadid = boost::this_thread::get_id();
			}
		}
}

void FMTExceptionHandler::raiseFromThreadCatch(std::string text,
	const std::string& method, const int& line, const std::string& file,
	Core::FMTsection lsection)
{
	if (_isThisThreadThrowed())
	{
		try {
			raiseFromCatch(text, method, line, file, lsection);
		}catch (...)
		{
			m_threadcrashexception = std::current_exception();
		}
	}else if (_isMainThread())
	{
		raiseFromCatch(text, method, line, file, lsection);
	}
	//Do nothing if you are a thread an you have not thrown...all your exceptions are lost
}

void FMTExceptionHandler::_registerWorkerThread()
{
	boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
	m_registered_threads.insert(boost::this_thread::get_id());
}

bool FMTExceptionHandler::_isThreadRegistered() const
{
return (m_registered_threads.find(boost::this_thread::get_id()) != m_registered_threads.end());
}



void FMTExceptionHandler::reRaiseIfThreadCrash()
{
	
	if (_isThrowedOnThread() && !_isThisThreadThrowed() && !_isThreadRegistered())
	{
		_registerWorkerThread();
		//registered_threads.insert(boost::this_thread::get_id());
		//Raise a dumy exception to make sure the main thread and the slave thread are aware of crash
		raise(Exception::FMTexc::FMTthreadcrash,"","FMTExceptionHandler::reRaiseIfThreadCrash",__LINE__,__FILE__);
	}
}

FMTException FMTExceptionHandler::raiseFromCatch(std::string text,
	const std::string& method, const int& line, const std::string& file,
	Core::FMTsection lsection)
{
	boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
	FMTexc lexception = FMTexc::FMTfunctionfailed;
	const std::exception_ptr expointer = std::current_exception();
	if (expointer)
	{
		try {
			std::rethrow_exception(expointer);
		}
		catch (const FMTException& tcexception)
		{
			return this->raise(lexception,text, method, line, file,lsection);
		}
		#if defined FMTWITHOSI
			catch (const CoinError& coinexception)
			{
				std::unique_ptr<FMTException> error = FMTExceptionFactory::createCoinException(coinexception);
				_updateStatus(error);
				try {
					std::throw_with_nested(*error);
				}catch (...)
					{
					return this->raise(lexception, text, method, line, file, lsection);
					}
			}
		#endif
		catch (const boost::bad_graph& grapherror)
		{
			std::unique_ptr<FMTException> error = FMTExceptionFactory::createBoostGraphException(grapherror);
			_updateStatus(error);
			try {
				std::throw_with_nested(*error);
			}
			catch (...)
			{
				return this->raise(lexception,text, method, line, file, lsection);
			}
		}
		#if defined _MSC_VER
			catch (const FMTSeException& seError)
			{
				std::unique_ptr<FMTException> error = FMTExceptionFactory::createSeException(seError);
				_updateStatus(error);
				try {
					std::throw_with_nested(*error);
				}
				catch (...)
				{
					return this->raise(lexception, text, method, line, file, lsection);
				}
			}
		#endif
		catch (const std::exception& stdexception)
		{
			std::unique_ptr<FMTException> error = FMTExceptionFactory::createUnhandledException(stdexception);
			_updateStatus(error);
			return this->raise(error->getType(), 
				std::string(stdexception.what()) + " " + text,
				method, line, file, lsection);
		}catch (...)
		{
			return this->raise(FMTexc::FMTunhandlederror,text, method, line, file, lsection);
		}
	}
	return this->raise(lexception,text, method, line, file, lsection);
}

void FMTExceptionHandler::_gutsOfPrintExceptions(std::string text,
	const std::string& method, const int& line, const std::string& fil,int& levelreference,
	Core::FMTsection lsection, bool logfirstlevel)
{
	boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
	FMTexc lexception = FMTexc::FMTfunctionfailed;
	const std::exception_ptr expointer = std::current_exception();
	bool rethrowing = false;
#if defined FMTWITHR
	if (_errorcount == 0)
	{
		rethrowing = true;
	}
#endif
	const FMTException newexception = this->raise(lexception, text, method, line, fil, lsection, rethrowing);
	if (_logger)
	{
		_logger->setStreamFlush(true);
	}
	const bool keepit =  ((!_isMainThread() && _isThisThreadThrowed()));// && !print);
	//Keep it est a vrai sur le main mais devrait �tre � faux...
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
		catch (const FMTException& tcexception)
		{
			if (m_usenestedexceptions)
			{
				if (needtolog)
				{
					*_logger << newexception.what() << "\n";

				}
				++levelreference;
				//levelreference = 1;
			}
			this->throwNested(tcexception, levelreference);
		}
#if defined FMTWITHOSI
		catch (const CoinError& coinexception)
		{
			if (m_usenestedexceptions)
			{
				if (needtolog)
				{
					*_logger << newexception.what() << "\n";

				}
				++levelreference;
				//levelreference = 1;
			}
			this->throwNested(*FMTExceptionFactory::createCoinException(coinexception), levelreference);
		}
#endif
		catch (const boost::bad_graph& grapherror)
		{
			if (m_usenestedexceptions)
			{
				if (needtolog)
				{
					*_logger << newexception.what() << "\n";

				}
				++levelreference;
				//levelreference = 1;
			}
			this->throwNested(*FMTExceptionFactory::createBoostGraphException(grapherror), levelreference);
		}
		#if defined _MSC_VER
		catch (const FMTSeException& seError)
		{
			if (m_usenestedexceptions)
			{
				if (needtolog)
				{
					*_logger << newexception.what() << "\n";

				}
				++levelreference;
			}
			this->throwNested(*FMTExceptionFactory::createSeException(seError), levelreference);
		}
	#endif
		catch (...)
		{
			lexception = FMTexc::FMTunhandlederror;
			const FMTException newexception = this->raise(lexception, text, method, line, fil, lsection, false);
			this->throwNested(newexception, levelreference );//0);
		}
	}
	else {
		++levelreference;
		this->throwNested(newexception, levelreference);//1);
	}
}

void FMTExceptionHandler::_gutsOfExceptionLog(const std::exception& texception, const int& level)
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

void  FMTExceptionHandler::_updateWarningCount(const FMTException& p_warning)
	{
	std::unordered_map<FMTexc, size_t>::iterator spwit = m_specificwarningcount.find(p_warning.getType());
	if (spwit != m_specificwarningcount.end())
		{
			size_t wcount = spwit->second;
			if (wcount <= m_maxwarningsbeforesilenced)
			{
				*_logger << p_warning.getMessage() << "\n";
				wcount += 1;
			}
			else if (wcount == m_maxwarningsbeforesilenced + 1)
			{
				*_logger << "FMTexc(" + std::to_string(static_cast<int>(p_warning.getType())) + ") has reached the maximum number of times it can be raised." << "\n";
				wcount += 1;
			}
			spwit->second = wcount;
		}
		else {
			*_logger << p_warning.getMessage() << "\n";
			m_specificwarningcount[p_warning.getType()] = 1;
		}
	}


void FMTExceptionHandler::printExceptions(std::string text,
	const std::string& method, const int& line, const std::string& fil,
	Core::FMTsection lsection)
{
	int levelofprint = 0;
	try {
		_gutsOfPrintExceptions(text, method, line, fil, levelofprint,lsection);
	}catch (...)
	{
		if (m_threadcrashexception)
		{
			*_logger << std::string(levelofprint,' ') + "Crash on thread " << boost::lexical_cast<std::string>(m_crashedthreadid) << "\n";
			try {
				std::rethrow_exception(m_threadcrashexception);
			}
			catch (...)
			{
				
				_gutsOfPrintExceptions(text, method, line, fil, levelofprint, lsection,false);
			}
		}
		throw;
	}
}
}



BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTExceptionHandler)
