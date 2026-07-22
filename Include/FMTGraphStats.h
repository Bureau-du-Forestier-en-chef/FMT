/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTgraphstats_Hm_included
#define FMTgraphstats_Hm_included

/*#ifdef FMTWITHOSI
	#include "OsiSolverInterface.h"
#endif*/
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <memory>
#include <string>
#include "FMTObject.h"

namespace Graph
{
// DocString: FMTGraphStats
/**
@brief Statistics about a FMTGraph and its FMTLpSolver: their size, elements and type of elements.
*/
class FMTEXPORT FMTGraphStats
	{
	friend class boost::serialization::access;
	// DocString: FMTGraphStats::serialize
	/**
	@brief Serialize the FMTGraphStats for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(cols);
		ar & BOOST_SERIALIZATION_NVP(rows);
		ar & BOOST_SERIALIZATION_NVP(vertices);
		ar & BOOST_SERIALIZATION_NVP(edges);
		ar & BOOST_SERIALIZATION_NVP(transfer_rows);
		ar & BOOST_SERIALIZATION_NVP(output_rows);
		ar & BOOST_SERIALIZATION_NVP(output_cols);
	}
	public:
		// DocString: FMTGraphStats::cols
		///Number of columns in the matrix of FMTLpSolver.
		int cols;
		// DocString: FMTGraphStats::rows
		///Number of rows in the matrix of FMTLpSolver.
		int rows;
		// DocString: FMTGraphStats::vertices
		///Number of vertices in the FMTGraph.
		int vertices;
		// DocString: FMTGraphStats::edges
		///Number of edges in the FMTGraph.
		int edges;
		// DocString: FMTGraphStats::transfer_rows
		///Number of area transfer rows in the FMTLpSolver.
		int transfer_rows;
		// DocString: FMTGraphStats::output_rows
		///Number of output rows in the FMTLpSolver.
		int output_rows;
		// DocString: FMTGraphStats::output_cols
		///Number of output cols in the FMTLpSolver.
		int output_cols;
		// DocString: FMTGraphStats()
		/**
		@brief Default constructor for FMTGraphStats.
		*/
		FMTGraphStats();
		// DocString: FMTGraphStats(const FMTGraphStats&)
		/**
		@brief Default copy constructor for FMTGraphStats.
		@param[in] rhs the FMTGraphStats to copy.
		*/
		FMTGraphStats(const FMTGraphStats& rhs)=default;
		// DocString: FMTGraphStats::operator=
		/**
		@brief Default copy assignment operator for FMTGraphStats.
		@param[in] rhs the FMTGraphStats to copy.
		@return a reference to this FMTGraphStats.
		*/
		FMTGraphStats& operator = (const FMTGraphStats& rhs)=default;
		// DocString: FMTGraphStats::operator+=
		/**
		@brief Compound addition assignment operator for FMTGraphStats.
		@param[in] rhs the FMTGraphStats to add.
		@return a reference to this FMTGraphStats.
		*/
		FMTGraphStats& operator += (const FMTGraphStats& rhs);
		// DocString: FMTGraphStats::operator-=
		/**
		@brief Compound subtraction assignment operator for FMTGraphStats.
		@param[in] rhs the FMTGraphStats to subtract.
		@return a reference to this FMTGraphStats.
		*/
		FMTGraphStats& operator -= (const FMTGraphStats& rhs);
		// DocString: FMTGraphStats::operator+
		/**
		@brief Addition operator for FMTGraphStats.
		@param[in] rhs the FMTGraphStats to add.
		@return the sum of the FMTGraphStats.
		*/
		FMTGraphStats operator + (const FMTGraphStats& rhs);
		// DocString: FMTGraphStats::operator-
		/**
		@brief Subtraction operator for FMTGraphStats.
		@param[in] rhs the FMTGraphStats to subtract.
		@return the difference of the FMTGraphStats.
		*/
		FMTGraphStats operator - (const FMTGraphStats& rhs);
		// DocString: FMTGraphStats::operator==
		/**
		@brief Comparison operator for FMTGraphStats.
		@param[in] rhs the FMTGraphStats to compare to.
		@return true if both stats are equal else false.
		*/
		bool operator == (const FMTGraphStats& rhs) const;
		// DocString: FMTGraphStats::operator!=
		/**
		@brief Comparison operator for FMTGraphStats.
		@param[in] rhs the FMTGraphStats to compare to.
		@return true if both stats are different else false.
		*/
		bool operator != (const FMTGraphStats& rhs) const;
		// DocString: FMTGraphStats::operator std::string
		/**
		@brief Return all the information contained in the FMTGraphStats as a string.
		@return the string representation of the stats.
		*/
		operator std::string() const;
		// DocString: ~FMTGraphStats()
		/**
		@brief Default destructor for FMTGraphStats.
		*/
		~FMTGraphStats()=default;
	};
}
BOOST_CLASS_EXPORT_KEY(Graph::FMTGraphStats)
#endif
