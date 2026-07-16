#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include "FMTexception.h"
#include "FMTmodelparser.h"

namespace Models
{
	class FMTmodel;
}

namespace Exception
{
	class FMTexceptionhandler;
}

namespace Wrapper
{
	class FMTFormLogger;
	class FMTexceptionhandlerwarning;
	class FMTFormCache
	{
		public:
			~FMTFormCache()=default;
			const Models::FMTmodel& getModel(const int& index) const;
			FMTexceptionhandlerwarning* GetFormHandler();
			const FMTexceptionhandlerwarning* GetFormHandler() const;
			Exception::FMTexceptionhandler* getExceptionHandler() const;
			FMTFormLogger* GetFormLogger();
			static FMTFormCache* GetInstance();
			bool empty() const;
			size_t size() const;
			void push_back(const Models::FMTmodel& model);
			void erase(const int& index);
			void clear();
			void InitializeExceptionHandler(const int& maxwarnings, const std::vector<Exception::FMTexc>& warning);
			void CloseLogger();
			void InitializeLogger(const std::string& filename,System::IntPtr intptrptr);
			// Reconstruit le logger ET l'exception handler a partir de la configuration
			// enregistree lors des Initialize*, suite a un crash. Le logger est rouvert
			// sur le MEME fichier (mode append) : aucune perte du log existant.
			// 'intptrptr' est un pointeur de fonction FRAIS (nouveau delegue manage) :
			// l'ancien peut etre invalide apres un crash.
			void RecoverLoggerAndHandler(System::IntPtr intptrptr);
			// Nom du fichier log fige a l'initialisation (vide si non initialise).
			// Permet d'ecrire directement dans le log en dernier recours, sans
			// dependre de l'objet logger (qui peut etre invalide apres un crash).
			const std::string& GetLoggerFilename() const;
			Parser::FMTmodelparser GetConfiguredParser() const;
			FMTFormCache(const FMTFormCache& rhs) = delete;
			FMTFormCache& operator =(const FMTFormCache& rhs) = delete;
		private:
			FMTFormCache()=default;
			std::vector<std::unique_ptr<Models::FMTmodel>> m_Models;
			static std::unique_ptr<FMTFormCache> m_Instance;

			// Configuration du logger figee a la premiere initialisation : la location
			// du log ne doit JAMAIS changer durant une run. Conservee pour pouvoir
			// reconstruire le logger apres un crash (RecoverLoggerAndHandler).
			std::string m_loggerFilename;
			void* m_loggerFuncPtr = nullptr;
			bool m_loggerInitialized = false;

			// Configuration de l'exception handler, conservee pour la reconstruction.
			int m_maxwarnings = 10;
			std::vector<Exception::FMTexc> m_warnings;
			bool m_handlerInitialized = false;

			// Construit et installe un FMTFormLogger sur le fichier/func ptr stockes.
			void buildLogger();
			// Construit et installe un FMTexceptionhandlerwarning sur la config stockee.
			void buildExceptionHandler();
			
			

	};
}