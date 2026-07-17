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
FMTYields is one FMTList containing multiple yieldhandlers has seen in the yield section.
FMTYields hold all the information related to the forest productivity this class is sometime super large.
FMTYields is a class used to check if a given FMTDevelopment can be operable to an action, calculate outputs,
constraints and disturb a forest stand in a FMTTransition.
*/
class FMTEXPORT FMTYields : public FMTList<std::unique_ptr<FMTYieldHandler>>
    {
	friend class Models::FMTModel;
	friend class Models::FMTNssModel;
    public:
		// DocString: FMTYields::generateDefaultYields
		/**
		Generate the default yields like _TSLA in the yield class.
		*/
		void generateDefaultYields(const std::vector<Core::FMTTheme>& themes);
		// DocString: FMTYields::swap
		/**
		Swap FMTyield.
		*/
		void swap(FMTYields& rhs);
		// DocString: FMTYields::gotYieldType
		/**
		Returns true if the yield section has a given yield type.
		*/
		bool gotYieldType(FMTyldtype type) const;
		// DocString: FMTYields::setActionsMappingToModelHandlers
		/**
		Clear the actual actionsmapping of every yield model handlers and replace it with the new.
		*/
		void setActionsMappingToModelHandlers(const std::vector<int>& actionids);
		// DocString: FMTYields::getAllYieldNames
		/**
		return all yield names from the FMTList.
		*/
		std::vector<std::string> getAllYieldNames() const;
		// DocString: FMTYields()
		/**
		Default constructor for FMTYields.
		*/
        FMTYields();
		// DocString: ~FMTYields()
		/**
		Default destructor for FMTYields.
		*/
		~FMTYields()=default;
		// DocString: FMTYields(const FMTYields&)
		/**
		Default copy constructor for FMTYields.
		*/
        FMTYields(const FMTYields& rhs);
		// DocString: FMTYields::operator=
		/**
		Default copy assignment for FMTYields.
		*/
        FMTYields& operator = (const FMTYields& rhs);
		// DocString: FMTYields::clearCache
		/**
		Clear caching from all FMTData.
		*/
		void clearCache() final;
		// DocString: FMTYields::clearRandomYieldsCache
		/**
		@brief clear the cache of all random yield model.
		*/
		void clearRandomYieldsCache();
		// DocString: FMTYields::isYld
		/**
		This function returns true if the FMTYields section contains a given (value) non null yield.
		*/
        bool isYld(const std::string& value,bool fromsource = false) const;
		// DocString: FMTYields::isNullYld
		/**
		This function returns true if the FMTYields section contains a given (value) null yield.
		*/
		bool isNullYld(const std::string& value) const;
		// DocString: FMTYields::get
		/**
		This function is the main function used to get the yields value (targets) for a given FMTDevelopment (dev),
		looking at age,period,lock,mask etc... it returns a map of yield name (keys) and there vlues(items).
		*/
		//std::vector<double>get(std::vector<FMTYieldRequest>& requests) const;
		// DocString: FMTYields::getsingle
		/**
		This function is the main function used to get the yield value (target) for a given FMTDevelopment (dev),
		looking at age,period,lock,mask etc... it returns  the yield value.
		*/
		//double getsingle(const FMTDevelopment& dev,const std::string& target) const;
		//std::map<std::string,double>getYlds(const FMTDevelopment& dev,const FMTSpec& spec) const;
		// DocString: FMTYields::getYlds
		/**
		This function gets the yields used and its values (map) by a given specification (spec) for a given development (dev).
		*/
		//std::vector<double>getYlds(const FMTDevelopment& dev, const FMTSpec& spec) const;
		// DocString: FMTYields::getAge
		/**
		This function is used to get the new age of a FMTDevelopment (dev) 
		when disturbed by a given FMTTransition specification (sepc).
		*/
        int getAge(const FMTYieldRequest& request,const FMTSpec& spec) const;
		// DocString: FMTYields::getAllYields
		/**
		This function returns a map with mask has key (with only one FMTTheme) for only given FMTYieldHandler type (type).
		The map contains all the yield values for each yield name (map key). This function is used for generating a text file 
		containing all the yields values for GCBM (might be only usefull for Forestier en chef) .
		*/
		std::map<std::string, std::map<std::string, std::vector<double>>>getAllYields(const FMTTheme& target,FMTyldtype type) const;
		// DocString: FMTYields::operator==
		/**
		FMTYields equality operator check if FMTYields are the same.
		*/
		bool operator == (const FMTYields& rhs) const;
		// DocString: FMTYields::operator!=
		/**
		FMTYields non equality operator check if FMTYields are the same.
		*/
		bool operator != (const FMTYields& rhs) const;
		// DocString: FMTYields::update
		/**
		FMTYields being an FMTList an update function needs to be implemented to update 
		the yieldnames and nullyieldsname caching.
		*/
        void update() override;
		// DocString: FMTYields::reserve
		/**
		@brief reserve memory based on a other FMTYields
		@param[in] p_other the other yields.
		*/
		void reserve(const FMTYields& p_other);
		// DocString: FMTYields::presolve
		/**
		Presolving might be realy usefull for FMTYields because this class tend to get realy large and contains
		sometime useless stuff. So using the same presolved information it returns a presolved FMTYields section.
		*/
		FMTYields presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			const std::vector<FMTTheme>& newthemes) const;
		// DocString: FMTYields::presolveRef
		/**
		@brief Presolving might be realy usefull for FMTYields because this class tend to get realy large and contains
		sometime useless stuff. So using the same presolved information it returns a presolved FMTYields section.
		@param[in] p_filter
		@param[in] p_originalThemes
		@param[in] p_newThemes
		*/
		void presolveRef(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_originalThemes,
			const std::vector<FMTTheme>& p_newThemes);
		// DocString: FMTYields::getStacked
		/**
		This function returns the FMTYields has a string in a vector.
		*/
		std::vector<std::string>getStacked() const;
		// DocString: FMTYields::getFromFactor
		/**
		This function multiply yields section with a factor and returns a new yields function.
		If vector of yieldnames is given by the user then only the data within this names list is going
		to be multiplied by the factor.
		*/
		FMTYields getFromFactor(const double& factor,
			std::vector<std::string>yieldnames = std::vector<std::string>()) const;
		// DocString: FMTYields::get
		/**
		This function is the main function used to get the yields value (targets) for a given FMTDevelopment (dev),
		looking at age,period,lock,mask etc... it returns a map of yield name (keys) and there vlues(items).
		*/
		double get(const FMTYieldRequest& request,const std::string& yld) const;
		// DocString: FMTYields::pushBackAgeHandler
		/**
		Push back ageyield for R
		*/
		void pushBackAgeHandler(const FMTMask& mask, const FMTAgeYieldHandler& value);
		// DocString: FMTYields::pushBackTimeHandler
		/**
		Push back timeyield for R
		*/
		void pushBackTimeHandler(const FMTMask& mask, const FMTTimeYieldHandler& value);
		// DocString: FMTYields::getNullYield
		/*
		* @brief get a nullyield
		* @return null yield value.
		*/
		static const std::string& getNullYield();
		// DocString: FMTYields::getMaxBase
		/**
		 * @brief Returns the maximum age seen in the yield handlers.
		 *
		 * Each yield can have different sizes in a yield section. This function looks at all yield handlers
		 * and returns the maximum age observed among them.
		 *
		 * @param handlers A vector of pointers to FMTYieldHandler objects.
		 * @return The maximum age observed among the yield handlers.
		 */
		static int getMaxBase(const std::vector<const FMTYieldHandler*>& handlers);
		// DocString: FMTYields::clear
		/**
		 * @brief clear all yield data.
		 */
		void clear();
	private:
		// DocString: FMTYields::serialize
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		// DocString: FMTYields::getHandlerOfType
		/**
		The function returns a vector of pointer to all yieldhandler of a given (type).
		(Can returns all handler related to time/age/complex ...)
		*/
		std::vector<const FMTYieldHandler*> getHandlerOfType(FMTyldtype type) const;
		// DocString: FMTYields::getHandlerOfType
		/**
		The function returns a vector of pointer to all yieldhandler of a given (type).
		(Can returns all handler related to time/age/complex ...)
		*/
		std::vector<FMTYieldHandler*> getHandlers(FMTyldtype type);
		// DocString: FMTYields::_updateYieldLocations
		/**
		@brief Update the yields presence true or false.
		*/
		void _updateYieldLocations();
		// DocString: FMTYields::setModel
		/**
		@brief set the model to the FMTyieldmodelhandler.
		@param[in] the model that we want to link to the yieldhandler.
		*/
		void setModel(Models::FMTModel* p_modelPtr);
		// DocString: FMTYields::_getFirstSeen
		/**
		@brief get the first location of yield with this name,if out gonna be size()
		@param[in] p_yield name.
		@return the location of the first yield with this name
		*/
		const_iterator _getFirstSeen(const std::string& p_yield) const;
    };
}
BOOST_CLASS_EXPORT_KEY(Core::FMTYields)
#endif // FMTYLD_Hm_included
