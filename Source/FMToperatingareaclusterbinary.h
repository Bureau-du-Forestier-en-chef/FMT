/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingareaclusterbinary_H_INCLUDED
#define FMToperatingareaclusterbinary_H_INCLUDED

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include "FMToperatingarea.h"
#include "FMToutput.h"

namespace Heuristics
{
	class FMToperatingareaclusterbinary : public FMToperatingarea
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("operatingarea", boost::serialization::base_object<FMToperatingarea>(*this));
			ar & BOOST_SERIALIZATION_NVP(variable);
			ar & BOOST_SERIALIZATION_NVP(maxrow);
			ar & BOOST_SERIALIZATION_NVP(minrow);
			ar & BOOST_SERIALIZATION_NVP(statistic);
			}
		int variable;
		int maxrow;
		int minrow;
		double statistic;
	public:
		FMToperatingareaclusterbinary() = default;
		FMToperatingareaclusterbinary(const FMToperatingareaclusterbinary&) = default;
		FMToperatingareaclusterbinary& operator = (const FMToperatingareaclusterbinary&) = default;
		~FMToperatingareaclusterbinary() = default;
		FMToperatingareaclusterbinary(const FMToperatingarea& oparea);
		std::vector<FMToperatingareaclusterbinary> filterneighbors(std::vector<FMToperatingareaclusterbinary> potentiallink) const;
		Core::FMToutput getoutputintersect(const Core::FMToutput& output) const;
		void setstatistic(const double& statvalue);
		void setvariable(const int& lvariable);
		void setmaximalrow(const int& lmaxrow);
		void setminimalrow(const int& lminrow);
		inline const int& getvariable() const
			{
			return variable;
			}
		inline const int& getmaxrow() const
			{
			return maxrow;
			}
		inline const int& getminrow() const
			{
			return minrow;
			}
		inline const double& getstatistic() const
			{
			return statistic;
			}

	};

}
BOOST_CLASS_EXPORT_KEY(Heuristics::FMToperatingareaclusterbinary)
#endif
#endif
