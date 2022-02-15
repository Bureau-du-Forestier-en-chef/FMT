/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTFORK_H_INCLUDED
#define FMTFORK_H_INCLUDED

#include "FMTtransitionmask.hpp"
#include "FMTbounds.hpp"
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Core
{

class FMTdevelopment;
class FMTdevelopmentpath;
class FMTyieldrequest;
class FMTyields;

class FMTEXPORT FMTfork : public FMTspec
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("specifications", boost::serialization::base_object<FMTspec>(*this));
		ar & BOOST_SERIALIZATION_NVP(transitions);
		}
	std::vector<FMTtransitionmask>transitions;
    public:
        FMTfork();
        FMTfork(const FMTfork& rhs);
		~FMTfork() = default;
        FMTfork& operator = (const FMTfork& rhs);
        void add(FMTtransitionmask& transition);
		std::vector<FMTdevelopmentpath> getpaths(const Core::FMTdevelopment& base, const Core::FMTyields& ylds,
				const std::vector<FMTtheme>& themes,const bool& reset_age) const;
        FMTdevelopment getmax(const FMTdevelopment& base,const FMTyields& ylds,const std::vector<FMTtheme>& themes, const bool& reset_age) const;
		std::vector<FMTtransitionmask> getmasktrans() const;
		bool operator == (const FMTfork& rhs) const;
        FMTfork single() const;
        double sumprop() const;
        size_t size() const;
		FMTfork presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>&presolvedthemes) const;
        operator std::string() const override;
    };

}

BOOST_CLASS_EXPORT_KEY(Core::FMTfork)

#endif // FMTFORK_H_INCLUDED
