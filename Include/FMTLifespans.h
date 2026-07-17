/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTLIF_Hm_included
#define FMTLIF_Hm_included

#include "FMTList.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
namespace Core
{
// DocString: FMTLifespans
/**
FMTLifespans class keeps the information about the maximum age of an aggregate of FMTdevelopments.
It's derived from FMTList<int> each element of the FMTList is a maximum age given for a FMTMask.
The class is used by the FMTParser but also into the when generating the default _DEATH action and transition.
*/
class FMTEXPORT FMTLifespans : public FMTList<int>
    {
	// DocString: FMTLifespans::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("data", boost::serialization::base_object<FMTList<int>>(*this));
	}
    public:
		// DocString: swap
		/**
		swap FMTLifespans
		*/
		void swap(FMTLifespans& rhs);
		// DocString: FMTLifespans()
		/**
		Default constructor for FMTLifespans.
		*/
        FMTLifespans();
		// DocString: ~FMTLifespans()
		/**
		Default destructor for FMTLifespans.
		*/
		~FMTLifespans()=default;
		// DocString: FMTLifespans(const FMTLifespans&)
		/**
		Default copy constructor for FMTLifespans.
		*/
        FMTLifespans(const FMTLifespans& rhs);
		// DocString: FMTLifespans::operator=
		/**
		Default copy assignment for FMTLifespans.
		*/
        FMTLifespans& operator = (const FMTLifespans& rhs);
		// DocString: FMTLifespans::operator==
		/**
		Comparison operator of FMTLifespans.
		*/
		bool operator == (const FMTLifespans& rhs) const;
		// DocString: FMTLifespans::operator!=
		/**
		Comparison !operator of FMTLifespans.
		*/
		bool operator != (const FMTLifespans& rhs) const;
		// DocString: FMTLifespans::operator std::string
		/**
		Returns a string representation of FMTLifespans section (.lif).
		*/
        operator std::string() const;
		// DocString: FMTLifespans::update
		/**
		Override the FMTList update function.
		It needs to be called if any changes is done to the FMTList.
		*/
		void update() override;
		// DocString: FMTLifespans::presolve
		/**
		Eliminate FMTList elements and presolve all masks based on a baseMask a preolved mask
		and presolved themes.
		*/
		FMTLifespans presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			std::vector<FMTTheme>& newthemes,bool compressdata=false) const;
    };

}
BOOST_CLASS_EXPORT_KEY(Core::FMTLifespans)

#endif // FMTLIF_Hm_included
