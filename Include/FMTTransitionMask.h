/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMASKTRAN_Hm_included
#define FMTMASKTRAN_Hm_included

#include "FMTBounds.hpp"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include <boost/serialization/serialization.hpp>
#include <string>
#include <vector>
#include "FMTutility.h"




namespace Core
{
class FMTDevelopment;
class FMTDevelopmentPath;
class FMTYields;
class FMTYieldRequest;
class FMTTheme;

// DocString: FMTTransitionMask
/**
@brief Transition mask holding a target mask and a proportion, used to disturb a development in a transition.
*/
class FMTEXPORT FMTTransitionMask : public FMTMaskFilter,public FMTSpec
    {
	friend class boost::serialization::access;
	// DocString: FMTTransitionMask::serialize
	/**
	@brief Serialize the FMTTransitionMask through its bases FMTMaskFilter and FMTSpec for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("maskfilter", boost::serialization::base_object<FMTMaskFilter>(*this));
		ar & boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTSpec>(*this));
		ar & BOOST_SERIALIZATION_NVP(mask);
		ar & BOOST_SERIALIZATION_NVP(proportion);
	}
    FMTMask mask;
    double proportion;
        // DocString: FMTTransitionMask::build
        /**
        @brief Build the transition mask from a mask string and the themes.
        @param[in] lmask the mask string.
        @param[in] themes the themes.
        */
        void build(const std::string& lmask,const std::vector<FMTTheme>& themes);
    public:
        // DocString: FMTTransitionMask()
        /**
        @brief Default constructor for FMTTransitionMask.
        */
        FMTTransitionMask();
		// DocString: ~FMTTransitionMask()
		/**
		@brief Default destructor for FMTTransitionMask.
		*/
		~FMTTransitionMask()=default;
        // DocString: FMTTransitionMask(const std::string&,const std::vector<FMTTheme>&,const double&)
        /**
        @brief Construct a transition mask from a mask string, the themes and a proportion.
        @param[in] lmask the mask string.
        @param[in] themes the themes.
        @param[in] lproportion the proportion.
        */
        FMTTransitionMask(const std::string& lmask,const std::vector<FMTTheme>& themes,
						const double& lproportion);
        // DocString: FMTTransitionMask::trans
        /**
        @brief Apply the transition mask to a base mask.
        @param[in] p_baseMask the base mask.
        @param[in] p_themes the themes.
        @return the transitioned mask.
        */
        FMTMask trans(const FMTMask& p_baseMask,
                        const std::vector<FMTTheme>& p_themes) const;
        // DocString: FMTTransitionMask(const FMTTransitionMask&)
        /**
        @brief Copy constructor for FMTTransitionMask.
        @param[in] rhs the FMTTransitionMask to copy.
        */
        FMTTransitionMask(const FMTTransitionMask& rhs);
        // DocString: FMTTransitionMask(const FMTTransitionMask&,const FMTMask&,const std::vector<FMTTheme>&)
        /**
        @brief Construct a transition mask from another transition mask, a mask and the themes.
        @param[in] rhs the transition mask to copy.
        @param[in] lmask the mask.
        @param[in] themes the themes.
        */
        FMTTransitionMask(const FMTTransitionMask& rhs,const FMTMask& lmask,const std::vector<FMTTheme>& themes);
        // DocString: FMTTransitionMask::operator=
        /**
        @brief Copy assignment operator for FMTTransitionMask.
        @param[in] rhs the FMTTransitionMask to copy.
        @return a reference to this FMTTransitionMask.
        */
        FMTTransitionMask& operator = (const FMTTransitionMask& rhs);
        // DocString: FMTTransitionMask::disturb
        /**
        @brief Disturb a development using the transition mask.
        @param[in] dev the development to disturb.
        @param[in] yields the yields.
        @param[in] themes the themes.
        @param[in] reset_age if true resets the age.
        @return the resulting development path.
        */
        FMTDevelopmentPath disturb(const Core::FMTDevelopment& dev, const FMTYields& yields,
			const std::vector<FMTTheme>& themes,const bool& reset_age) const;
		// DocString: FMTTransitionMask::get
		/**
		@brief Return the transition mask as a map of theme name to attribute.
		@param[in] themes the themes.
		@return a map of theme name to attribute.
		*/
		std::map<std::string, std::string>get(const std::vector<FMTTheme>& themes) const;
		// DocString: FMTTransitionMask::operator==
		/**
		@brief Equality comparison operator of FMTTransitionMask.
		@param[in] rhs the transition mask to compare with.
		@return true if both transition masks are equal else false.
		*/
		bool operator == (const FMTTransitionMask& rhs) const;
        // DocString: FMTTransitionMask::getProportion
        /**
        @brief Return the proportion of the transition mask.
        @return the proportion.
        */
        double getProportion() const;
        // DocString: FMTTransitionMask::getMask
        /**
        @brief Return the mask of the transition mask.
        @return the mask.
        */
        FMTMask getMask() const;
        // DocString: FMTTransitionMask::setMask
        /**
        @brief Set the mask of the transition mask.
        @param[in] mtmask the mask to set.
        */
        void setMask(const Core::FMTMask& mtmask);
        // DocString: FMTTransitionMask::setProportion
        /**
        @brief Set the proportion of the transition mask.
        @param[in] newproportion the proportion to set.
        */
        void setProportion(double newproportion);
		// DocString: FMTTransitionMask::preSolve
		/**
		@brief Return a presolved copy of the transition mask.
		@param[in] filter the mask filter.
		@param[in] presolvedthemes the presolved themes.
		@return the presolved transition mask.
		*/
		FMTTransitionMask preSolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&presolvedthemes) const;
        // DocString: FMTTransitionMask::preSolveRef
        /**
        @brief Presolve the transition mask in place.
        @param[in] filter the mask filter.
        @param[in] presolvedthemes the presolved themes.
        */
        void preSolveRef(const FMTMaskFilter& filter, const std::vector<FMTTheme>& presolvedthemes);
        // DocString: FMTTransitionMask::operator std::string
        /**
        @brief Return the string representation of the transition mask.
        @return the string representation of the transition mask.
        */
        operator std::string() const override;
    };

}
#endif // FMTMASKTRAN_Hm_included
