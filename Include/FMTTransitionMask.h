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

class FMTEXPORT FMTTransitionMask : public FMTMaskFilter,public FMTSpec
    {
	friend class boost::serialization::access;
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
        void build(const std::string& lmask,const std::vector<FMTTheme>& themes);
    public:
        FMTTransitionMask();
		~FMTTransitionMask()=default;
        FMTTransitionMask(const std::string& lmask,const std::vector<FMTTheme>& themes,
						const double& lproportion);
        FMTMask trans(const FMTMask& p_baseMask,
                        const std::vector<FMTTheme>& p_themes) const;
        FMTTransitionMask(const FMTTransitionMask& rhs);
        FMTTransitionMask(const FMTTransitionMask& rhs,const FMTMask& lmask,const std::vector<FMTTheme>& themes);
        FMTTransitionMask& operator = (const FMTTransitionMask& rhs);
        FMTDevelopmentPath disturb(const Core::FMTDevelopment& dev, const FMTYields& yields,
			const std::vector<FMTTheme>& themes,const bool& reset_age) const;
		std::map<std::string, std::string>get(const std::vector<FMTTheme>& themes) const;
		bool operator == (const FMTTransitionMask& rhs) const;
        double getProportion() const;
        FMTMask getMask() const;
        void setMask(const Core::FMTMask& mtmask);
        void setProportion(double newproportion);
		FMTTransitionMask presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&presolvedthemes) const;
        void presolveRef(const FMTMaskFilter& filter, const std::vector<FMTTheme>& presolvedthemes);
        operator std::string() const override;
    };

}
#endif // FMTMASKTRAN_Hm_included
