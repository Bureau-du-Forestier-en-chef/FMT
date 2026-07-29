/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTYLD_Hm_included
#define FMTYLD_Hm_included

#include "FMTList.hpp"
#include "FMTYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTBounds.hpp"
#include <vector>
#include <map>
#include <string>
#include <boost/serialization/serialization.hpp>
#include "FMTutility.h"
#include "FMTTheme.h"
#include <unordered_map>
#include <memory>
#include <boost/serialization/export.hpp>
#include "FMTYieldRequest.h"

namespace Models
{
	class FMTModel;
	class FMTNssModel;
}


namespace Core
{
class FMTDevelopment;
class FMTAgeYieldHandler;
class FMTTimeYieldHandler;
// DocString: FMTYields
/**
@brief FMTList containing multiple yield handlers as seen in the yield section, holding the information related to forest productivity.
@details Used to check if a development is operable to an action, calculate outputs and constraints, and disturb a forest stand in a transition.
*/
class FMTEXPORT FMTYields : public FMTList<std::unique_ptr<FMTYieldHandler>>
    {
	friend class Models::FMTModel;
	friend class Models::FMTNssModel;
    public:
		// DocString: FMTYields::generateDefaultYields
		/**
		@brief Generate the default yields such as _TSLA in the yield class.
		@param[in] themes the themes.
		*/
		void generateDefaultYields(const std::vector<Core::FMTTheme>& themes);
		// DocString: FMTYields::swap
		/**
		@brief Swap this FMTYields with another.
		@param[in,out] rhs the FMTYields to swap with.
		*/
		void swap(FMTYields& rhs);
		// DocString: FMTYields::gotYieldType
		/**
		@brief Return true if the yield section has a given yield type.
		@param[in] type the yield type.
		@return true if the yield section has the type else false.
		*/
		bool gotYieldType(FMTyldtype type) const;
		// DocString: FMTYields::setActionsMappingToModelHandlers
		/**
		@brief Clear and replace the actions mapping of every yield model handler.
		@param[in] actionids the new action ids.
		*/
		void setActionsMappingToModelHandlers(const std::vector<int>& actionids);
		// DocString: FMTYields::getAllYieldNames
		/**
		@brief Return all the yield names from the list.
		@return all the yield names.
		*/
		std::vector<std::string> getAllYieldNames() const;
		// DocString: FMTYields()
		/**
		@brief Default constructor for FMTYields.
		*/
        FMTYields();
		// DocString: ~FMTYields()
		/**
		@brief Default destructor for FMTYields.
		*/
		~FMTYields()=default;
		// DocString: FMTYields(const FMTYields&)
		/**
		@brief Copy constructor for FMTYields.
		@param[in] rhs the FMTYields to copy.
		*/
        FMTYields(const FMTYields& rhs);
		// DocString: FMTYields::operator=
		/**
		@brief Copy assignment operator for FMTYields.
		@param[in] rhs the FMTYields to copy.
		@return a reference to this FMTYields.
		*/
        FMTYields& operator = (const FMTYields& rhs);
		// DocString: FMTYields::clearCache
		/**
		@brief Clear the caching from all FMTData.
		*/
		void clearCache() final;
		// DocString: FMTYields::clearRandomYieldsCache
		/**
		@brief Clear the cache of all random yield models.
		*/
		void clearRandomYieldsCache();
		// DocString: FMTYields::isYld
		/**
		@brief Return true if the yield section contains a given non null yield.
		@param[in] value the yield name.
		@param[in] fromsource if true checks the sources.
		@return true if the yield is present else false.
		*/
        bool isYld(const std::string& value,bool fromsource = false) const;
		// DocString: FMTYields::isNullYld
		/**
		@brief Return true if the yield section contains a given null yield.
		@param[in] value the yield name.
		@return true if the yield is a null yield else false.
		*/
		bool isNullYld(const std::string& value) const;
		// DocString: FMTYields::get
		/**
		@brief Get the yields values for a development from a set of yield requests.
		@return a map of yield names to their values.
		*/
		//std::vector<double>get(std::vector<FMTYieldRequest>& requests) const;
		// DocString: FMTYields::getsingle
		/**
		@brief Get the yield value for a development, looking at age, period, lock and mask.
		@return the yield value.
		*/
		//double getsingle(const FMTDevelopment& dev,const std::string& target) const;
		//std::map<std::string,double>getYlds(const FMTDevelopment& dev,const FMTSpec& spec) const;
		// DocString: FMTYields::getYlds
		/**
		@brief Get the yields used and their values for a development and a specification.
		@return the yields and their values.
		*/
		//std::vector<double>getYlds(const FMTDevelopment& dev, const FMTSpec& spec) const;
		// DocString: FMTYields::getAge
		/**
		@brief Return the new age of a development when disturbed by a transition specification.
		@param[in] request the yield request.
		@param[in] spec the specification.
		@return the new age.
		*/
        int getAge(const FMTYieldRequest& request,const FMTSpec& spec) const;
		// DocString: FMTYields::getAllYields
		/**
		@brief Return a map of all the yield values for a given yield handler type, used for generating a text file of yields for GCBM.
		@param[in] target the target theme.
		@param[in] type the yield handler type.
		@return a map of mask to yield names and their values.
		*/
		std::map<std::string, std::map<std::string, std::vector<double>>>getAllYields(const FMTTheme& target,FMTyldtype type) const;
		// DocString: FMTYields::operator==
		/**
		@brief Equality comparison operator of FMTYields.
		@param[in] rhs the yields to compare with.
		@return true if both yields are equal else false.
		*/
		bool operator == (const FMTYields& rhs) const;
		// DocString: FMTYields::operator!=
		/**
		@brief Inequality comparison operator of FMTYields.
		@param[in] rhs the yields to compare with.
		@return true if both yields are different else false.
		*/
		bool operator != (const FMTYields& rhs) const;
		// DocString: FMTYields::update
		/**
		@brief Update the yield names and null yield names caching, as required by FMTList.
		*/
        void update() override;
		// DocString: FMTYields::reserve
		/**
		@brief Reserve memory based on another FMTYields.
		@param[in] p_other the other yields.
		*/
		void reserve(const FMTYields& p_other);
		// DocString: FMTYields::presolve
		/**
		@brief Return a presolved FMTYields section, removing useless data.
		@param[in] filter the mask filter.
		@param[in] originalthemes the original themes.
		@param[in] newthemes the presolved themes.
		@return the presolved yields.
		*/
		FMTYields presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			const std::vector<FMTTheme>& newthemes) const;
		// DocString: FMTYields::presolveRef
		/**
		@brief Presolve the FMTYields section in place, removing useless data.
		@param[in] p_filter the mask filter.
		@param[in] p_originalThemes the original themes.
		@param[in] p_newThemes the presolved themes.
		*/
		void presolveRef(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_originalThemes,
			const std::vector<FMTTheme>& p_newThemes);
		// DocString: FMTYields::getStacked
		/**
		@brief Return the yields as a vector of strings.
		@return the yields as strings.
		*/
		std::vector<std::string>getStacked() const;
		// DocString: FMTYields::getFromFactor
		/**
		@brief Multiply the yields section by a factor and return a new yields section.
		@param[in] factor the factor.
		@param[in] yieldnames the yield names to multiply, all if empty.
		@return the multiplied yields.
		*/
		FMTYields getFromFactor(const double& factor,
			std::vector<std::string>yieldnames = std::vector<std::string>()) const;
		// DocString: FMTYields::get
		/**
		@brief Get the yield value for a development from a yield request.
		@param[in] request the yield request.
		@param[in] yld the yield name.
		@return the yield value.
		*/
		double get(const FMTYieldRequest& request,const std::string& yld) const;
		// DocString: FMTYields::pushBackAgeHandler
		/**
		@brief Push back an age yield handler, for R.
		@param[in] mask the mask.
		@param[in] value the age yield handler.
		*/
		void pushBackAgeHandler(const FMTMask& mask, const FMTAgeYieldHandler& value);
		// DocString: FMTYields::pushBackTimeHandler
		/**
		@brief Push back a time yield handler, for R.
		@param[in] mask the mask.
		@param[in] value the time yield handler.
		*/
		void pushBackTimeHandler(const FMTMask& mask, const FMTTimeYieldHandler& value);
		// DocString: FMTYields::getNullYield
		/**
		@brief Return the null yield.
		@return the null yield value.
		*/
		static const std::string& getNullYield();
		// DocString: FMTYields::getMaxBase
		/**
		@brief Return the maximum age seen in the yield handlers.
		@param[in] handlers the yield handlers.
		@return the maximum age among the yield handlers.
		*/
		static int getMaxBase(const std::vector<const FMTYieldHandler*>& handlers);
		// DocString: FMTYields::clear
		/**
		@brief Clear all the yield data.
		*/
		void clear();
	private:
		// DocString: FMTYields::serialize
		/**
		@brief Serialize the FMTYields through its base FMTList for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		friend class FMTYieldRequest;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {
				ar& boost::serialization::make_nvp("handlers", boost::serialization::base_object<FMTList<std::unique_ptr<FMTYieldHandler>>>(*this));
				_updateYieldLocations();
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTYields::serialize", __LINE__, __FILE__);
			}
		}
		// DocString: FMTYields::m_yieldsLocations
		///If size_t < size then presence else not
		std::unordered_map<std::string,size_t>m_yieldsLocations;
		// DocString: FMTYields::nullYield
		///the null yields.
		static const std::string m_nullYield;
		// DocString: FMTYields::_getHandlerOfType
		/**
		@brief Return pointers to all yield handlers of a given type.
		@param[in] type the yield handler type.
		@return the yield handlers of the type.
		*/
		std::vector<const FMTYieldHandler*> _getHandlerOfType(FMTyldtype type) const;
		// DocString: FMTYields::_getHandlerOfType
		/**
		@brief Return pointers to all yield handlers of a given type.
		@param[in] type the yield handler type.
		@return the yield handlers of the type.
		*/
		std::vector<FMTYieldHandler*> _getHandlers(FMTyldtype type);
		// DocString: FMTYields::_updateYieldLocations
		/**
		@brief Update the presence of the yields.
		*/
		void _updateYieldLocations();
		// DocString: FMTYields::_setModel
		/**
		@brief Set the model linked to the yield model handlers.
		@param[in] p_modelPtr the model to link to the yield handlers.
		*/
		void _setModel(Models::FMTModel* p_modelPtr);
		// DocString: FMTYields::_getFirstSeen
		/**
		@brief Return the location of the first yield with a given name, or size() if not found.
		@param[in] p_yield the yield name.
		@return the location of the first yield with this name.
		*/
		const_iterator _getFirstSeen(const std::string& p_yield) const;
    };
}
BOOST_CLASS_EXPORT_KEY(Core::FMTYields)
#endif // FMTYLD_Hm_included
