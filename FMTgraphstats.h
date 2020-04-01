/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef FMTgraphstats_H_INCLUDED
#define FMTgraphstats_H_INCLUDED

#include "FMTgraphdescription.h"
#include "OsiSolverInterface.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <memory>
#include <string>

namespace Graph
{
class FMTgraphstats
	{
	friend class boost::serialization::access;
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
		int cols;
		int rows;
		int vertices;
		int edges;
		int transfer_rows;
		int output_rows;
		int output_cols;
		FMTgraphstats();
		FMTgraphstats(const std::unique_ptr<OsiSolverInterface>& solverinterface,
			const FMTadjacency_list& graph,
			int ltransfer_rows, int loutput_rows, int loutput_cols);
		FMTgraphstats(const FMTgraphstats& rhs);
		FMTgraphstats& operator = (const FMTgraphstats& rhs);
		FMTgraphstats& operator += (const FMTgraphstats& rhs);
		FMTgraphstats& operator -= (const FMTgraphstats& rhs);
		FMTgraphstats operator + (const FMTgraphstats& rhs);
		FMTgraphstats operator - (const FMTgraphstats& rhs);
		bool operator == (const FMTgraphstats& rhs) const;
		bool operator != (const FMTgraphstats& rhs) const;
		operator std::string() const;
		~FMTgraphstats()=default;
	};
}

#endif
