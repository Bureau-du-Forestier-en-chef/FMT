/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTgraphstats_H_INCLUDED
#define FMTgraphstats_H_INCLUDED

/*#ifdef FMTWITHOSI
	#include "OsiSolverInterface.hpp"
#endif*/
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <memory>
#include <string>
#include "FMTobject.hpp"

namespace Graph
{
// DocString: FMTgraphstats
/**
Graph stats gives you information about the FMTgraph and the FMTlpsolver. About there size, elements and type of elements.
*/
class FMTEXPORT FMTgraphstats
	{
	friend class boost::serialization::access;
	// DocString: FMTgraphstats::serialize
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
		// DocString: FMTgraphstats::cols
		///Number of columns in the matrix of FMTlpsolver.
		int cols;
		// DocString: FMTgraphstats::rows
		///Number of rows in the matrix of FMTlpsolver.
		int rows;
		// DocString: FMTgraphstats::vertices
		///Number of verticies in the FMTgraph.
		int vertices;
		// DocString: FMTgraphstats::edges
		///Number of edges in the FMTgraph.
		int edges;
		// DocString: FMTgraphstats::transfer_rows
		///Number of area transfer rows in the FMTlpsolver.
		int transfer_rows;
		// DocString: FMTgraphstats::output_rows
		///Number of output rows in the FMTlpsolver.
		int output_rows;
		// DocString: FMTgraphstats::output_cols
		///Number of output cols in the FMTlpsolver.
		int output_cols;
		// DocString: FMTgraphstats()
		/**
		Default constructor for FMTgraphstats.
		*/
		FMTgraphstats();
		// DocString: FMTgraphstats(const FMTgraphstats&)
		/**
		Default copy constructor for FMTgraphstats.
		*/
		FMTgraphstats(const FMTgraphstats& rhs);
		// DocString: FMTgraphstats::operator=
		/**
		Default copy assignment for FMTgraphstats.
		*/
		FMTgraphstats& operator = (const FMTgraphstats& rhs);
		// DocString: FMTgraphstats::operator+=
		/**
		Default += compount assignment for FMTgraphstats.
		*/
		FMTgraphstats& operator += (const FMTgraphstats& rhs);
		// DocString: FMTgraphstats::operator-=
		/**
		Default -= compount assignment for FMTgraphstats.
		*/
		FMTgraphstats& operator -= (const FMTgraphstats& rhs);
		// DocString: FMTgraphstats::operator+
		/**
		Default + operator for FMTgraphstats.
		*/
		FMTgraphstats operator + (const FMTgraphstats& rhs);
		// DocString: FMTgraphstats::operator-
		/**
		Default - operator for FMTgraphstats.
		*/
		FMTgraphstats operator - (const FMTgraphstats& rhs);
		// DocString: FMTgraphstats::operator==
		/**
		Comparison operator of FMTgraphstats.
		*/
		bool operator == (const FMTgraphstats& rhs) const;
		// DocString: FMTgraphstats::operator!=
		/**
		FMTgraphstats nonequality operator.
		*/
		bool operator != (const FMTgraphstats& rhs) const;
		// DocString: FMTgraphstats:operator std::string
		/**
		Returns all the informations contained in the FMTgraphstats in a std::string.
		*/
		operator std::string() const;
		// DocString: ~FMTgraphstats()
		/**
		Default destructor for FMTgraphstats.
		*/
		~FMTgraphstats()=default;
	};
}
BOOST_CLASS_EXPORT_KEY(Graph::FMTgraphstats)
#endif
