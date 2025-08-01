#pragma once
#include <functional>
#include <string>

// ===== CallbackManager.h - Header complet =====
#pragma once
#include <functional>
#include <string>

namespace FMTWrapperCore 
{
    /**
     * Types de messages pour différencier les destinations
     */
    enum class MessageType 
    {
        UI_MESSAGE,      // Messages pour l'interface utilisateur (RetourJson)
        LOGGER_MESSAGE,  // Messages pour le logger (*logger)
        DEBUG_MESSAGE    // Messages de débogage (idk si ça peut être utile)
    };
    
    /**
     * Gestionnaire centralisé des callbacks
     * Thread-safe et conçu pour la remontée en temps réel
     */
    class CallbackManager 
    {
    private:
        // Callbacks séparés pour chaque type de message
        static thread_local std::function<void(const std::string&)> uiCallback;
        static thread_local std::function<void(const std::string&)> loggerCallback;
        static thread_local std::function<void(const std::string&)> debugCallback;
        
    public:
        /**
         * Enregistre le callback pour les messages d'interface utilisateur
         * Ces messages iront vers RetourJson pour affichage immédiat
         */
        static void setUICallback(std::function<void(const std::string&)> callback);
        
        /**
         * Enregistre le callback pour les messages de logger technique
         * Ces messages iront vers votre *logger existant
         */
        static void setLoggerCallback(std::function<void(const std::string&)> callback);
        
        /**
         * Enregistre le callback pour les messages de débogage (optionnel)
         */
        static void setDebugCallback(std::function<void(const std::string&)> callback);
        
        /**
         * Envoie un message selon le type spécifié
         * Garantit l'appel immédiat du callback (I hope)
         */
        static void reportMessage(MessageType type, const std::string& message);
        
        /**
         * Méthodes de convenance pour simplifier l'utilisation
         */
        static void reportToUI(const std::string& message);
        static void reportToLogger(const std::string& message);
        static void reportToDebug(const std::string& message);
        
        /**
         * Envoie le même message vers plusieurs destinations
         */
        static void reportToUIAndLogger(const std::string& message);
        
        /**
         * Vérifications de disponibilité
         */
        static bool hasUICallback();
        static bool hasLoggerCallback();
        static bool hasDebugCallback();
        
        /**
         * Nettoyage complet - CRITIQUE pour éviter les fuites
         */
        static void clearAllCallbacks();
    };
}