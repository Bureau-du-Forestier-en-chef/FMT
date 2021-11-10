/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodel.hpp"
#include "FMTyieldrequest.hpp"
#include "FMTgraph.hpp"
#include "FMTlinegraph.hpp"
#include "FMTyields.hpp"
#include "FMTcarbonpredictor.hpp"
#include "FMTsrmodel.hpp"

#ifdef FMTWITHONNXR
	#include  <onnxruntime/core/session/onnxruntime_cxx_api.h>
#endif

namespace Core {

	FMTyieldmodel::FMTyieldmodel(const std::string& themodelname):modelname(themodelname)
	{

	}

	const std::string& FMTyieldmodel::GetModelName() const
	{
		return modelname;
	}

	std::unique_ptr<FMTyieldmodel>FMTyieldmodel::Clone() const
	{
		try {
			//_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			//	"FMTyieldmodel::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodel(*this));
	}

	std::string FMTyieldmodel::GetModelInfo() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldmodel::GetModelInfo", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::GetModelInfo", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::string();
	}
	bool FMTyieldmodel::Validate(const std::vector<std::string>& YieldsAvailable) const
	{
		try {
			//_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			//	"FMTyieldmodel::Validate", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Validate", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return false;
	}
	std::vector<std::string>FMTyieldmodel::GetYieldsOutputs() const
	{
		try {
			//_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			//	"FMTyieldmodel::GetYieldsSources", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::GetYieldsSources", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<std::string>();
	}


	std::vector<double>FMTyieldmodel::Predict(const Core::FMTyieldrequest& request) const
	{
		try {
			#ifdef FMTWITHONNXR
				Ort::Env env;
			#endif
			const Graph::FMTgraphvertextoyield* graphinfo = request.getvertexgraphinfo();
			const Models::FMTmodel* modelptr = graphinfo->getmodel();
			const std::vector<std::string>modelinputyields(1,"VOLUMETOTAL");//Les yields inputs du modele 4 dans le cas de FORAC doit etre membre de FMTyieldmodel tu dois ajuster ca dans ta classe ici j'ai harcode nimporte quoi.
			const Graph::FMTgraph<Graph::FMTbasevertexproperties,Graph::FMTbaseedgeproperties>* linegraph = graphinfo->getlinegraph();
			const Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>* fullgraph = graphinfo->getfullgraph();
			if (linegraph!=nullptr)//Im a linegraph
				{
				const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor* vertex = linegraph->getvertexfromvertexinfo(graphinfo);
				const std::vector<Graph::FMTcarbonpredictor>predictors = linegraph->getcarbonpredictors(*vertex, *modelptr,modelinputyields, 3);
				const Graph::FMTcarbonpredictor& predictor = predictors.at(0);//Seulement un predictor car on est un linegraph...								  //return dopredictionson(predictor.getpredictors()) Utiliser se vecteur de double (xs du modele) pour predire
				//Ta fonction doit retourner quelque chose qui ressemble a ça utilise getpredictors pour avoir tes doubles de prediction
				//return dopredictionson(predictor.getpredictors())
			}else if (fullgraph!=nullptr)//Im a full graph
				{
				const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor* vertex = fullgraph->getvertexfromvertexinfo(graphinfo);
				const std::vector<Graph::FMTcarbonpredictor>predictors = fullgraph->getcarbonpredictors(*vertex, *modelptr, modelinputyields, 3);
				//Dans un fullgraph il existe plusieurs predicteurs pour chaque noeud predictors.size() >= 1 <= a beaucoup
				//On peut faire du blackmagic pour aller chercher la solution existante de chaque predictor...
				const Models::FMTsrmodel* srmodelptr = dynamic_cast<const Models::FMTsrmodel*>(modelptr); //cast to a srmodel
				const Models::FMTlpsolver* solverptr = srmodelptr->getconstsolverptr(); //getsolver
				if (!solverptr->isProvenOptimal())
					{
					//Si le lpsolver n'est pas optimal faudrait trouver une autre solution...
					//C'est le cas qu'on veut optimiser du carbone
					//Peut-etre faire un "solution" avec des 1 partout pour representer chaque edge de façon equivalente?
					//Pour l'instant on va mettre un message d'erreur
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot use " + modelname + " without solution",
						"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
					}
				const double* solution = solverptr->getColSolution();
				const std::vector<int>invariables = fullgraph->getinvariables(*vertex);
				double totalarea = 0;
				std::vector<double>results(GetYieldsOutputs().size(), 0.0);
				for (size_t inedgeid = 0; inedgeid < invariables.size(); ++inedgeid)
				{
					totalarea += *(solution + invariables.at(inedgeid));
				}
				for (size_t inedgeid = 0; inedgeid < invariables.size();++inedgeid)
					{
					//Ta fonction doit remplire ve vecteur
					std::vector<double>edgeresults;// = dopredictionson(predictors.at(inedgeid).getpredictors());
					const double edgevalue = (*(solution + invariables.at(inedgeid)) / totalarea);
					for (size_t yldid = 0 ; yldid < results.size();++yldid)
						{
						results[yldid] += (edgeresults.at(yldid)*edgevalue);
						}
					}
				return results;
			}else {//Im nothing
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot use "+ modelname+" yield model without graph info",
					"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
			}

			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Something went wrong in "+ modelname,
				"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return std::vector<double>();
	}
	
}