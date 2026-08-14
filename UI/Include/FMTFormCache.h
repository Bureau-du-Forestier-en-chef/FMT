#pragma once

#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>

#include "FMTException.h"
#include "FMTModelParser.h"

namespace Models
{
	class FMTModel;
}

namespace Exception
{
	class FMTExceptionHandler;
}

namespace Wrapper
{
	class FMTFormLogger;
	class FMTExceptionHandlerWarning;

	// DocString: FMTFormCache
	/**
	@brief Singleton cache used by the graphical interface.

	This class maintains loaded models, logger configuration and
	exception handler configuration. It also provides recovery
	mechanisms for rebuilding logging and exception handling resources
	after a crash.
	*/
	class FMTFormCache
	{
	public:

		// DocString: ~FMTFormCache()
		/**
		@brief Default destructor.
		*/
		~FMTFormCache() = default;

		// DocString: FMTFormCache::getModel
		/**
		@brief Returns a model stored in the cache.

		@param[in] index Index of the model.

		@return The requested model.
		*/
		const Models::FMTModel& getModel(const int& index) const;

		// DocString: FMTFormCache::GetFormHandler
		/**
		@brief Returns the graphical exception handler.

		@return Pointer to the cached FMTExceptionHandlerWarning instance.
		*/
		FMTExceptionHandlerWarning* GetFormHandler();

		// DocString: FMTFormCache::GetFormHandler const
		/**
		@brief Returns the graphical exception handler.

		@return Constant pointer to the cached FMTExceptionHandlerWarning instance.
		*/
		const FMTExceptionHandlerWarning* GetFormHandler() const;

		// DocString: FMTFormCache::getExceptionHandler
		/**
		@brief Returns the active exception handler.

		@return Pointer to the current exception handler.
		*/
		Exception::FMTExceptionHandler* getExceptionHandler() const;

		// DocString: FMTFormCache::GetFormLogger
		/**
		@brief Returns the graphical logger.

		@return Pointer to the cached FMTFormLogger instance.
		*/
		FMTFormLogger* GetFormLogger();

		// DocString: FMTFormCache::GetInstance
		/**
		@brief Returns the singleton instance.

		@return Pointer to the singleton cache instance.
		*/
		static FMTFormCache* GetInstance();

		// DocString: FMTFormCache::empty
		/**
		@brief Checks whether the cache contains any models.

		@return True if the cache is empty.
		*/
		bool empty() const;

		// DocString: FMTFormCache::size
		/**
		@brief Returns the number of cached models.

		@return Number of models currently in the cache.
		*/
		size_t size() const;

		// DocString: FMTFormCache::push_back
		/**
		@brief Adds a model to the cache.

		@param[in] model Model to add.
		*/
		void push_back(const Models::FMTModel& model);

		// DocString: FMTFormCache::erase
		/**
		@brief Removes a model from the cache.

		@param[in] index Index of the model to remove.
		*/
		void erase(const int& index);

		// DocString: FMTFormCache::clear
		/**
		@brief Removes all models from the cache.
		*/
		void clear();

		// DocString: FMTFormCache::InitializeExceptionHandler
		/**
		@brief Initializes and installs the graphical exception handler.

		The supplied configuration is retained so that the exception
		handler can later be reconstructed during crash recovery.

		@param[in] maxwarnings Maximum number of warnings before silencing.
		@param[in] warning Exception types that should be treated as warnings.
		*/
		void InitializeExceptionHandler(
			const int& maxwarnings,
			const std::vector<Exception::FMTexc>& warning);

		// DocString: FMTFormCache::CloseLogger
		/**
		@brief Closes the active logger.
		*/
		void CloseLogger();

		// DocString: FMTFormCache::InitializeLogger
		/**
		@brief Initializes and installs the graphical logger.

		The logger configuration is retained so that the logger can later
		be reconstructed during crash recovery.

		@param[in] filename Log file path.
		@param[in] intptrptr Pointer to the managed logging callback.
		*/
		void InitializeLogger(
			const std::string& filename,
			System::IntPtr intptrptr);

		// DocString: FMTFormCache::RecoverLoggerAndHandler
		/**
		@brief Rebuilds the logger and exception handler after a crash.

		The reconstructed logger is reopened using the original log file
		and a fresh callback function pointer. Previously logged
		information is preserved because the log file is reopened in
		append mode.

		@param[in] intptrptr Fresh managed callback function pointer.
		*/
		void RecoverLoggerAndHandler(System::IntPtr intptrptr);

		// DocString: FMTFormCache::GetLoggerFilename
		/**
		@brief Returns the filename used by the graphical logger.

		@return The configured log file path.
		*/
		const std::string& GetLoggerFilename() const;

		// DocString: FMTFormCache::GetConfiguredParser
		/**
		@brief Creates a parser configured with the current logger and exception handler.

		Copies of the active logger and exception handler are attached to
		the returned parser instance.

		@return A configured model parser.
		*/
		Parser::FMTModelParser GetConfiguredParser() const;

		FMTFormCache(const FMTFormCache& rhs) = delete;
		FMTFormCache& operator =(const FMTFormCache& rhs) = delete;

	private:

		// DocString: FMTFormCache()
		/**
		@brief Default constructor.
		*/
		FMTFormCache() = default;

		// DocString: FMTFormCache::m_Models
		/**
		@brief Collection of models stored in the cache.
		*/
		std::vector<std::unique_ptr<Models::FMTModel>> m_Models;

		// DocString: FMTFormCache::m_Instance
		/**
		@brief Singleton instance of the cache.
		*/
		static std::unique_ptr<FMTFormCache> m_Instance;

		// DocString: FMTFormCache::m_loggerFilename
		/**
		@brief Logger filename retained for crash recovery.
		*/
		std::string m_loggerFilename;

		// DocString: FMTFormCache::m_loggerFuncPtr
		/**
		@brief Callback function pointer retained for crash recovery.
		*/
		void* m_loggerFuncPtr = nullptr;

		// DocString: FMTFormCache::m_loggerInitialized
		/**
		@brief Indicates whether the logger has been initialized.
		*/
		bool m_loggerInitialized = false;

		// DocString: FMTFormCache::m_maxwarnings
		/**
		@brief Maximum number of warnings before warning messages are silenced.
		*/
		int m_maxwarnings = 10;

		// DocString: FMTFormCache::m_warnings
		/**
		@brief Collection of exceptions that should be treated as warnings.
		*/
		std::vector<Exception::FMTexc> m_warnings;

		// DocString: FMTFormCache::m_handlerInitialized
		/**
		@brief Indicates whether the exception handler has been initialized.
		*/
		bool m_handlerInitialized = false;

		// DocString: FMTFormCache::buildLogger
		/**
		@brief Builds and installs the graphical logger using the stored configuration.
		*/
		void buildLogger();

		// DocString: FMTFormCache::buildExceptionHandler
		/**
		@brief Builds and installs the graphical exception handler using the stored configuration.

		Any previously installed graphical exception handler has its
		thread tracking information reset before reconstruction.
		*/
		void buildExceptionHandler();
	};
}