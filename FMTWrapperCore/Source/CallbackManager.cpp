#include "CallbackManager.h"

namespace FMTWrapperCore
{
    // Définitions des variables statiques thread_local
    thread_local std::function<void(const std::string &)> CallbackManager::uiCallback = nullptr;
    thread_local std::function<void(const std::string &)> CallbackManager::loggerCallback = nullptr;
    thread_local std::function<void(const std::string &)> CallbackManager::debugCallback = nullptr;

    void CallbackManager::setUICallback(std::function<void(const std::string &)> callback)
    {
        uiCallback = callback;
    }

    void CallbackManager::setLoggerCallback(std::function<void(const std::string &)> callback)
    {
        loggerCallback = callback;
    }

    void CallbackManager::setDebugCallback(std::function<void(const std::string &)> callback)
    {
        debugCallback = callback;
    }

    void CallbackManager::reportMessage(MessageType type, const std::string &message)
    {
        try
        {
            switch (type)
            {
            case MessageType::UI_MESSAGE:
                if (uiCallback)
                {
                    uiCallback(message);
                    // Optionnel: forcer le flush si nécessaire
                    // std::cout.flush(); // Décommentez si des problèmes de buffering
                }
                break;

            case MessageType::LOGGER_MESSAGE:
                if (loggerCallback)
                {
                    loggerCallback(message);
                }
                break;

            case MessageType::DEBUG_MESSAGE:
                if (debugCallback)
                {
                    debugCallback(message);
                }
                break;
            }
        }
        catch (...)
        {
            // Protection: si un callback plante, ne pas casser la simulation
            // TODO logger cette erreur ailleurs en prod
        }
    }

    void CallbackManager::reportToUI(const std::string &message)
    {
        reportMessage(MessageType::UI_MESSAGE, message);
    }

    void CallbackManager::reportToLogger(const std::string &message)
    {
        reportMessage(MessageType::LOGGER_MESSAGE, message);
    }

    void CallbackManager::reportToDebug(const std::string &message)
    {
        reportMessage(MessageType::DEBUG_MESSAGE, message);
    }

    void CallbackManager::reportToUIAndLogger(const std::string &message)
    {
        reportMessage(MessageType::UI_MESSAGE, message);
        reportMessage(MessageType::LOGGER_MESSAGE, message);
        l
    }

    bool CallbackManager::hasUICallback()
    {
        return uiCallback != nullptr;
    }

    bool CallbackManager::hasLoggerCallback()
    {
        return loggerCallback != nullptr;
    }

    bool CallbackManager::hasDebugCallback()
    {
        return debugCallback != nullptr;
    }

    void CallbackManager::clearAllCallbacks()
    {
        uiCallback = nullptr;
        loggerCallback = nullptr;
        debugCallback = nullptr;
    }
}
