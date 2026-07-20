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
@brief List holding the maximum age of aggregates of developments.
@details Derived from FMTList<int>, each element is a maximum age for a FMTMask. Used by the parser and to generate the default _DEATH action and transition.
*/
class FMTEXPORT FMTLifespans : public FMTList<int>
    {
	// DocString: FMTLifespans::serialize
	/**
	@brief Serialize the FMTLifespans through its base FMTList<int> for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
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
		@brief Swap this FMTLifespans with another.
		@param[in,out] rhs the FMTLifespans to swap with.
		*/
		void swap(FMTLifespans& rhs);
		// DocString: FMTLifespans()
		/**
		@brief Default constructor for FMTLifespans.
		*/
        FMTLifespans();
		// DocString: ~FMTLifespans()
		/**
		@brief Default destructor for FMTLifespans.
		*/
		~FMTLifespans()=default;
		// DocString: FMTLifespans(const FMTLifespans&)
		/**
		@brief Copy constructor for FMTLifespans.
		@param[in] rhs the FMTLifespans to copy.
		*/
        FMTLifespans(const FMTLifespans& rhs);
		// DocString: FMTLifespans::operator=
		/**
		@brief Copy assignment operator for FMTLifespans.
		@param[in] rhs the FMTLifespans to copy.
		@return a reference to this FMTLifespans.
		*/
        FMTLifespans& operator = (const FMTLifespans& rhs);
		// DocString: FMTLifespans::operator==
		/**
		@brief Equality comparison operator of FMTLifespans.
		@param[in] rhs the FMTLifespans to compare with.
		@return true if both are equal else false.
		*/
		bool operator == (const FMTLifespans& rhs) const;
		// DocString: FMTLifespans::operator!=
		/**
		@brief Inequality comparison operator of FMTLifespans.
		@param[in] rhs the FMTLifespans to compare with.
		@return true if both are different else false.
		*/
		bool operator != (const FMTLifespans& rhs) const;
		// DocString: FMTLifespans::operator std::string
		/**
		@brief Return a string representation of the FMTLifespans section (.lif).
		@return the string representation of the FMTLifespans.
		*/
        operator std::string() const;
		// DocString: FMTLifespans::update
		/**
		@brief Override the FMTList update function, to be called after any change to the list.
		*/
		void update() override;
		// DocString: FMTLifespans::presolve
		/**
		@brief Presolve the lifespans by eliminating list elements and presolving the masks.
		@param[in] filter the mask filter.
		@param[in] originalthemes the original themes.
		@param[in,out] newthemes the presolved themes.
		@param[in] compressdata if true compresses the data.
		@return the presolved FMTLifespans.
		*/
		FMTLifespans presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			std::vector<FMTTheme>& newthemes,bool compressdata=false) const;
    };

}
BOOST_CLASS_EXPORT_KEY(Core::FMTLifespans)

#endif // FMTLIF_Hm_included
