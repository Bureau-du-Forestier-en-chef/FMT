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
Graph stats gives you information about the FMTGraph and the FMTLpSolver. About there size, elements and type of elements.
*/
class FMTEXPORT FMTGraphStats
	{
	friend class boost::serialization::access;
	// DocString: FMTGraphStats::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		Default constructor for FMTGraphStats.
		*/
		FMTGraphStats();
		// DocString: FMTGraphStats(const FMTGraphStats&)
		/**
		Default copy constructor for FMTGraphStats.
		*/
		FMTGraphStats(const FMTGraphStats& rhs)=default;
		// DocString: FMTGraphStats::operator=
		/**
		Default copy assignment for FMTGraphStats.
		*/
		FMTGraphStats& operator = (const FMTGraphStats& rhs)=default;
		// DocString: FMTGraphStats::operator+=
		/**
		Default += compount assignment for FMTGraphStats.
		*/
		FMTGraphStats& operator += (const FMTGraphStats& rhs);
		// DocString: FMTGraphStats::operator-=
		/**
		Default -= compount assignment for FMTGraphStats.
		*/
		FMTGraphStats& operator -= (const FMTGraphStats& rhs);
		// DocString: FMTGraphStats::operator+
		/**
		Default + operator for FMTGraphStats.
		*/
		FMTGraphStats operator + (const FMTGraphStats& rhs);
		// DocString: FMTGraphStats::operator-
		/**
		Default - operator for FMTGraphStats.
		*/
		FMTGraphStats operator - (const FMTGraphStats& rhs);
		// DocString: FMTGraphStats::operator==
		/**
		Comparison operator of FMTGraphStats.
		*/
		bool operator == (const FMTGraphStats& rhs) const;
		// DocString: FMTGraphStats::operator!=
		/**
		FMTGraphStats nonequality operator.
		*/
		bool operator != (const FMTGraphStats& rhs) const;
		// DocString: FMTGraphStats:operator std::string
		/**
		Returns all the informations contained in the FMTGraphStats in a std::string.
		*/
		operator std::string() const;
		// DocString: ~FMTGraphStats()
		/**
		Default destructor for FMTGraphStats.
		*/
		~FMTGraphStats()=default;
	};
}
BOOST_CLASS_EXPORT_KEY(Graph::FMTGraphStats)
#endif
