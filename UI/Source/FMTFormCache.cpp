#include "stdafx.h"
#include <string>

#include "FMTExceptionHandlerWarning.h"
#include "FMTDefaultExceptionHandler.h"

#include <msclr/marshal_cppstd.h>

#include "FMTFormLogger.h"
#include "FMTModel.h"
#include "FMTFormCache.h"

namespace Wrapper
{

	std::unique_ptr<FMTFormCache> FMTFormCache::m_Instance =
		std::unique_ptr<FMTFormCache>(nullptr);

	Parser::FMTModelParser FMTFormCache::GetConfiguredParser() const
	{
		Parser::FMTModelParser parser;

		try
		{
			FMTFormLogger* mainLogger =
				dynamic_cast<FMTFormLogger*>(Models::FMTModel::getLogger());

			if (mainLogger)
			{
				std::unique_ptr<Logging::FMTLogger> loggerClone =
					mainLogger->Clone();

				parser.passInLogger(loggerClone);
			}

			const FMTExceptionHandlerWarning* handler =
				GetFormHandler();

			std::unique_ptr<Exception::FMTExceptionHandler> handlerClone =
				handler->clone();

			parser.passInExceptionHandler(handlerClone);
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::GetConfiguredParser",
				__LINE__,
				__FILE__);
		}

		return parser;
	}

	FMTFormCache* FMTFormCache::GetInstance()
	{
		if (!m_Instance)
		{
			m_Instance =
				std::unique_ptr<FMTFormCache>(
					new FMTFormCache());
		}

		return m_Instance.get();
	}

	const Models::FMTModel& FMTFormCache::getModel(const int& index) const
	{
		try
		{
			if (static_cast<size_t>(index) >= m_Models.size())
			{
				getExceptionHandler()->raise(
					Exception::FMTexc::FMTrangeerror,
					"no scenario for index " + std::to_string(index) + " in cache ",
					"FMTFormCache::getModel",
					__LINE__,
					__FILE__);
			}
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::getModel",
				__LINE__,
				__FILE__);
		}

		return *m_Models.at(index).get();
	}

	FMTExceptionHandlerWarning* FMTFormCache::GetFormHandler()
	{
		FMTExceptionHandlerWarning* wwarptr =
			dynamic_cast<FMTExceptionHandlerWarning*>(
				getExceptionHandler());

		try
		{
			if (!wwarptr)
			{
				getExceptionHandler()->raise(
					Exception::FMTexc::FMTrangeerror,
					"no valid FMTExceptionHandlerWarning in cache ",
					"FMTFormCache::GetFormHandler",
					__LINE__,
					__FILE__);
			}
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::GetFormHandler",
				__LINE__,
				__FILE__);
		}

		return wwarptr;
	}

	const FMTExceptionHandlerWarning* FMTFormCache::GetFormHandler() const
	{
		const FMTExceptionHandlerWarning* wwarptr =
			dynamic_cast<const FMTExceptionHandlerWarning*>(
				getExceptionHandler());

		try
		{
			if (!wwarptr)
			{
				getExceptionHandler()->raise(
					Exception::FMTexc::FMTrangeerror,
					"no valid FMTExceptionHandlerWarning in cache ",
					"FMTFormCache::GetFormHandler",
					__LINE__,
					__FILE__);
			}
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::GetFormHandler",
				__LINE__,
				__FILE__);
		}

		return wwarptr;
	}

	FMTFormLogger* FMTFormCache::GetFormLogger()
	{
		FMTFormLogger* loggerptr =
			dynamic_cast<FMTFormLogger*>(
				Models::FMTModel::getLogger());

		try
		{
			if (!loggerptr)
			{
				getExceptionHandler()->raise(
					Exception::FMTexc::FMTrangeerror,
					"no valid FMTExceptionHandlerWarning in cache ",
					"FMTFormCache::GetFormLogger",
					__LINE__,
					__FILE__);
			}
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::GetFormLogger",
				__LINE__,
				__FILE__);
		}

		return loggerptr;
	}

	void FMTFormCache::push_back(const Models::FMTModel& model)
	{
		try
		{
			m_Models.push_back(
				std::move(
					std::unique_ptr<Models::FMTModel>(
						new Models::FMTModel(model))));
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::push_back",
				__LINE__,
				__FILE__);
		}
	}

	void FMTFormCache::erase(const int& index)
	{
		try
		{
			m_Models.erase(m_Models.begin() + index);
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::erase",
				__LINE__,
				__FILE__);
		}
	}

	void FMTFormCache::clear()
	{
		try
		{
			m_Models.clear();
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::clear",
				__LINE__,
				__FILE__);
		}
	}

	void FMTFormCache::buildExceptionHandler()
	{
		if (getExceptionHandler())
		{
			FMTExceptionHandlerWarning* old =
				dynamic_cast<FMTExceptionHandlerWarning*>(
					getExceptionHandler());

			if (old)
			{
				old->ResetThread();
			}
		}

		std::unique_ptr<Exception::FMTExceptionHandler> handler(
			new FMTExceptionHandlerWarning(m_maxwarnings));

		Models::FMTModel useLessModel;

		useLessModel.passInExceptionHandler(handler);
		useLessModel.setErrorsToWarnings(m_warnings);
		useLessModel.setTerminateStack();
		useLessModel.setAbortStack();
	}

	void FMTFormCache::InitializeExceptionHandler(
		const int& maxwarnings,
		const std::vector<Exception::FMTexc>& warnings)
	{
		try
		{
			m_maxwarnings = maxwarnings;
			m_warnings = warnings;
			m_handlerInitialized = true;

			buildExceptionHandler();
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::InitializeExceptionHandler",
				__LINE__,
				__FILE__);
		}
	}

	void FMTFormCache::buildLogger()
	{
		std::unique_ptr<Logging::FMTLogger> logger(
			new FMTFormLogger(
				m_loggerFilename,
				(logfunc)m_loggerFuncPtr));

		Models::FMTModel useLessModel;
		useLessModel.passInLogger(logger);
	}

	void FMTFormCache::InitializeLogger(
		const std::string& filename,
		System::IntPtr intptrptr)
	{
		try
		{
			m_loggerFilename = filename;
			m_loggerFuncPtr = intptrptr.ToPointer();

			// Bug fix: required for RecoverLoggerAndHandler()
			m_loggerInitialized = true;

			buildLogger();
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::InitializeLogger",
				__LINE__,
				__FILE__);
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
				"",
				"FMTFormCache::CloseLogger",
				__LINE__,
				__FILE__);
		}
	}

	const std::string& FMTFormCache::GetLoggerFilename() const
	{
		return m_loggerFilename;
	}

	void FMTFormCache::RecoverLoggerAndHandler(System::IntPtr intptrptr)
	{
		try
		{
			if (m_loggerInitialized)
			{
				m_loggerFuncPtr = intptrptr.ToPointer();
				buildLogger();
			}

			if (m_handlerInitialized)
			{
				buildExceptionHandler();
			}
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"FMTFormCache::RecoverLoggerAndHandler",
				__LINE__,
				__FILE__);
		}
	}

	Exception::FMTExceptionHandler* FMTFormCache::getExceptionHandler() const
	{
		return Models::FMTModel::getExceptionHandler();
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