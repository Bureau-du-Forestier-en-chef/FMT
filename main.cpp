
//The serialization part should be placed in this order !!!
////////////////////////from ////////////////////////////
//If you mess with that order linking in g++ is not going to work
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include "FMTParser.h"
#include "FMTModels.h"
#include "FMTSpatials.h"
#include "FMTversion.h"
#include "FMToperatingareaheuristic.h"

BOOST_CLASS_EXPORT(Core::FMTmask)
BOOST_CLASS_EXPORT(Core::FMTtheme)
BOOST_CLASS_EXPORT(Core::FMTdevelopment)
BOOST_CLASS_EXPORT(Core::FMTactualdevelopment)
BOOST_CLASS_EXPORT(Core::FMTfuturdevelopment)
BOOST_CLASS_EXPORT(Core::FMTaction)
BOOST_CLASS_EXPORT(Core::FMTtransition)
BOOST_CLASS_EXPORT(Core::FMTyields)
BOOST_CLASS_EXPORT(Core::FMToutput)
BOOST_CLASS_EXPORT(Core::FMTconstraint)
BOOST_CLASS_EXPORT(Core::FMTschedule)
BOOST_CLASS_EXPORT(Models::FMTmodel)
BOOST_CLASS_EXPORT(Graph::FMTgraph)
BOOST_CLASS_EXPORT(Models::FMTsesmodel)
#ifdef FMTWITHOSI
	BOOST_CLASS_EXPORT(Models::FMTlpmodel)
	BOOST_CLASS_EXPORT(Heuristics::FMToperatingareaheuristic)
#endif
////////////////////////to////////////////////////////

#if defined FMTWITHPYTHON
#include "FMTpythonpickle.h"
#include "FMTpythoncore.h"
#include <boost/python/docstring_options.hpp>
#include <boost/python/args.hpp>

//Function to export each namespace in python

#include "PYdefinitions.h"
#include "PYexportExceptions.h"
#include "PYexportCore.h"
#include "PYexportSpatial.h"
#include "PYexportParser.h"
#include "PYexportGraph.h"
#include "PYexportModel.h"
#include "PYexportVersion.h"
#include "PYexportHeuristics.h"
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

	#include "Rdeclarations.h"
	#include "Rdefinitions.h"
	#include "RexportExceptions.h"
	#include "RexportCore.h"
	#include "RexportSpatial.h"
	#include "RexportParser.h"
	#include "RexportGraph.h"
	#include "RexportModel.h"
	#include "RexportVersion.h"
	#include "RexportHeuristics.h"
	#include <Rcpp>
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
	SEXP testnorm();
	}

// definition
SEXP testnorm()// use that part for exception handling to return custom exception to R
	{
		BEGIN_RCPP

		END_RCPP
	}

#endif
