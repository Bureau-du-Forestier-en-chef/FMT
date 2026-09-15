#include "ModelQuery.h"

#include <algorithm>
#include <fstream>
#include <set>

#include <boost/filesystem.hpp>

#include "FMTAction.h"
#include "FMTAreaParser.h"
#include "FMTConstraint.h"
#include "FMTForest.h"
#include "FMTLpModel.h"
#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "FMTOutput.h"
#include "FMTSchedule.h"
#include "FMTTheme.h"

namespace FMTWrapperCore
{
    namespace
    {
        // Au-delà de ce nombre de combinaisons d'attributs, la décomposition exhaustive
        // des masques est trop coûteuse et le modèle est résolu.
        constexpr size_t GET_ALL_MASKS_THRESHOLD = 1000000;
    }

    int ModelQuery::getMaxAge(const Models::FMTModel& p_model)
    {
        int result = 0;

        try
        {
            const Core::FMTYields YIELDS = p_model.getYields();
            std::vector<const Core::FMTYieldHandler*> handler;

            for (const auto& DATA : YIELDS)
            {
                if (DATA.second->getType() == Core::FMTyldtype::FMTageyld)
                {
                    handler.push_back(DATA.second.get());
                }
            }

            result = YIELDS.getMaxBase(handler);
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::getMaxAge", __LINE__, __FILE__);
        }

        return result;
    }

    double ModelQuery::getYield(
        const Models::FMTModel& p_model,
        const std::string& p_mask,
        const std::string& p_yield,
        int p_age)
    {
        double result = 0;

        try
        {
            result = p_model.getYieldValue(p_mask, p_yield, p_age, 0);
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::getYield", __LINE__, __FILE__);
        }

        return result;
    }

    Core::FMTMask ModelQuery::getFullMask(const std::vector<Core::FMTTheme>& p_themes)
    {
        std::string mask;

        for (const Core::FMTTheme& THEME : p_themes)
        {
            mask += "? ";
        }

        // Sans thème, la chaîne est vide : pop_back() serait un comportement indéfini.
        if (!mask.empty())
        {
            mask.pop_back();
        }

        return Core::FMTMask(mask, p_themes);
    }

    std::set<std::string> ModelQuery::_getThemesDecomposition(
        const Core::FMTMask& p_mask,
        const std::vector<Core::FMTTheme>& p_themes)
    {
        std::vector<Core::FMTMask> allMasks;
        allMasks.push_back(p_mask);

        for (const Core::FMTTheme& THEME : p_themes)
        {
            std::vector<Core::FMTMask> newMasks;

            for (const Core::FMTMask& MASK : allMasks)
            {
                const std::vector<Core::FMTMask> NEW = MASK.decompose(THEME);
                newMasks.insert(newMasks.end(), NEW.begin(), NEW.end());
            }

            allMasks.swap(newMasks);
        }

        return std::set<std::string>(allMasks.begin(), allMasks.end());
    }

    std::vector<Core::FMTActualDevelopment> ModelQuery::_getRasterArea(
        const Models::FMTModel& p_model,
        const std::string& p_rasterPath)
    {
        // Le chemin doit deja se terminer par un separateur.
        const std::string AGE_RASTER_PATH = p_rasterPath + "AGE.tif";
        const std::string STANLOCK_RASTER_PATH = p_rasterPath + "STANLOCK.tif";

        // Si le stanlock est present dans le dossier raster, on doit le prendre en compte.
        bool stanlockExists = false;
        std::ifstream file(STANLOCK_RASTER_PATH);

        if (file)
        {
            stanlockExists = true;
        }

        std::vector<std::string> themesRasters;

        for (size_t i = 1; i <= p_model.getThemes().size(); ++i)
        {
            themesRasters.push_back(p_rasterPath + "THEME" + std::to_string(i) + ".tif");
        }

        Parser::FMTAreaParser areaParser;
        Spatial::FMTForest initialForestMap;

        if (!stanlockExists)
        {
            initialForestMap = areaParser.readRasters(
                p_model.getThemes(), themesRasters, AGE_RASTER_PATH, 1, 0.0001);
        }
        else
        {
            initialForestMap = areaParser.readRasters(
                p_model.getThemes(), themesRasters, AGE_RASTER_PATH, 1, 0.0001,
                STANLOCK_RASTER_PATH);
        }

        return initialForestMap.getArea();
    }

