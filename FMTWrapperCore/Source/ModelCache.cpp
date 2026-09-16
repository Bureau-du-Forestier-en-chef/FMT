#include <string>

#include "WarningExceptionHandler.h"
#include "FMTDefaultExceptionHandler.h"


#include "CallbackLogger.h"
#include "FMTModel.h"
#include "ModelCache.h"

namespace FMTWrapperCore
{

	std::unique_ptr<ModelCache> ModelCache::m_Instance =
		std::unique_ptr<ModelCache>(nullptr);

	// Defined here, where FMTModel.h is included: building or destroying m_Models
	// (a vector of unique_ptr<FMTModel>) requires the complete type.
	ModelCache::ModelCache() = default;

	ModelCache::~ModelCache() = default;

	ModelCache* ModelCache::GetInstance()
	{
		if (!m_Instance)
		{
			m_Instance =
				std::unique_ptr<ModelCache>(
					new ModelCache());
		}

		return m_Instance.get();
	}

	const Models::FMTModel& ModelCache::getModel(const int& index) const
	{
		try
		{
			if (static_cast<size_t>(index) >= m_Models.size())
			{
				getExceptionHandler()->raise(
					Exception::FMTexc::FMTrangeerror,
					"no scenario for index " + std::to_string(index) + " in cache ",
					"ModelCache::getModel",
					__LINE__,
					__FILE__);
			}
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"ModelCache::getModel",
				__LINE__,
				__FILE__);
		}

		return *m_Models.at(index).get();
	}

	WarningExceptionHandler* ModelCache::GetWarningHandler()
	{
		WarningExceptionHandler* wwarptr =
			dynamic_cast<WarningExceptionHandler*>(
				getExceptionHandler());

		try
		{
			if (!wwarptr)
			{
				getExceptionHandler()->raise(
					Exception::FMTexc::FMTrangeerror,
					"no valid WarningExceptionHandler in cache ",
					"ModelCache::GetWarningHandler",
					__LINE__,
					__FILE__);
			}
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"ModelCache::GetWarningHandler",
				__LINE__,
				__FILE__);
		}

		return wwarptr;
	}

	const WarningExceptionHandler* ModelCache::GetWarningHandler() const
	{
		const WarningExceptionHandler* wwarptr =
			dynamic_cast<const WarningExceptionHandler*>(
				getExceptionHandler());

		try
		{
			if (!wwarptr)
			{
				getExceptionHandler()->raise(
					Exception::FMTexc::FMTrangeerror,
					"no valid WarningExceptionHandler in cache ",
					"ModelCache::GetWarningHandler",
					__LINE__,
					__FILE__);
			}
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"ModelCache::GetWarningHandler",
				__LINE__,
				__FILE__);
		}

		return wwarptr;
	}

	CallbackLogger* ModelCache::GetCallbackLogger()
	{
		CallbackLogger* loggerptr =
			dynamic_cast<CallbackLogger*>(
				Models::FMTModel::getLogger());

		try
		{
			if (!loggerptr)
			{
				getExceptionHandler()->raise(
					Exception::FMTexc::FMTrangeerror,
					"no valid WarningExceptionHandler in cache ",
					"ModelCache::GetCallbackLogger",
					__LINE__,
					__FILE__);
			}
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"ModelCache::GetCallbackLogger",
				__LINE__,
				__FILE__);
		}

		return loggerptr;
	}

	void ModelCache::push_back(const Models::FMTModel& model)
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
				"ModelCache::push_back",
				__LINE__,
				__FILE__);
		}
	}

	void ModelCache::erase(const int& index)
	{
		try
		{
			m_Models.erase(m_Models.begin() + index);
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"ModelCache::erase",
				__LINE__,
				__FILE__);
		}
	}

	void ModelCache::clear()
	{
		try
		{
			m_Models.clear();
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"ModelCache::clear",
				__LINE__,
				__FILE__);
		}
	}

	void ModelCache::buildExceptionHandler()
	{
		if (getExceptionHandler())
		{
			WarningExceptionHandler* old =
				dynamic_cast<WarningExceptionHandler*>(
					getExceptionHandler());

			if (old)
			{
				old->ResetThread();
			}
		}

		std::unique_ptr<Exception::FMTExceptionHandler> handler(
			new WarningExceptionHandler(m_maxwarnings));

		Models::FMTModel useLessModel;

		useLessModel.passInExceptionHandler(handler);
		useLessModel.setErrorsToWarnings(m_warnings);
		useLessModel.setTerminateStack();
		useLessModel.setAbortStack();
	}

	void ModelCache::InitializeExceptionHandler(
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
				"ModelCache::InitializeExceptionHandler",
				__LINE__,
				__FILE__);
		}
	}

	void ModelCache::buildLogger()
	{
		std::unique_ptr<Logging::FMTLogger> logger(
			new CallbackLogger(
				m_loggerFilename,
				(logfunc)m_loggerFuncPtr));

		Models::FMTModel useLessModel;
		useLessModel.passInLogger(logger);
	}

	void ModelCache::InitializeLogger(
		const std::string& filename,
		void* intptrptr)
	{
		try
		{
			m_loggerFilename = filename;
			m_loggerFuncPtr = intptrptr;

			// Bug fix: required for RecoverLoggerAndHandler()
			m_loggerInitialized = true;

			buildLogger();
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"ModelCache::InitializeLogger",
				__LINE__,
				__FILE__);
		}
	}

	void ModelCache::CloseLogger()
	{
		try
		{
			CallbackLogger* logger = GetCallbackLogger();

			if (logger)
			{
				logger->closeFile();
			}
		}
		catch (...)
		{
			getExceptionHandler()->raiseFromCatch(
				"",
				"ModelCache::CloseLogger",
				__LINE__,
				__FILE__);
		}
	}

	const std::string& ModelCache::GetLoggerFilename() const
	{
		return m_loggerFilename;
	}

	void ModelCache::RecoverLoggerAndHandler(void* intptrptr)
	{
		try
		{
			if (m_loggerInitialized)
			{
				m_loggerFuncPtr = intptrptr;
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
				"ModelCache::RecoverLoggerAndHandler",
				__LINE__,
				__FILE__);
		}
	}

	Exception::FMTExceptionHandler* ModelCache::getExceptionHandler() const
	{
		return Models::FMTModel::getExceptionHandler();
	}

	bool ModelCache::empty() const
	{
		return m_Models.empty();
	}

	size_t ModelCache::size() const
	{
		return m_Models.size();
	}

}