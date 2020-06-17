/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMASKTRAN_H_INCLUDED
#define FMTMASKTRAN_H_INCLUDED

#include "FMTbounds.h"
#include "FMTdevelopment.h"
#include "FMTmask.h"
#include "FMTmaskfilter.h"
#include <boost/serialization/serialization.hpp>
#include "FMTyields.h"
#include <string>
#include "FMTtheme.h"


namespace Core
{

class FMTdevelopment;
class FMTyields;

class FMTtransitionmask : public FMTmaskfilter,public FMTspec
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("maskfilter", boost::serialization::base_object<FMTmaskfilter>(*this));
		ar & boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTspec>(*this));
		ar & BOOST_SERIALIZATION_NVP(mask);
		ar & BOOST_SERIALIZATION_NVP(proportion);
	}
    FMTmask mask;
    double proportion;
        void build(const std::string& lmask,const std::vector<FMTtheme>& themes);
    public:
        FMTtransitionmask();
		~FMTtransitionmask()=default;
        FMTtransitionmask(const std::string& lmask,const std::vector<FMTtheme>& themes,
						const double& lproportion);
        FMTmask trans(const FMTmask& basemask) const;
        FMTtransitionmask(const FMTtransitionmask& rhs);
        FMTtransitionmask(const FMTtransitionmask& rhs,const FMTmask& lmask,const std::vector<FMTtheme>& themes);
        FMTtransitionmask& operator = (const FMTtransitionmask& rhs);
        FMTdevelopment disturb(const FMTdevelopment& dev,const FMTyields& yields,
			const std::vector<FMTtheme>& themes,const bool& reset_age) const;
		std::map<std::string, std::string>get(const std::vector<FMTtheme>& themes) const;
		bool operator == (const FMTtransitionmask& rhs) const;
        double getproportion() const;
        FMTmask getmask() const;
        void setproportion(double newproportion);
		FMTtransitionmask presolve(const FMTmask& selectedmask, const std::vector<FMTtheme>&presolvedthemes) const;
        operator std::string() const override;
    };

}
#endif // FMTMASKTRAN_H_INCLUDED
