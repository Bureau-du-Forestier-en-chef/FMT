#include "Selection.h"

#include <algorithm>

#include "FMTFreeExceptionHandler.h"

namespace FMTWrapper::Backend
{
    std::string Selection::_normalize(const std::string& p_text)
    {
        std::string normalized = p_text;

        normalized.erase(
            std::remove(normalized.begin(), normalized.end(), '\r'),
            normalized.cend());

        normalized.erase(
            std::remove(normalized.begin(), normalized.end(), '\n'),
            normalized.cend());

        const size_t FIRST = normalized.find_first_not_of(" \t");

        if (FIRST == std::string::npos)
        {
            return std::string();
        }

        const size_t LAST = normalized.find_last_not_of(" \t");

        return normalized.substr(FIRST, LAST - FIRST + 1);
    }

    std::vector<Core::FMTConstraint> Selection::selectConstraints(
        const std::vector<Core::FMTConstraint>& p_allConstraints,
        const std::vector<std::string>& p_selectedNames)
    {
        std::vector<Core::FMTConstraint> selected;

        try
        {
            std::vector<std::string> normalizedNames;
            normalizedNames.reserve(p_selectedNames.size());

            for (const std::string& NAME : p_selectedNames)
            {
                normalizedNames.push_back(_normalize(NAME));
            }

            for (const Core::FMTConstraint& CONSTRAINT : p_allConstraints)
            {
                const std::string TEXT = _normalize(std::string(CONSTRAINT));

                if (std::find(normalizedNames.begin(), normalizedNames.end(), TEXT)
                    != normalizedNames.end())
                {
                    selected.push_back(CONSTRAINT);
                }
            }
        }
        catch (const std::exception& e)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                e.what(), "Selection::selectConstraints", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                "Unknown error", "Selection::selectConstraints", __LINE__, __FILE__);
        }

        return selected;
    }

    std::vector<Core::FMTOutput> Selection::selectOutputs(
        const std::vector<Core::FMTOutput>& p_allOutputs,
        const std::vector<std::string>& p_selectedNames)
    {
        std::vector<Core::FMTOutput> selected;

        try
        {
            std::vector<std::string> normalizedNames;
            normalizedNames.reserve(p_selectedNames.size());

            for (const std::string& NAME : p_selectedNames)
            {
                normalizedNames.push_back(_normalize(NAME));
            }

            for (const Core::FMTOutput& OUTPUT : p_allOutputs)
            {
                const std::string NAME = _normalize(OUTPUT.getName());

                if (std::find(normalizedNames.begin(), normalizedNames.end(), NAME)
                    != normalizedNames.end())
                {
                    selected.push_back(OUTPUT);
                }
            }
        }
        catch (const std::exception& e)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                e.what(), "Selection::selectOutputs", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                "Unknown error", "Selection::selectOutputs", __LINE__, __FILE__);
        }

        return selected;
    }

    Core::FMTOutput Selection::findOutput(
        const std::vector<Core::FMTOutput>& p_allOutputs,
        const std::string& p_name)
    {
        Core::FMTOutput found;

        try
        {
            const std::string TARGET = _normalize(p_name);

            for (const Core::FMTOutput& OUTPUT : p_allOutputs)
            {
                if (_normalize(OUTPUT.getName()) == TARGET)
                {
                    found = OUTPUT;
                    break;
                }
            }
        }
        catch (const std::exception& e)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                e.what(), "Selection::findOutput", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(
                "Unknown error", "Selection::findOutput", __LINE__, __FILE__);
        }

        return found;
    }
}
