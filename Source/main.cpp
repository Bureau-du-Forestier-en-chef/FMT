/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>



#include "FMTModels.hpp"
#include "FMTversion.hpp"

#if defined FMTWITHPYTHON
#include "FMTpythonpickle.hpp"
#include "FMTpythoncore.hpp"
#include <boost/python/docstring_options.hpp>
#include <boost/python/args.hpp>

//Function to export each namespace in python

#include "PYdefinitions.hpp"
#include "PYexportExceptions.hpp"
#include "PYexportCore.hpp"
#include "PYexportSpatial.hpp"
#include "PYexportParser.hpp"
#include "PYexportGraph.hpp"
#include "PYexportModel.hpp"
#include "PYexportVersion.hpp"
#include "PYexportHeuristics.hpp"
#include "Python.h"


extern "C"
{

	BOOST_PYTHON_MODULE(FMT)
	{
		boost::python::to_python_converter<std::vector<std::string, std::allocator<std::string>>, Python::VecToList<std::string>>();
		boost::python::to_python_converter<std::vector<double, std::allocator<double>>, Python::VecToList<double>>();
		boost::python::to_python_converter<std::map<std::string, std::vector<std::string>>, Python::MapToDict<std::string, std::vector<std::string>>>();
		Python::VecFrList<std::string>();
		Python::VecFrList<double>();

		//Exceptions
		Python::exportException();
		//Core
		Python::exportCore();
		//Spatial
		Python::exportSpatial();
		//Graph
		Python::exportGraph();
		//Model
		Python::exportModel();
		//Parser
		Python::exportParser();
		//Version
		Python::exportVersion();
		//Heuristics
		Python::exportHeuristics();
	}

}
#elif defined FMTWITHR

#include "Rdeclarations.hpp"
#include "Rdefinitions.hpp"
#include "RexportExceptions.hpp"
#include "RexportCore.hpp"
#include "RexportGraph.hpp"
#include "RexportSpatial.hpp"
#include "RexportModel.hpp"
#include "RexportParser.hpp"
#include "RexportVersion.hpp"
#include "RexportHeuristics.hpp"
#include <Rcpp.h>



	RCPP_MODULE(FMT)
		{
		//Exceptions
		R::exportException();
		//Core
		R::exportCore();
		//Spatial
		R::exportSpatial();
		//Graph
		R::exportGraph();
		//Model
		R::exportModel();
		//Parser
		R::exportParser();
		//Version
		R::exportVersion();
		//Heuristics
		R::exportHeuristics();
		}

		// declarations
	extern "C"
	{
		SEXP FMT();
	}

// definition
SEXP FMT()// use that part for exception handling to return custom exception to R
	{
		BEGIN_RCPP
		//Exceptions
		
		END_RCPP
	}





#endif
