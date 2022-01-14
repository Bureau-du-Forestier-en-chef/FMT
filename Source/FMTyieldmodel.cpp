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
	std::unique_ptr<Ort::Env> FMTyieldmodel::envPtr = std::unique_ptr<Ort::Env>(new Ort::Env());

	const std::string JSON_PROP_MODEL_NAME = "modelFileName";
	const std::string JSON_PROP_MODEL_TYPE = "modelType";
	const std::string JSON_PROP_STAND_FILE_PATH = "csvStandardisationFile";

	FMTyieldmodel::FMTyieldmodel(const boost::property_tree::ptree& jsonProps) :
		FMTobject(), modelName(), modelType()
	{
		boost::property_tree::ptree::const_assoc_iterator modelNameIt = jsonProps.find(JSON_PROP_MODEL_NAME);
		modelName = modelNameIt->second.data();
		boost::property_tree::ptree::const_assoc_iterator modelTypeIt = jsonProps.find(JSON_PROP_MODEL_TYPE);
		modelType = modelTypeIt->second.data();
		boost::property_tree::ptree::const_assoc_iterator stdParamsFileNameIt = jsonProps.find(JSON_PROP_STAND_FILE_PATH);
		std::string stdParamsFileName = stdParamsFileNameIt->second.data();

		/*ortApi = OrtGetApiBase()->GetApi(ORT_API_VERSION);
		ortApi->CreateEnv(ORT_LOGGING_LEVEL_ERROR, "test", &envPtr);
		ortApi->CreateSessionOptions(&sessionOptionsPtr);
		ortApi->CreateSession(envPtr, wideModelName.c_str(), sessionOptionsPtr, &sessionPtr);*/

		std::ifstream file(stdParamsFileName);
		std::vector<std::string> headers = getNextLineAndSplitIntoTokens(file);
		headers.erase(headers.begin());
		std::vector<std::string> strMeans = getNextLineAndSplitIntoTokens(file);
		strMeans.erase(strMeans.begin());
		std::vector<std::string> strVars = getNextLineAndSplitIntoTokens(file);
		strVars.erase(strVars.begin());

		standardParamMeans = std::vector<float>(strMeans.size());
		standardParamVars = std::vector<float>(strVars.size());
		for (size_t i = 0; i < strMeans.size(); i++)
		{
			standardParamMeans[i] = std::stof(strMeans[i]);
			standardParamVars[i] = std::stof(strVars[i]);
		}
	}

	FMTyieldmodel::FMTyieldmodel(const FMTyieldmodel& rhs) :
		FMTobject(), 
		modelName(rhs.GetModelName()),
		modelType(rhs.GetModelType()), 
		standardParamMeans(rhs.GetStandardParamMeans()), 
		standardParamVars(rhs.GetStandardParamVars())
	{
	}

	const std::vector<float>& FMTyieldmodel::GetStandardParamMeans() const
	{
		return standardParamMeans;
	}

	const std::vector<float>& FMTyieldmodel::GetStandardParamVars() const
	{
		return standardParamVars;
	}

	const std::string& FMTyieldmodel::GetModelName() const
	{
		return modelName;
	}

	const std::string& FMTyieldmodel::GetModelType() const
	{
		return modelType;
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

	const std::vector<float> FMTyieldmodel::standardize(std::vector<float>& input, const std::vector<float>& means, const std::vector<float>& vars) const
	{
		std::vector<float> output(input.size());
		for (size_t i = 0; i < input.size(); i++)
		{
			output[i] = (input[i] - means[i]) / sqrt(vars[i]);
		}

		return std::vector<float>(output.begin(), output.end());
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
			std::vector<std::string> modelinputyields;
			if (modelType == "POOLS")
			{
				modelinputyields.push_back("YV_G_GFI");
				modelinputyields.push_back("YV_G_GFT");
				modelinputyields.push_back("YV_G_GR");
				modelinputyields.push_back("YV_G_GF");
			}

			std::wstring wideModelName = std::wstring(modelName.begin(), modelName.end());
			std::unique_ptr<Ort::Session> sessionPtr = std::unique_ptr<Ort::Session>(new Ort::Session(*envPtr.get(), wideModelName.c_str(), Ort::SessionOptions{}));
			auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
			Ort::AllocatorWithDefaultOptions allocator;

			const Graph::FMTgraphvertextoyield* graphinfo = request.getvertexgraphinfo();
			const Models::FMTmodel* modelptr = graphinfo->getmodel();
			//const std::vector<std::string>modelinputyields(1, "VOLUMETOTAL");//Les yields inputs du modele 4 dans le cas de FORAC doit etre membre de FMTyieldmodel tu dois ajuster ca dans ta classe ici j'ai harcode nimporte quoi.
			const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>* linegraph = graphinfo->getlinegraph();
			const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>* fullgraph = graphinfo->getfullgraph();

			std::vector<double> result;
			if (linegraph != nullptr)//Im a linegraph
			{
				const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor* vertex = linegraph->getvertexfromvertexinfo(graphinfo);
				const std::vector<Graph::FMTcarbonpredictor>predictors = linegraph->getcarbonpredictors(*vertex, *modelptr, modelinputyields, 3);
				const Graph::FMTcarbonpredictor& predictor = predictors.at(0);//Seulement un predictor car on est un linegraph...								  //return dopredictionson(predictor.getpredictors()) Utiliser se vecteur de double (xs du modele) pour predire
				//Ta fonction doit retourner quelque chose qui ressemble a ça utilise getpredictors pour avoir tes doubles de prediction

				std::vector<int64_t> input_shape = sessionPtr->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
				char* inputName = sessionPtr->GetInputName(0, allocator);
				std::vector<int64_t> output_shapes = sessionPtr->GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
				char* outputName = sessionPtr->GetOutputName(0, allocator);
				std::vector<const char*> inputNames{ inputName };
				std::vector<const char*> outputNames{ outputName };

				//std::vector<double> inputsDbl = predictor.getpredictors();
				std::vector<double> inputsDbl{ 17, 1, 47, 10, 1, 17, 16, 2.49, 0, 85.47, 2.49 };
				std::vector<float> inputs(inputsDbl.begin(), inputsDbl.end());
				std::vector<int64_t> inputShape = sessionPtr->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
				std::vector<float> stdInput = standardize(inputs, GetStandardParamMeans(), GetStandardParamVars());
				const Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memoryInfo, stdInput.data(), stdInput.size(), inputShape.data(), inputShape.size());
				std::vector<Ort::Value> output_tensors = sessionPtr->Run(Ort::RunOptions{nullptr}, inputNames.data(), &input_tensor, inputNames.size(), outputNames.data(), outputNames.size());
				std::vector<int64_t> outputShape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
				const float* outputValues = output_tensors[0].GetTensorMutableData<float>();
				std::vector<float> result_flt(outputValues, outputValues + ((int)outputShape.data()[1]));
				result = std::vector<double>(result_flt.begin(), result_flt.end());
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
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot use " + modelName + " without solution",
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
				//return results;
			}
			else {//Im nothing
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot use " + modelName + " yield model without graph info",
					"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
			}

			sessionPtr->release();

			return result;

			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Something went wrong in " + modelName,
				"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<double>();
	}
	
}