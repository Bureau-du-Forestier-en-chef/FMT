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


#ifndef FMToperatingareaheuristic_H_INCLUDED
#define FMToperatingareaheuristic_H_INCLUDED

#include "FMTmask.h"
#include "FMTyieldhandler.h"
#include "FMToperatingarea.h"
#include "FMTgraph.h"
#include "FMToutput.h"
#include "FMTmodel.h"
#include "OsiSolverInterface.hpp"
#include "FMTserializablematrix.h"
#include "FMTsolverinterface.h"
#include "FMTtheme.h"
#include <random>
#include <memory>
#include <map>
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


namespace Heuristics
{
	class FMToperatingareaheuristic : public FMTobject
	{
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar & BOOST_SERIALIZATION_NVP(operatingareas);
			ar & BOOST_SERIALIZATION_NVP(adjacencyconstraints);
			//Save the matrix columnes / rows / column solution / row solution / objective
			Models::FMTserializablematrix matrix(solverinterface,solvertype);
			ar & BOOST_SERIALIZATION_NVP(matrix);
			ar & BOOST_SERIALIZATION_NVP(seed);
			ar & BOOST_SERIALIZATION_NVP(proportionofset);
			ar & BOOST_SERIALIZATION_NVP(userandomness);
			ar & BOOST_SERIALIZATION_NVP(usingsolvercopy);
			ar & BOOST_SERIALIZATION_NVP(useprimal);
		}
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(operatingareas);
			ar & BOOST_SERIALIZATION_NVP(adjacencyconstraints);
			//load the matrix
			Models::FMTserializablematrix matrix;
			ar & BOOST_SERIALIZATION_NVP(matrix);
			//Need to create solverinterface with new
			matrix.setsolvertype(solvertype);
			solverinterface = matrix.buildsolverinterface(solvertype, &*this->_logger);
			matrix.setmatrix(solverinterface);
			ar & BOOST_SERIALIZATION_NVP(seed);
			this->setgeneratorseed(seed);
			ar & BOOST_SERIALIZATION_NVP(proportionofset);
			ar & BOOST_SERIALIZATION_NVP(userandomness);
			ar & BOOST_SERIALIZATION_NVP(usingsolvercopy);
			ar & BOOST_SERIALIZATION_NVP(useprimal);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		std::vector<FMToperatingarea>operatingareas;
		std::map<std::pair<FMTmask, FMTmask>,vector<int>>adjacencyconstraints;
		std::shared_ptr<OsiSolverInterface> solverinterface; //Can be pointing to the FMTmodelpinterface or can own is pointer!
		std::default_random_engine generator;
		size_t seed;
		double proportionofset;
		bool userandomness;
		bool usingsolvercopy;
		bool useprimal;
		Models::FMTsolverinterface solvertype;
		void setoperatingareasconstraints(const Graph::FMTgraph& maingraph,
			const Models::FMTmodel& model,
			const Core::FMToutputnode& target);
		void setadjacencyconstraints();//use getcommonbinairies
		vector<std::vector<FMToperatingarea>::const_iterator> setdraw();
		size_t setbounds(const vector<std::vector<FMToperatingarea>::const_iterator>& tobound);
		void unboundall();
		void setallinteger();
		int resolvemodel();
	public:
		void initialsolve();
		void branchnboundsolve();
		void setasrandom();
		void setgeneratorseed(const size_t& lseed);
		std::vector<FMTyieldhandler> getsolution(const string& yldname) const;
		FMToperatingareaheuristic(const std::vector<FMToperatingarea>& loperatingareas,
			const Graph::FMTgraph& maingraph,
			const Models::FMTmodel& model,
			const Core::FMToutputnode& target,
			std::shared_ptr<OsiSolverInterface> initialsolver,
			const Models::FMTsolverinterface& lsolvertype, size_t lseed = 0,
			double proportionofset = 0.25, bool userandomness = false, bool copysolver = true);
		FMToperatingareaheuristic()=default;
		FMToperatingareaheuristic(const FMToperatingareaheuristic& rhs);
		FMToperatingareaheuristic& operator = (const FMToperatingareaheuristic& rhs);
		bool isfeasible() const;
		double getobjective() const;
		~FMToperatingareaheuristic();
	};
}

#endif
