#include "CallbackManager.h"

namespace FMTWrapperCore 
{
    // Définition de la variable statique thread-local
    thread_local std::function<void(const std::string&)> CallbackManager::currentReportCallback = nullptr;
    
    void CallbackManager::setReportCallback(std::function<void(const std::string&)> callback)
    {
        currentReportCallback = callback;
    }
    
    void CallbackManager::reportMessage(const std::string& message)
    {
        // Vérification de sécurité avant d'appeler le callback
        if (currentReportCallback)
        {
            try
            {
                currentReportCallback(message);
            }
            catch (...)
            {
                // En cas d'erreur dans le callback, on l'ignore pour ne pas 
                // casser la logique métier. Dans un vrai système, vous pourriez
                // logger cette erreur dans un système de logging séparé.
            }
        }
    }
    
    void CallbackManager::clearCallback()
    {
        currentReportCallback = nullptr;
    }
    
    bool CallbackManager::hasCallback()
    {
        return currentReportCallback != nullptr;
    }
}
