/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingareacluster_H_INCLUDED
#define FMToperatingareacluster_H_INCLUDED

#include "FMToperatingareaclusterbinary.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <vector>

namespace Heuristics
{
	class FMToperatingareacluster
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(maxvar);
			ar & BOOST_SERIALIZATION_NVP(minvar);
			ar & BOOST_SERIALIZATION_NVP(minvart);
			ar & BOOST_SERIALIZATION_NVP(binaries);
			ar & BOOST_SERIALIZATION_NVP(centroid);
		}
		int maxvar;
		int minvar;
		int minvart;
		double minimalarea;
        double maximalarea;
		std::vector<FMToperatingareaclusterbinary>binaries;
		FMToperatingareaclusterbinary centroid;
	public:
		FMToperatingareacluster() = default;
		FMToperatingareacluster(const FMToperatingareaclusterbinary& localcentroid,
								const std::vector<FMToperatingareaclusterbinary>& neighborsbinaries,
								const double& lminimalarea,
								const double& lmaximalarea);
		FMToperatingareacluster(const FMToperatingareacluster&) = default;
		FMToperatingareacluster& operator = (const FMToperatingareacluster&) = default;
		~FMToperatingareacluster() = default;
		const FMToperatingareaclusterbinary& getbinary(const Core::FMTmask& mask) const;
		FMToperatingareaclusterbinary getcentroid() const;
		std::vector<FMToperatingareaclusterbinary> getbinaries() const;
		int getmaximalobjectivevariable() const;
		int getminimalobjectivevariable() const;
		double getminimalarea() const;
		double getmaximalarea() const;
		double getrealminimalarea() const;
		double getrealmaximalarea() const;
		double gettotalpotentialarea() const;
		bool isvalidarea(const double& area) const;
		void setmaximalobjectivevariable(const int& lmaxvar);
		void setminimalobjectivevariable(const int& lminvar);
		void setminimalarea(const double& newminimalarea);
		void setmaximalarea(const double& newmaximalarea);
		bool isvalidareabounds() const;
	};
}

BOOST_CLASS_EXPORT_KEY(Heuristics::FMToperatingareacluster)
#endif
#endif
