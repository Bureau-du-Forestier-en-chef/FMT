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

class FMTDevelopment;
class FMTDevelopmentPath;
class FMTYieldRequest;
class FMTYields;
class FMTTheme;
class FMTMaskFilter;


class FMTEXPORT FMTFork : public FMTSpec, public FMTObject
    {
    public:
        FMTFork();
        FMTFork(const FMTFork& rhs)=default;
		~FMTFork();
        FMTFork& operator = (const FMTFork& rhs)=default;
        void add(const FMTTransitionMask& transition);
        void clear();
		std::vector<FMTDevelopmentPath> getPaths(const Core::FMTDevelopment& base, const Core::FMTYields& ylds,
				const std::vector<FMTTheme>& themes,const bool& reset_age) const;
        FMTDevelopment getMax(const FMTDevelopment& base,const FMTYields& ylds,const std::vector<FMTTheme>& themes, const bool& reset_age) const;
		std::vector<FMTTransitionMask> getMaskTrans() const;
		bool operator == (const FMTFork& rhs) const;
        FMTFork single() const;
        double sumProp() const;
        size_t size() const;
		FMTFork presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&presolvedthemes) const;
        void presolveRef(const FMTMaskFilter& filter, const std::vector<FMTTheme>& presolvedthemes);
        operator std::string() const override;
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar& boost::serialization::make_nvp("specifications", boost::serialization::base_object<FMTSpec>(*this));
            ar& m_transitions;
        }
        std::vector<FMTTransitionMask>m_transitions;
        FMTDevelopmentPath _getPath(const FMTTransitionMask& p_target,
            const Core::FMTDevelopment& p_base, const Core::FMTYields& p_yields,
            const std::vector<FMTTheme>& p_themes, bool p_AgeReset) const;
    };

}

BOOST_CLASS_EXPORT_KEY(Core::FMTFork)

#endif // FMTFORK_Hm_included