    std::set<std::string> ModelQuery::getAllMasks(
        const Models::FMTModel& p_model,
        const int p_periods,
        const std::vector<int>& p_themesNumbers,
        const std::string& p_rasterPath)
    {
        std::set<std::string> masks;

        try
        {
            // Copie du modele : setArea avec les donnees du raster le modifie.
            Models::FMTModel modelCopy = p_model;
            modelCopy.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, false);

            if (!p_rasterPath.empty())
            {
                modelCopy.setArea(_getRasterArea(modelCopy, p_rasterPath));
            }

            std::vector<Core::FMTTheme> themes;
            size_t numberOfAttributes = 1;
            const std::vector<Core::FMTTheme> THEMESMODELS = modelCopy.getThemes();

            for (const int THEME_NUMBER : p_themesNumbers)
            {
                themes.push_back(THEMESMODELS.at(THEME_NUMBER - 1));
                numberOfAttributes *= themes.back().size();
            }

            if (numberOfAttributes > GET_ALL_MASKS_THRESHOLD)
            {
                modelCopy.setConstraints(std::vector<Core::FMTConstraint>());
                modelCopy.setParameter(Models::FMTintmodelparameters::LENGTH, 1);

                std::vector<Core::FMTActualDevelopment> area = modelCopy.getArea();

                for (int period = 1; period <= p_periods; ++period)
                {
                    Models::FMTLpModel optModel(modelCopy, Models::FMTSolverInterface::MOSEK);

                    for (auto& dev : area)
                    {
                        dev.setPeriod(period - 1);
                    }

                    optModel.setArea(area);
                    optModel.doPlanning(false);

                    const std::set<std::string> TEMP_MASKS = optModel.getAllMasks(themes);
                    masks.insert(TEMP_MASKS.begin(), TEMP_MASKS.end());

                    area = optModel.getPotentialArea(period + 1);
                }
            }
            else
            {
                const Core::FMTMask PLAIN_MASK = getFullMask(THEMESMODELS);
                masks = _getThemesDecomposition(PLAIN_MASK, themes);
            }
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::getAllMasks", __LINE__, __FILE__);
        }

