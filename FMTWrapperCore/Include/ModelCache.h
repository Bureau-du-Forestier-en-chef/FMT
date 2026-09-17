#pragma once

#include <vector>
#include <string>
#include <memory>

#include "FMTException.h"
#include "FMTModelParser.h"
#include "FMTWrapperCoreExport.h"

namespace Models
{
	class FMTModel;
}

namespace Exception
{
	class FMTExceptionHandler;
}

namespace FMTWrapperCore
{
	class CallbackLogger;
	class WarningExceptionHandler;

	// DocString: ModelCache
	/**
	@brief Singleton cache used by the graphical interface.

	This class maintains loaded models, logger configuration and
	exception handler configuration. It also provides recovery
	mechanisms for rebuilding logging and exception handling resources
	after a crash.
	*/
	class FMT_WRAPPER_CORE_EXPORT ModelCache
	{
	public:

		// DocString: ~ModelCache()
		/**
		@brief Destructor.

		Declared here but defined in the .cpp: m_Models holds
		std::unique_ptr<Models::FMTModel> while this header only knows
		FMTModel through a forward declaration. Since the class is exported,
		an inline destructor would be instantiated in every translation unit
		that sees the class exported, and none of them can delete an
		incomplete type.
		*/
		~ModelCache();

		// DocString: ModelCache::getModel
		/**
		@brief Returns a model stored in the cache.

		@param[in] index Index of the model.

		@return The requested model.
		*/
		const Models::FMTModel& getModel(const int& index) const;

		// DocString: ModelCache::GetWarningHandler
		/**
		@brief Returns the graphical exception handler.

		@return Pointer to the cached WarningExceptionHandler instance.
		*/
		WarningExceptionHandler* GetWarningHandler();

		// DocString: ModelCache::GetWarningHandler const
		/**
		@brief Returns the graphical exception handler.

		@return Constant pointer to the cached WarningExceptionHandler instance.
		*/
		const WarningExceptionHandler* GetWarningHandler() const;

		// DocString: ModelCache::getExceptionHandler
		/**
		@brief Returns the active exception handler.

		@return Pointer to the current exception handler.
		*/
		Exception::FMTExceptionHandler* getExceptionHandler() const;

		// DocString: ModelCache::GetCallbackLogger
		/**
		@brief Returns the graphical logger.

		@return Pointer to the cached CallbackLogger instance.
		*/
		CallbackLogger* GetCallbackLogger();

		// DocString: ModelCache::GetInstance
		/**
		@brief Returns the singleton instance.

		@return Pointer to the singleton cache instance.
		*/
		static ModelCache* GetInstance();

		// DocString: ModelCache::empty
		/**
		@brief Checks whether the cache contains any models.

		@return True if the cache is empty.
		*/
		bool empty() const;

		// DocString: ModelCache::size
		/**
		@brief Returns the number of cached models.

		@return Number of models currently in the cache.
		*/
		size_t size() const;

		// DocString: ModelCache::push_back
		/**
		@brief Adds a model to the cache.

		@param[in] model Model to add.
		*/
		void push_back(const Models::FMTModel& model);

		// DocString: ModelCache::erase
		/**
		@brief Removes a model from the cache.

		@param[in] index Index of the model to remove.
		*/
		void erase(const int& index);

		// DocString: ModelCache::clear
		/**
		@brief Removes all models from the cache.
		*/
		void clear();

		// DocString: ModelCache::InitializeExceptionHandler
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

		// DocString: ModelCache::CloseLogger
		/**
		@brief Closes the active logger.
		*/
		void CloseLogger();

		// DocString: ModelCache::InitializeLogger
		/**
		@brief Initializes and installs the graphical logger.

		The logger configuration is retained so that the logger can later
		be reconstructed during crash recovery.

		@param[in] filename Log file path.
		@param[in] intptrptr Pointer to the managed logging callback.
		*/
		void InitializeLogger(
			const std::string& filename,
			void* intptrptr);

		// DocString: ModelCache::RecoverLoggerAndHandler
		/**
		@brief Rebuilds the logger and exception handler after a crash.

		The reconstructed logger is reopened using the original log file
		and a fresh callback function pointer. Previously logged
		information is preserved because the log file is reopened in
		append mode.

		@param[in] intptrptr Fresh managed callback function pointer.
		*/
		void RecoverLoggerAndHandler(void* intptrptr);

		// DocString: ModelCache::GetLoggerFilename
		/**
		@brief Returns the filename used by the graphical logger.

		@return The configured log file path.
		*/
		const std::string& GetLoggerFilename() const;

		ModelCache(const ModelCache& rhs) = delete;
		ModelCache& operator =(const ModelCache& rhs) = delete;

	private:

		// DocString: ModelCache()
		/**
		@brief Default constructor.

		Defined in the .cpp for the same reason as the destructor: if the
		construction of a member declared after m_Models fails, stack
		unwinding destroys m_Models, which requires a complete FMTModel.
		*/
		ModelCache();

		// DocString: ModelCache::m_Models
		/**
		@brief Collection of models stored in the cache.
		*/
		std::vector<std::unique_ptr<Models::FMTModel>> m_Models;

		// DocString: ModelCache::m_Instance
		/**
		@brief Singleton instance of the cache.
		*/
		static std::unique_ptr<ModelCache> m_Instance;

		// DocString: ModelCache::m_loggerFilename
		/**
		@brief Logger filename retained for crash recovery.
		*/
		std::string m_loggerFilename;

		// DocString: ModelCache::m_loggerFuncPtr
		/**
		@brief Callback function pointer retained for crash recovery.
		*/
		void* m_loggerFuncPtr = nullptr;

		// DocString: ModelCache::m_loggerInitialized
		/**
		@brief Indicates whether the logger has been initialized.
		*/
		bool m_loggerInitialized = false;

		// DocString: ModelCache::m_maxwarnings
		/**
		@brief Maximum number of warnings before warning messages are silenced.
		*/
		int m_maxwarnings = 10;

		// DocString: ModelCache::m_warnings
		/**
		@brief Collection of exceptions that should be treated as warnings.
		*/
		std::vector<Exception::FMTexc> m_warnings;

		// DocString: ModelCache::m_handlerInitialized
		/**
		@brief Indicates whether the exception handler has been initialized.
		*/
		bool m_handlerInitialized = false;

		// DocString: ModelCache::buildLogger
		/**
		@brief Builds and installs the graphical logger using the stored configuration.
		*/
		void buildLogger();

		// DocString: ModelCache::buildExceptionHandler
		/**
		@brief Builds and installs the graphical exception handler using the stored configuration.

		Any previously installed graphical exception handler has its
		thread tracking information reset before reconstruction.
		*/
		void buildExceptionHandler();
	};
}