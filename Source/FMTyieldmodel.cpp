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

namespace Core {
	const std::string JSON_PROP_MODEL_NAME = "modelFileName";
	const std::string JSON_PROP_STAND_FILE_PATH = "csvStandardisationFile";

	FMTyieldmodel::FMTyieldmodel(const boost::property_tree::ptree& jsonProps) :
		FMTobject(), modelname()
	{
		boost::property_tree::ptree::const_assoc_iterator modelNameIt = jsonProps.find(JSON_PROP_MODEL_NAME);
		modelname = modelNameIt->second.data();
		boost::property_tree::ptree::const_assoc_iterator stdParamsFileNameIt = jsonProps.find(JSON_PROP_STAND_FILE_PATH);
		std::string stdParamsFileName = stdParamsFileNameIt->second.data();

		std::wstring wideModelName = std::wstring(modelname.begin(), modelname.end());
		envPtr = std::unique_ptr<Ort::Env>(new Ort::Env());
		sessionPtr = std::unique_ptr<Ort::Session>(new Ort::Session(*envPtr.get(), wideModelName.c_str(), Ort::SessionOptions{}));
		allocatorPtr = std::unique_ptr<Ort::AllocatorWithDefaultOptions>(new Ort::AllocatorWithDefaultOptions());

		std::ifstream file(stdParamsFileName);
		std::vector<std::string> headers = getNextLineAndSplitIntoTokens(file);
		headers.erase(headers.begin());
		std::vector<std::string> strMeans = getNextLineAndSplitIntoTokens(file);
		strMeans.erase(strMeans.begin());
		std::vector<std::string> strVars = getNextLineAndSplitIntoTokens(file);
		strVars.erase(strVars.begin());

		standardParamMeans = std::vector<double>(strMeans.size());
		standardParamVars = std::vector<double>(strVars.size());
		for (size_t i = 0; i < strMeans.size(); i++)
		{
			standardParamMeans[i] = std::stof(strMeans[i]);
			standardParamVars[i] = std::stof(strVars[i]);
		}
	}

	FMTyieldmodel::FMTyieldmodel(const FMTyieldmodel& rhs) :
		FMTobject(), modelname(rhs.GetModelName())
	{
		std::wstring wideModelName = std::wstring(modelname.begin(), modelname.end());
		envPtr = std::unique_ptr<Ort::Env>(new Ort::Env());
		sessionPtr = std::unique_ptr<Ort::Session>(new Ort::Session(*envPtr.get(), wideModelName.c_str(), Ort::SessionOptions{}));
		allocatorPtr = std::unique_ptr<Ort::AllocatorWithDefaultOptions>(new Ort::AllocatorWithDefaultOptions());
		standardParamMeans = rhs.GetStandardParamMeans();
		standardParamVars = rhs.GetStandardParamVars();
	}

	const std::vector<double>& FMTyieldmodel::GetStandardParamMeans() const
	{
		return standardParamMeans;
	}

	const std::vector<double>& FMTyieldmodel::GetStandardParamVars() const
	{
		return standardParamVars;
	}

	const std::vector<const char*>& FMTyieldmodel::GetInputNames() const
	{
		return inputNames;
	}

	const std::vector<const char*>& FMTyieldmodel::GetOutputNames() const
	{
		return outputNames;
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

	const std::vector<std::string> FMTyieldmodel::getNextLineAndSplitIntoTokens(std::istream& str)
	{
		std::vector<std::string>   result;
		std::string                line;
		std::getline(str, line);

		std::stringstream          lineStream(line);
		std::string                cell;

		while (std::getline(lineStream, cell, ','))
		{
			result.push_back(cell);
		}
		// This checks for a trailing comma with no data after it.
		if (!lineStream && cell.empty())
		{
			// If there was a trailing comma then add an empty element.
			result.push_back("");
		}
		return result;
	}

	void FMTyieldmodel::standardize(std::vector<double>& input, const std::vector<double>& means, const std::vector<double>& vars) const
	{
		std::vector<double> output(input.size());
		for (size_t i = 0; i < input.size(); i++)
		{
			input[i] = (input[i] - means[i]) / sqrt(vars[i]);
		}
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
			const Graph::FMTgraphvertextoyield* graphinfo = request.getvertexgraphinfo();
			const Models::FMTmodel* modelptr = graphinfo->getmodel();
			const std::vector<std::string>modelinputyields(1, "VOLUMETOTAL");//Les yields inputs du modele 4 dans le cas de FORAC doit etre membre de FMTyieldmodel tu dois ajuster ca dans ta classe ici j'ai harcode nimporte quoi.
			const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>* linegraph = graphinfo->getlinegraph();
			const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>* fullgraph = graphinfo->getfullgraph();


			auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);


			if (linegraph != nullptr)//Im a linegraph
			{
				const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor* vertex = linegraph->getvertexfromvertexinfo(graphinfo);
				const std::vector<Graph::FMTcarbonpredictor>predictors = linegraph->getcarbonpredictors(*vertex, *modelptr, modelinputyields, 3);
				const Graph::FMTcarbonpredictor& predictor = predictors.at(0);//Seulement un predictor car on est un linegraph...								  //return dopredictionson(predictor.getpredictors()) Utiliser se vecteur de double (xs du modele) pour predire
				//Ta fonction doit retourner quelque chose qui ressemble a ça utilise getpredictors pour avoir tes doubles de prediction

				std::vector<double> inputs = predictor.getpredictors();
				std::vector<int64_t> shape = sessionPtr->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
				const Ort::Value input_tensor = Ort::Value::CreateTensor<double>(memoryInfo, inputs.data(), (size_t)inputs.size(), shape.data(), shape.size());
				standardize(inputs, GetStandardParamMeans(), GetStandardParamVars());
				std::vector<Ort::Value> output_tensors = sessionPtr->Run(nullptr, inputNames.data(), &input_tensor, inputNames.size(), outputNames.data(), outputNames.size());
				const double* outputValues = output_tensors[0].GetTensorMutableData<double>();
				return std::vector<double>(outputValues, outputValues + outputNames.size());

				//return dopredictionson(predictor.getpredictors())
			}
			else if (fullgraph != nullptr)//Im a full graph
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
				for (size_t inedgeid = 0; inedgeid < invariables.size(); ++inedgeid)
				{
					//Ta fonction doit remplire ve vecteur
					std::vector<double>edgeresults;// = dopredictionson(predictors.at(inedgeid).getpredictors());
					const double edgevalue = (*(solution + invariables.at(inedgeid)) / totalarea);
					for (size_t yldid = 0; yldid < results.size(); ++yldid)
					{
						results[yldid] += (edgeresults.at(yldid) * edgevalue);
					}
				}
				return results;
			}
			else {//Im nothing
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot use " + modelname + " yield model without graph info",
					"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
			}

			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Something went wrong in " + modelname,
				"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<double>();
	}
	
}