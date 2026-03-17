/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTFORK_Hm_included
#define FMTFORK_Hm_included

#include "FMTtransitionmask.h"
#include "FMTbounds.hpp"
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include "FMTobject.h"

namespace Core
{

class FMTdevelopment;
class FMTdevelopmentpath;
class FMTyieldrequest;
class FMTyields;
class FMTtheme;
class FMTmaskfilter;


class FMTEXPORT FMTfork : public FMTspec, public FMTobject
    {
    public:
        FMTfork();
        FMTfork(const FMTfork& rhs)=default;
		~FMTfork();
        FMTfork& operator = (const FMTfork& rhs)=default;
        void add(const FMTtransitionmask& transition);
        void clear();
		std::vector<FMTdevelopmentpath> getpaths(const Core::FMTdevelopment& base, const Core::FMTyields& ylds,
				const std::vector<FMTtheme>& themes,const bool& reset_age) const;
        FMTdevelopment getmax(const FMTdevelopment& base,const FMTyields& ylds,const std::vector<FMTtheme>& themes, const bool& reset_age) const;
		std::vector<FMTtransitionmask> getmasktrans() const;
		bool operator == (const FMTfork& rhs) const;
        FMTfork single() const;
        double sumprop() const;
        size_t size() const;
		FMTfork presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>&presolvedthemes) const;
        void presolveRef(const FMTmaskfilter& filter, const std::vector<FMTtheme>& presolvedthemes);
        operator std::string() const override;
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar& boost::serialization::make_nvp("specifications", boost::serialization::base_object<FMTspec>(*this));
            ar& m_transitions;
        }
        std::vector<FMTtransitionmask>m_transitions;
        FMTdevelopmentpath _GetPath(const FMTtransitionmask& p_target,
            const Core::FMTdevelopment& p_base, const Core::FMTyields& p_yields,
            const std::vector<FMTtheme>& p_themes, bool p_AgeReset) const;
    };

}

BOOST_CLASS_EXPORT_KEY(Core::FMTfork)

#endif // FMTFORK_Hm_included
