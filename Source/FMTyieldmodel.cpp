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
	std::unique_ptr<Ort::Env> FMTyieldmodel::envPtr = std::unique_ptr<Ort::Env>(new Ort::Env());
	const float FMTyieldmodel::UNKNOWN_DISTURBANCE_CODE = 17;

	const std::vector<std::string> FMTyieldmodel::GetNextLineAndSplitIntoTokens(std::istream& str)
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

	const std::vector<float> FMTyieldmodel::Standardize(std::vector<float>& input, const std::vector<float>& means, const std::vector<float>& vars)
	{
		std::vector<float> output(input.size());
		for (size_t i = 0; i < input.size(); i++)
		{
			output[i] = (input[i] - means[i]) / sqrt(vars[i]);
		}

		return std::vector<float>(output.begin(), output.end());
	}

	//Might go in child classes instead
	const void FMTyieldmodel::RemoveNans(std::vector<float>& input)
	{
		for (int i = 0; i < input.size(); i++)
		{
			if (isnan(input[i]))
			{
				if (i == 0 || i == 2 || i == 4)
					input[i] = 0;
				if (i == 1 || i == 3 || i == 5)
					input[i] = FMTyieldmodel::UNKNOWN_DISTURBANCE_CODE;
			}
		}
	}
	
	const std::vector<double>FMTyieldmodel::Predict(const Core::FMTyieldrequest& request) const
	{
		try {
			std::string mdlName = GetModelName();
			std::wstring wideModelName = std::wstring(mdlName.begin(), mdlName.end());
			const std::vector<std::string> modelYields = GetModelYields();
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
				const std::vector<Graph::FMTcarbonpredictor>predictors = linegraph->getcarbonpredictors(*vertex, *modelptr, modelYields, 3);
				const Graph::FMTcarbonpredictor& predictor = predictors.at(0);//Seulement un predictor car on est un linegraph...								  //return dopredictionson(predictor.getpredictors()) Utiliser se vecteur de double (xs du modele) pour predire
				//Ta fonction doit retourner quelque chose qui ressemble a ça utilise getpredictors pour avoir tes doubles de prediction

				std::vector<int64_t> input_shape = sessionPtr->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
				char* inputName = sessionPtr->GetInputName(0, allocator);
				std::vector<int64_t> output_shapes = sessionPtr->GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
				char* outputName = sessionPtr->GetOutputName(0, allocator);
				std::vector<const char*> inputNames{ inputName };
				std::vector<const char*> outputNames{ outputName };

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
				const std::vector<Graph::FMTcarbonpredictor>predictors = fullgraph->getcarbonpredictors(*vertex, *modelptr, modelYields, 3);
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
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot use " + mdlName + " without solution",
						"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
				const double* solution = solverptr->getColSolution();
				const std::vector<int>invariables = fullgraph->getinvariables(*vertex);
				double totalarea = 0;
				std::vector<double>result(GetYieldsOutputs().size(), 0.0);
				for (size_t inedgeid = 0; inedgeid < invariables.size(); ++inedgeid)
				{
					totalarea += *(solution + invariables.at(inedgeid));
				}
				for (size_t inedgeid = 0; inedgeid < invariables.size(); ++inedgeid)
				{
					//const Graph::FMTcarbonpredictor& predictor = predictors.at(inedgeid)
					//Ta fonction doit remplire ve vecteur
					std::vector<double>edgeresults;// = dopredictionson(predictors.at(inedgeid).getpredictors());
					const double edgevalue = (*(solution + invariables.at(inedgeid)) / totalarea);
					for (size_t yldid = 0; yldid < result.size(); ++yldid)
					{
						result[yldid] += (edgeresults.at(yldid) * edgevalue);
					}
				}
			}
			else {//Im nothing
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot use " + mdlName + " yield model without graph info",
					"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
			}

			sessionPtr->release();

			return result;

			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Something went wrong in " + mdlName,
				"FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<double>();
	}
}