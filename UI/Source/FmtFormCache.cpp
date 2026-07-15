#include "stdafx.h"
#include <string>
#include "FMTexceptionhandlerwarning.h"
#include "FMTdefaultexceptionhandler.h"
#include <msclr\marshal_cppstd.h>
#include "FMTFormLogger.h"
#include "FMTmodel.h"
#include "FMTFormCache.h"

namespace Wrapper
{
std::unique_ptr<FMTFormCache> FMTFormCache::m_Instance = std::unique_ptr<FMTFormCache>(nullptr);

Parser::FMTmodelparser FMTFormCache::GetConfiguredParser() const
{
	Parser::FMTmodelparser parser;
	try
	{
		FMTFormLogger* mainLogger =
			dynamic_cast<FMTFormLogger*>(Models::FMTmodel::getLogger());
		if (mainLogger)
		{
			std::unique_ptr<Logging::FMTlogger> loggerClone =
				mainLogger->Clone();
			parser.passInLogger(loggerClone);
		}
		const FMTexceptionhandlerwarning* handler = GetFormHandler();
		std::unique_ptr<Exception::FMTexceptionhandler> handlerClone =
			handler->Clone();
		parser.passInExceptionHandler(handlerClone);
	}
	catch (...)
	{
		getExceptionHandler()->raiseFromCatch(
			"", "FMTFormCache::GetConfiguredParser", __LINE__, __FILE__);
	}
	return parser;
}

FMTFormCache* FMTFormCache::GetInstance()
	{
	if (!m_Instance)
		{
		m_Instance = std::unique_ptr<FMTFormCache>(new FMTFormCache());
		}
	return m_Instance.get();
	}


const Models::FMTmodel& FMTFormCache::getModel(const int& index) const
	{
	try {
		if (static_cast<size_t>(index)>= m_Models.size())
		{
			getExceptionHandler()->raise(
				Exception::FMTexc::FMTrangeerror, "no scenario for index " + std::to_string(index) + " in cache ",
				"FMTFormCache::getModel", __LINE__, __FILE__);
		}

	}catch (...)
		{
		getExceptionHandler()->raiseFromCatch("", "FMTFormCache::getModel", __LINE__, __FILE__);
		}
	return *m_Models.at(index).get();
	}


FMTexceptionhandlerwarning* FMTFormCache::GetFormHandler()
{
	FMTexceptionhandlerwarning* wwarptr = dynamic_cast<FMTexceptionhandlerwarning*>(getExceptionHandler());
	try {
		if (!wwarptr)
		{
			getExceptionHandler()->raise(
				Exception::FMTexc::FMTrangeerror, "no valid FMTexceptionhandlerwarning in cache ",
				"FMTFormCache::GetFormHandler", __LINE__, __FILE__);
		}
	}catch (...)
		{
		getExceptionHandler()->raiseFromCatch("", "FMTFormCache::GetFormHandler", __LINE__, __FILE__);
		}
	return wwarptr;
}

const FMTexceptionhandlerwarning* FMTFormCache::GetFormHandler() const
{
	const FMTexceptionhandlerwarning* wwarptr =
		dynamic_cast<const FMTexceptionhandlerwarning*>(getExceptionHandler());
	try {
		if (!wwarptr)
		{
			getExceptionHandler()->raise(
				Exception::FMTexc::FMTrangeerror,
				"no valid FMTexceptionhandlerwarning in cache ",
				"FMTFormCache::GetFormHandler", __LINE__, __FILE__);
		}
	}
	catch (...) {
		getExceptionHandler()->raiseFromCatch(
			"", "FMTFormCache::GetFormHandler", __LINE__, __FILE__);
	}
	return wwarptr;
}

FMTFormLogger*  FMTFormCache::GetFormLogger()
{
	FMTFormLogger* loggerptr = dynamic_cast<FMTFormLogger*>(Models::FMTmodel::getLogger());
	try{
		if (!loggerptr)
		{
			getExceptionHandler()->raise(
				Exception::FMTexc::FMTrangeerror, "no valid FMTexceptionhandlerwarning in cache ",
				"FMTFormCache::GetFormLogger", __LINE__, __FILE__);
		}
	}catch (...)
	{
		getExceptionHandler()->raiseFromCatch("", "FMTFormCache::GetFormLogger", __LINE__, __FILE__);
	}
	return loggerptr;
}


void FMTFormCache::push_back(const Models::FMTmodel& model)
{
	try {
		m_Models.push_back(std::move(std::unique_ptr<Models::FMTmodel>(new Models::FMTmodel(model))));
	}catch (...)
		{
			getExceptionHandler()->raiseFromCatch("", "FMTFormCache::push_back", __LINE__, __FILE__);
		}
		
}
void FMTFormCache::erase(const int& index)
{
	try{
		m_Models.erase(m_Models.begin() + index);
}catch (...)
	{
		getExceptionHandler()->raiseFromCatch("", "FMTFormCache::erase", __LINE__, __FILE__);
	}
}
void FMTFormCache::clear()
{
	try{
		m_Models.clear();
	}catch (...)
	{
		getExceptionHandler()->raiseFromCatch("", "FMTFormCache::clear", __LINE__, __FILE__);
	}
}

void FMTFormCache::buildExceptionHandler()
	{
	if (getExceptionHandler())
	{
		FMTexceptionhandlerwarning* old = dynamic_cast<FMTexceptionhandlerwarning*>(getExceptionHandler());
		if (old)
		{
			old->ResetThread();
		}
	}

	std::unique_ptr<Exception::FMTexceptionhandler> handler(new FMTexceptionhandlerwarning(m_maxwarnings));
	Models::FMTmodel useLessModel;
	useLessModel.passInExceptionHandler(handler);
	useLessModel.setErrorsToWarnings(m_warnings);
	useLessModel.setTerminateStack();
	useLessModel.setAbortStack();
	}

void FMTFormCache::InitializeExceptionHandler(const int& maxwarnings,const std::vector<Exception::FMTexc>& warnings)
	{
	try {
		m_maxwarnings = maxwarnings;
		m_warnings = warnings;
		m_handlerInitialized = true;
		buildExceptionHandler();
	}
	catch (...)
		{
		getExceptionHandler()->raiseFromCatch("", "FMTFormCache::InitializeExceptionHandler", __LINE__, __FILE__);
		}
	}

void FMTFormCache::buildLogger()
	{
	std::unique_ptr<Logging::FMTlogger> logger(new FMTFormLogger(m_loggerFilename, (logfunc)m_loggerFuncPtr));
	Models::FMTmodel useLessModel;
	useLessModel.passInLogger(logger);
	}

void FMTFormCache::InitializeLogger(const std::string& filename, System::IntPtr intptrptr)
	{
	try{
		m_loggerFilename = filename;
		m_loggerFuncPtr = intptrptr.ToPointer();
		buildLogger();
	}catch (...)
		{
		getExceptionHandler()->raiseFromCatch("", "FMTFormCache::InitializeLogger", __LINE__, __FILE__);
		}
	}
	
void FMTFormCache::CloseLogger()
{
	try
	{
		FMTFormLogger* logger = GetFormLogger();

		if (logger)
		{
			logger->closeFile();
		}
	}
	catch (...)
	{
		getExceptionHandler()->raiseFromCatch(
			"", "FMTFormCache::CloseLogger", __LINE__, __FILE__);
	}
}

const std::string& FMTFormCache::GetLoggerFilename() const
	{
	return m_loggerFilename;
	}

void FMTFormCache::RecoverLoggerAndHandler(System::IntPtr intptrptr)
	{
	try{
		// Apres un crash, le logger et/ou le handler peuvent avoir ete remplaces ou
		// laisses dans un etat instable. On les reconstruit a partir de la config
		// figee. Le fichier log est rouvert en mode append (setToFile) : le contenu
		// existant est conserve. On rafraichit le pointeur de fonction avec le
		// delegue frais fourni (l'ancien peut etre invalide).
		if (m_loggerInitialized)
		{
			m_loggerFuncPtr = intptrptr.ToPointer();
			buildLogger();
		}
		if (m_handlerInitialized)
		{
			buildExceptionHandler();
		}
	}catch (...)
		{
		getExceptionHandler()->raiseFromCatch("", "FMTFormCache::RecoverLoggerAndHandler", __LINE__, __FILE__);
		}
	}

Exception::FMTexceptionhandler* FMTFormCache::getExceptionHandler() const
{
	return Models::FMTmodel::getExceptionHandler();
}

bool FMTFormCache::empty() const
	{
	return m_Models.empty();
	}

size_t FMTFormCache::size() const
	{
	return m_Models.size();
	}

}
