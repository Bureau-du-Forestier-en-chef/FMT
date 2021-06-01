/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTLIF_H_INCLUDED
#define FMTLIF_H_INCLUDED

#include "FMTlist.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
namespace Core
{
// DocString: FMTlifespans
/**
FMTlifespans class keeps the information about the maximum age of an aggregate of FMTdevelopements.
It's derived from FMTlist<int> each element of the FMTlist is a maximum age given for a FMTmask.
The class is used by the FMTparser but also into the when generating the default _DEATH action and transition.
*/
class FMTEXPORT FMTlifespans : public FMTlist<int>
    {
	// DocString: FMTlifespans::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("data", boost::serialization::base_object<FMTlist<int>>(*this));
	}
    public:
		// DocString: FMTlifespans()
		/**
		Default constructor for FMTlifespans.
		*/
        FMTlifespans();
		// DocString: ~FMTlifespans()
		/**
		Default destructor for FMTlifespans.
		*/
		~FMTlifespans()=default;
		// DocString: FMTlifespans(const FMTlifespans&)
		/**
		Default copy constructor for FMTlifespans.
		*/
        FMTlifespans(const FMTlifespans& rhs);
		// DocString: FMTlifespans::operator=
		/**
		Default copy assignment for FMTlifespans.
		*/
        FMTlifespans& operator = (const FMTlifespans& rhs);
		// DocString: FMTlifespans::operator==
		/**
		Comparison operator of FMTlifespans.
		*/
		bool operator == (const FMTlifespans& rhs) const;
		// DocString: FMTlifespans::operator std::string
		/**
		Returns a string representation of FMTlifespans section (.lif).
		*/
        operator std::string() const;
		// DocString: FMTlifespans::update
		/**
		Override the FMTlist update function.
		It needs to be called if any changes is done to the FMTlist.
		*/
		void update() override;
		// DocString: FMTlifespans::presolve
		/**
		Eliminate FMTlist elements and presolve all masks based on a basemask a preolved mask
		and presolved themes.
		*/
		FMTlifespans presolve(const FMTmask& basemask,
			const std::vector<FMTtheme>& originalthemes,
			const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& newthemes) const;
    };

}
BOOST_CLASS_EXPORT_KEY(Core::FMTlifespans)

#endif // FMTLIF_H_INCLUDED