        return masks;
    }

    bool ModelQuery::validateMask(const Models::FMTModel& p_model, const std::string& p_mask)
    {
        bool valid = false;

        try
        {
            // validate() prend le masque par reference non-const : il le tronque
            // s'il y a moins de themes que d'attributs dans la chaine. Le wrapper
            // passait deja une copie locale et ignorait cette modification.
            std::string mask = p_mask;
            valid = Core::FMTTheme::validate(p_model.getThemes(), mask);
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::validateMask", __LINE__, __FILE__);
        }

        return valid;
    }

    std::vector<std::string> ModelQuery::getConstraintsAsText(const Models::FMTModel& p_model)
    {
        std::vector<std::string> constraints;

        try
        {
            for (const Core::FMTConstraint& CONSTRAINT : p_model.getConstraints())
            {
                constraints.push_back(std::string(CONSTRAINT));
            }
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::getConstraintsAsText", __LINE__, __FILE__);
        }

        return constraints;
    }

    std::vector<std::string> ModelQuery::getOutputsNames(const Models::FMTModel& p_model)
    {
        std::vector<std::string> names;

        try
        {
            for (const Core::FMTOutput& OUTPUT : p_model.getOutputs())
            {
                names.push_back(OUTPUT.getName());
            }
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::getOutputsNames", __LINE__, __FILE__);
        }

        return names;
    }

    std::vector<std::string> ModelQuery::getActionsNames(const Models::FMTModel& p_model)
    {
        std::vector<std::string> names;

        try
        {
            for (const Core::FMTAction& ACTION : p_model.getActions())
            {
                names.push_back(ACTION.getName());
            }
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::getActionsNames", __LINE__, __FILE__);
        }

        return names;
    }

    std::vector<std::string> ModelQuery::getAggregates(const Models::FMTModel& p_model)
    {
        std::vector<std::string> aggregates;

        try
        {
            std::set<std::string> unique;

            for (const Core::FMTAction& ACTION : p_model.getActions())
            {
                for (const std::string& AGGREGATE : ACTION.getAggregates())
                {
                    unique.insert(AGGREGATE);
                }
            }

            aggregates.assign(unique.begin(), unique.end());
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::getAggregates", __LINE__, __FILE__);
        }

        return aggregates;
    }

    std::vector<std::string> ModelQuery::getYieldsNames(const Models::FMTModel& p_model)
    {
        std::vector<std::string> names;

        try
        {
            names = p_model.getYields().getAllYieldNames();
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::getYieldsNames", __LINE__, __FILE__);
        }

        return names;
    }

    int ModelQuery::getThemesCount(const Models::FMTModel& p_model)
    {
        int count = 0;

        try
        {
            count = static_cast<int>(p_model.getThemes().size());
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::getThemesCount", __LINE__, __FILE__);
        }

        return count;
    }

    std::vector<std::string> ModelQuery::getThemeAttributes(
        const Models::FMTModel& p_model,
        const int p_themeIndex)
    {
        std::vector<std::string> attributes;

        try
        {
            // getThemes() retourne par valeur et getBaseAttributes() une reference
            // dans le theme : garder le vecteur en local plutot que de chainer.
            const std::vector<Core::FMTTheme> THEMES = p_model.getThemes();
            attributes = THEMES.at(p_themeIndex).getBaseAttributes();
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::getThemeAttributes", __LINE__, __FILE__);
        }

        return attributes;
    }

    std::vector<Core::FMTSchedule> ModelQuery::readSchedules(
        const std::string& p_primaryFilePath,
        const Models::FMTModel& p_model)
    {
        std::vector<Core::FMTSchedule> schedules;

        try
        {
            Parser::FMTModelParser modelParser;
            std::vector<Models::FMTModel> models;
            models.push_back(p_model);

            schedules = modelParser.readSchedules(p_primaryFilePath, models).at(0);
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::readSchedules", __LINE__, __FILE__);
        }

        return schedules;
    }

    int ModelQuery::getPeriodsCount(
        const std::string& p_primaryFilePath,
        const Models::FMTModel& p_model)
    {
        const std::vector<Core::FMTSchedule> SCHEDULES =
            readSchedules(p_primaryFilePath, p_model);

        // Sans cette garde, back() sur un vecteur vide est un comportement indefini.
        if (SCHEDULES.empty())
        {
            return 0;
        }

        return SCHEDULES.back().getPeriod();
    }

    void ModelQuery::writeToProject(
        const std::vector<Models::FMTModel>& p_models,
        const std::string& p_destinationDirectory)
    {
        if (p_models.empty())
        {
            return;
        }

        try
        {
            const std::string BASENAME = p_models.front().getName();

            if (!boost::filesystem::is_directory(p_destinationDirectory))
            {
                boost::filesystem::create_directories(p_destinationDirectory);
            }

            const boost::filesystem::path PRIMARY_PATH =
                boost::filesystem::path(p_destinationDirectory) / (BASENAME + ".pri");

            Parser::FMTModelParser modelParser;

            for (const Models::FMTModel& MODEL : p_models)
            {
                modelParser.writeToProject(PRIMARY_PATH.string(), MODEL);
            }
        }
        catch (...)
        {
            p_models.front().getExceptionHandler()->raiseFromCatch(
                "", "ModelQuery::writeToProject", __LINE__, __FILE__);
        }
    }
}
