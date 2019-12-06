
//The serialization part should be placed in this order !!!
////////////////////////from ////////////////////////////
//If you mess with that order linking in g++ is not going to work
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include "FMTwsParser.h"
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
BOOST_CLASS_EXPORT(Models::FMTlpmodel)
BOOST_CLASS_EXPORT(Models::FMTsesmodel)
BOOST_CLASS_EXPORT(Heuristics::FMToperatingareaheuristic)
////////////////////////to////////////////////////////

#if defined FMTPY
#include "FMTpythonpickle.h"
#include "FMTpythoncore.h"
#include <boost/python/docstring_options.hpp>
#include <boost/python/args.hpp>

//Function to export each namespace in python

#include "PYdefinitions.h"
#include "PYexportExceptions.h"
#include "PYexportCore.h"
#include "PYexportSpatial.h"
#include "PYexportWSParser.h"
#include "PYexportGraph.h"
#include "PYexportModel.h"
#include "PYexportVersion.h"
#include "PYexportHeuristics.h"

extern "C"
{


    BOOST_PYTHON_MODULE(FMT)
        {

		boost::python::to_python_converter<vector<string,allocator<string>>,VecToList<string>>();
        boost::python::to_python_converter<vector<double,allocator<double>>,VecToList<double>>();
		boost::python::to_python_converter<map<string,vector<string>>,MapToDict<string, vector<string>>>();

        VecFrList<string>();
        VecFrList<double>();

        //Exceptions
        exportException();
        //Core
        exportCore();
        //Spatial
		exportSpatial();
		//Graph
		exportGraph();
		//Model
		exportModel();
        //Parser
		exportWSParser();
		//Version
		exportVersion();
		//Heuristics
		exportHeuristics();
        }

}

#endif
