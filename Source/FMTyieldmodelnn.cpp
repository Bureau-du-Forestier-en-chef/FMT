/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodelnn.hpp"
#include "FMTyieldrequest.hpp"
#include "FMTgraph.hpp"
#include "FMTlinegraph.hpp"
#include "FMTyields.hpp"
#include "FMTpredictor.hpp"
#include "FMTsrmodel.hpp"
#include <vector>
#include "FMTexceptionhandler.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <cmath>

#ifdef FMTWITHONNXR
	#include  <onnxruntime/core/session/onnxruntime_cxx_api.h>
#endif



namespace Core {

	#ifdef FMTWITHONNXR
		std::unique_ptr<Ort::Env> FMTyieldmodelnn::envPtr = std::unique_ptr<Ort::Env>(nullptr);
	#endif	

		const float FMTyieldmodelnn::UNKNOWN_DISTURBANCE_CODE = 17;


	FMTyieldmodelnn::FMTyieldmodelnn(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
	#ifdef FMTWITHONNXR
			:sessionPtr()
	#endif	
	{
		try {
			#ifdef FMTWITHONNXR
						if (!envPtr)
						{
							envPtr = std::unique_ptr<Ort::Env>(new Ort::Env());
						}
			#endif
			boost::filesystem::path fmtdll(getruntimelocation());
			boost::property_tree::ptree::const_assoc_iterator modelNameIt = jsonProps.find(JSON_PROP_MODEL_NAME);
			boost::filesystem::path filenamepath(modelNameIt->second.data());
			modelName = (fmtdll / filenamepath).string();

			boost::property_tree::ptree::const_assoc_iterator modelTypeIt = jsonProps.find(JSON_PROP_MODEL_TYPE);
			modelType = modelTypeIt->second.data();
			boost::property_tree::ptree::const_assoc_iterator stdParamsFileNameIt = jsonProps.find(JSON_PROP_STAND_FILE_PATH);

			boost::filesystem::path parampath(stdParamsFileNameIt->second.data());
			std::string stdParamsFileName = (fmtdll / parampath).string();

			std::wstring wideModelName = std::wstring(modelName.begin(), modelName.end());
			#ifdef FMTWITHONNXR
			sessionPtr = std::unique_ptr<Ort::Session>(new Ort::Session(*envPtr.get(), wideModelName.c_str(), Ort::SessionOptions{}));
			#endif
			std::ifstream file(stdParamsFileName);
			std::vector<std::string> headers = GetNextLineAndSplitIntoTokens(file);
			headers.erase(headers.begin());
			std::vector<std::string> strMeans = GetNextLineAndSplitIntoTokens(file);
			strMeans.erase(strMeans.begin());
			std::vector<std::string> strVars = GetNextLineAndSplitIntoTokens(file);
			strVars.erase(strVars.begin());

			std::vector<std::string> yields;
			for (auto& item : jsonProps.get_child(JSON_PROP_MODEL_YIELDS))
			{
				yields.push_back(item.second.get_value<std::string>());
			}

			ValidateInputYields(yields, inputYields);

			modelYields = inputYields;

			for (auto& item : jsonProps.get_child(JSON_PROP_MODEL_OUTPUTS))
			{
				modelOutputs.push_back(item.second.get_value<std::string>());
			}

			standardParamMeans = std::vector<float>(strMeans.size());
			standardParamVars = std::vector<float>(strVars.size());
			for (size_t i = 0; i < strMeans.size(); i++)
			{
				standardParamMeans[i] = std::stof(strMeans[i]);
				standardParamVars[i] = std::stof(strVars[i]);
			}
		}
		catch (...)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "",
				"FMTyieldmodelnn::FMTyieldmodelnn", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}




	FMTyieldmodelnn::FMTyieldmodelnn(const FMTyieldmodelnn& rhs):
		FMTyieldmodel(rhs),
		modelType(rhs.GetModelType()),
		standardParamMeans(rhs.GetStandardParamMeans()),
		standardParamVars(rhs.GetStandardParamVars()),
		modelOutputs(rhs.GetModelOutputNames())
	{
	#ifdef FMTWITHONNXR
		std::wstring wideModelName = std::wstring(modelName.begin(), modelName.end());
		sessionPtr = std::unique_ptr<Ort::Session>(new Ort::Session(*envPtr.get(), wideModelName.c_str(), Ort::SessionOptions{}));
	#endif
	}

	FMTyieldmodelnn::~FMTyieldmodelnn()
	{
	#ifdef FMTWITHONNXR
			sessionPtr->release();
	#endif
	}


	const std::vector<std::string> FMTyieldmodelnn::GetNextLineAndSplitIntoTokens(std::istream& str)
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

	const std::vector<float> FMTyieldmodelnn::Standardize(std::vector<float>& input, const std::vector<float>& means, const std::vector<float>& vars)
	{
		std::vector<float> output(input.size());
		for (size_t i = 0; i < input.size(); i++)
		{
			output[i] = (input[i] - means[i]) / sqrt(vars[i]);
		}

		return std::vector<float>(output.begin(), output.end());
	}
	
	void FMTyieldmodelnn::ValidateInputYields(std::vector<std::string>& expectedYields, std::vector<std::string>& inputYields) const
	{
		size_t expectedNbYld = expectedYields.size();
		size_t recievedNbYld = inputYields.size();
		if (expectedNbYld != recievedNbYld)
		{
			std::stringstream mylds;
			std::copy(expectedYields.begin(), expectedYields.end(), std::ostream_iterator<std::string>(mylds, ", "));
			std::string concatMYlds = mylds.str();
			if (concatMYlds.length() > 1)
				concatMYlds.erase(concatMYlds.end() - 2);
			_exhandler->raisefromcatch("Expected a different amount of yield. Expected " + std::to_string(expectedNbYld) + " yields : " + concatMYlds +
				". Got " + std::to_string(recievedNbYld) + " yields.",
				"FMTyieldmodel::FMTyieldmodelpools", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}


	const std::vector<double>FMTyieldmodelnn::Predict(const Core::FMTyieldrequest& request) const
	{
		try {
			const std::string mdlName = GetModelName();
		#ifdef FMTWITHONNXR
			const std::vector<std::string> modelYields = GetModelYields();
			auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
			Ort::AllocatorWithDefaultOptions allocator;
			#if ORT_API_VERSION <= 4
			char* inputName = sessionPtr->GetInputName(0, allocator);
			char* outputName = sessionPtr->GetOutputName(0, allocator);
			std::vector<const char*> inputNames{ inputName };
			std::vector<const char*> outputNames{ outputName };
			#else
			std::vector<Ort::AllocatedStringPtr> inputNodeNameAllocatedStrings;
			std::vector<Ort::AllocatedStringPtr> outputNodeNameAllocatedStrings;
			std::vector<const char*> inputNames;
			std::vector<const char*> outputNames;
			auto inputName = sessionPtr->GetInputNameAllocated(0, allocator);
			inputNodeNameAllocatedStrings.push_back(std::move(inputName));
			inputNames.push_back(inputNodeNameAllocatedStrings.back().get());
			auto outputName = sessionPtr->GetOutputNameAllocated(0, allocator);
			outputNodeNameAllocatedStrings.push_back(std::move(outputName));
			outputNames.push_back(outputNodeNameAllocatedStrings.back().get());
			#endif

			const Graph::FMTgraphvertextoyield* graphinfo = request.getvertexgraphinfo();
			const Models::FMTmodel* modelptr = graphinfo->getmodel();
			const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>* linegraph = graphinfo->getlinegraph();
			const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>* fullgraph = graphinfo->getfullgraph();

			std::vector<double> result(GetModelOutputNames().size(), 0.0);
			if (linegraph != nullptr)//Im a linegraph
			{
				const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor* vertex = linegraph->getvertexfromvertexinfo(graphinfo);
				const std::vector<Graph::FMTpredictor>predictors = linegraph->getpredictors(*vertex, *modelptr, modelYields, 3);
				if (predictors.empty())
				{
					_exhandler->raise(Exception::FMTexc::FMTrangeerror, "Empty predictors",
						"FMTyieldmodel::Predict", __LINE__, __FILE__);

				}
				const Graph::FMTpredictor& predictor = predictors.at(0);//Seulement un predictor car on est un linegraph...
				std::vector<double> inputsDbl = GetInputValues(predictor);
				std::vector<float> inputs(inputsDbl.begin(), inputsDbl.end());
				RemoveNans(inputs);
				std::vector<int64_t> inputShape = sessionPtr->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
				std::vector<float> stdInput = Standardize(inputs, GetStandardParamMeans(), GetStandardParamVars());
				const Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memoryInfo, stdInput.data(), stdInput.size(), inputShape.data(), inputShape.size());
				std::vector<Ort::Value> output_tensors = sessionPtr->Run(Ort::RunOptions{ nullptr }, inputNames.data(), &input_tensor, inputNames.size(), outputNames.data(), outputNames.size());
				std::vector<int64_t> outputShape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
				const float* outputValues = output_tensors[0].GetTensorMutableData<float>();
				std::vector<float> result_flt(outputValues, outputValues + ((int)outputShape.data()[1]));
				result = std::vector<double>(result_flt.begin(), result_flt.end());
				
			}
			else if (fullgraph != nullptr)//Im a full graph
			{
				const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor* vertex = fullgraph->getvertexfromvertexinfo(graphinfo);
				const std::vector<Graph::FMTpredictor>predictors = fullgraph->getpredictors(*vertex, *modelptr, modelYields, 3);
				if (predictors.empty())
					{
					_exhandler->raise(Exception::FMTexc::FMTrangeerror, "Empty predictors",
						"FMTyieldmodel::Predict", __LINE__, __FILE__);
					}
				//Dans un fullgraph il existe plusieurs predicteurs pour chaque noeud predictors.size() >= 1 <= a beaucoup
				//On peut faire du blackmagic pour aller chercher la solution existante de chaque predictor...
				const Models::FMTsrmodel* srmodelptr = dynamic_cast<const Models::FMTsrmodel*>(modelptr); //cast to a srmodel
				const Models::FMTlpsolver* solverptr = srmodelptr->getconstsolverptr(); //getsolver
				bool withoutsolution = false;
				if ((fullgraph->getbuildtype() == Graph::FMTgraphbuild::fullbuild) &&
					(!solverptr->isProvenOptimal()))
				{
					//Si le lpsolver n'est pas optimal faudrait trouver une autre solution...
					//C'est le cas qu'on veut optimiser du carbone
					//Peut-etre faire un "solution" avec des 1 partout pour representer chaque edge de façon equivalente?
					withoutsolution = true;
					//Pour l'instant on va mettre un message d'erreur
					//_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot use " + mdlName + " without solution",
					//	"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
				const double* solution = solverptr->getColSolution();
				const std::vector<int>invariables = fullgraph->getinvariables(*vertex);
				const std::vector<double>inproportions = fullgraph->getinproportions(*vertex);
				double totalarea = 0;
				if (!withoutsolution)
				{
					totalarea = fullgraph->inarea(*vertex, solution);
					/*for (size_t inedgeid = 0; inedgeid < invariables.size(); ++inedgeid)
					{
						totalarea += *(solution + invariables.at(inedgeid));
					}*/
				}else {
					if (invariables.size()>1)//Avertissement sur les valeurs de prédite car on applique un weight équivalent
					{
						_exhandler->raise(Exception::FMTexc::FMTyieldmodelprediction,"using "+ mdlName+
							" Multiple in edges for "+ std::string(request.getdevelopment()) ,
								"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
					}
					totalarea = static_cast<double>(invariables.size()); // we consider a solution of 1 everywhere
					}
				
				if (totalarea > 0)
				{
					for (size_t inedgeid = 0; inedgeid < invariables.size(); ++inedgeid)
					{
						const Graph::FMTpredictor& predictor = predictors.at(inedgeid);
						std::vector<double> inputsDbl = GetInputValues(predictor);
						std::vector<float> inputs(inputsDbl.begin(), inputsDbl.end());
						RemoveNans(inputs);
						std::vector<int64_t> inputShape = sessionPtr->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
						std::vector<float> stdInput = Standardize(inputs, GetStandardParamMeans(), GetStandardParamVars());
						const Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memoryInfo, stdInput.data(), stdInput.size(), inputShape.data(), inputShape.size());
						std::vector<Ort::Value> output_tensors = sessionPtr->Run(Ort::RunOptions{ nullptr }, inputNames.data(), &input_tensor, inputNames.size(), outputNames.data(), outputNames.size());
						std::vector<int64_t> outputShape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
						const float* outputValues = output_tensors[0].GetTensorMutableData<float>();
						std::vector<float> result_flt(outputValues, outputValues + ((int)outputShape.data()[1]));
						std::vector<double>edgeresults = std::vector<double>(result_flt.begin(), result_flt.end());
						double edgevalue = (1 / totalarea);
						if (!withoutsolution)
							{
							edgevalue = ((* (solution + invariables.at(inedgeid)) * (inproportions.at(inedgeid)/100)) / totalarea);
							}
						for (size_t yldid = 0; yldid < edgeresults.size(); ++yldid)
						{
							result[yldid] += (edgeresults.at(yldid) * edgevalue);
							
						}
					}
				}
			}
			else {//Im nothing
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot use " + mdlName + " yield model without graph info",
					"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
			}

			return result;
		#endif

			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Something went wrong in " + mdlName,
				"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<double>();
	}

	const std::string& FMTyieldmodelnn::GetModelType() const
	{
		return modelType;
	}

	const std::vector<float>& FMTyieldmodelnn::GetStandardParamMeans() const
	{
		return standardParamMeans;
	}

	const std::vector<float>& FMTyieldmodelnn::GetStandardParamVars() const
	{
		return standardParamVars;
	}

	const std::vector<std::string>& FMTyieldmodelnn::GetModelOutputNames() const
	{
		return modelOutputs;
	}

	const void FMTyieldmodelnn::RemoveNans(std::vector<float>& input) const
	{
		for (int i = 0; i < input.size(); i++)
		{
			if (std::isnan(input[i]))
			{
				if (i == 0 || i == 2 || i == 4)
					input[i] = 0;
				if (i == 1 || i == 3 || i == 5)
					input[i] = FMTyieldmodelnn::UNKNOWN_DISTURBANCE_CODE;
			}
		}
	}

}