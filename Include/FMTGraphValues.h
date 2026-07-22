/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTGRAPHVALUES_H
#define FMTGRAPHVALUES_H

#include <vector>
#include <boost/functional/hash.hpp>


namespace Spatial
	{
	// DocString: FMTGraphValues
	/**
	@brief Wrapper around a vector of constraint values, hashable and comparable so that identical value vectors can be interned and shared.
	*/
	class FMTGraphValues
		{
		std::vector<double> m_Values;
		public:
			// DocString: FMTGraphValues()
			/**
			@brief Default constructor for FMTGraphValues.
			*/
			FMTGraphValues() = default;
			// DocString: FMTGraphValues(const std::vector<double>&)
			/**
			@brief Construct a FMTGraphValues from a vector of values.
			@param[in] p_values the values.
			*/
			FMTGraphValues(const std::vector<double>& p_values);
			// DocString: FMTGraphValues::operator==
			/**
			@brief Comparison operator for FMTGraphValues.
			@param[in] p_Other the FMTGraphValues to compare to.
			@return true if both value vectors are equal else false.
			*/
			bool operator == (const FMTGraphValues& p_Other) const;
			// DocString: FMTGraphValues::getValues
			/**
			@brief Return the values.
			@return the values.
			*/
			const std::vector<double>& getValues() const;
			// DocString: FMTGraphValues::hash
			/**
			@brief Hash the values.
			@return the hash of the values.
			*/
			size_t hash() const;
		};
	}

#endif 
