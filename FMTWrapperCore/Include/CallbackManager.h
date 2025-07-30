#pragma once
#include <functional>
#include <string>

namespace FMTWrapperCore
{
    /**
     * Gestionnaire centralisé pour les callbacks de messages.
     * Cette classe agit comme un pont de communication entre le code C++ pur
     * et l'interface, sans dépendre d'aucune logique métier spécifique.
     */
    class CallbackManager 
    {
    private:
        // Stockage du callback actuel - thread-safe pour les applications multi-thread
        static thread_local std::function<void(const std::string&)> currentReportCallback;
        
    public:
        /**
         * Enregistre un callback pour recevoir les messages de rapport.
         * @param callback Fonction qui sera appelée avec chaque message
         */
        static void setReportCallback(std::function<void(const std::string&)> callback);
        
        /**
         * Envoie un message via le callback enregistré.
         * Si aucun callback n'est enregistré, le message est ignoré silencieusement.
         * @param message Le message à transmettre
         */
        static void reportMessage(const std::string& message);
        
        /**
         * Nettoie le callback actuel.
         * Important à appeler en fin de traitement pour éviter les fuites de mémoire.
         */
        static void clearCallback();
        
        /**
         * Vérifie si un callback est actuellement enregistré.
         * @return true si un callback est disponible
         */
        static bool hasCallback();
    };
}