#ifdef FMTWITHOSI

#include "FMToperatingareaclusterer.h"


namespace Heuristics
{
	FMToperatingareaclusterer::FMToperatingareaclusterer(const std::vector<FMToperatingareacluster>& lclusters,
		const double& lminimalarea, const double& lmaximalarea,
		const int& minimalnumberofclusters, const int& maximalnumberofclusters):
		clusters(lclusters),
		minimalarea(lminimalarea),
		maximalarea(lmaximalarea),
		minimalnumberofclusters(minimalnumberofclusters),
		maximalnumberofclusters(maximalnumberofclusters)
	{

	}

	void FMToperatingareaclusterer::buildclustersvariables()
		{
		try {
			std::vector<FMToperatingareacluster>newclusterswithvariables;
			std::vector<int>variablesadded;
			int variableid = 0;
			for (const FMToperatingareacluster& cluster : clusters)
				{
				FMToperatingareaclusterbinary newcentroid = cluster.getcentroid();
				newcentroid.setvariable(variableid);
				this->addCol(0, nullptr, nullptr, 0.0, 1.0, 0.0);
				variablesadded.push_back(variableid);
				++variableid;
				std::vector<FMToperatingareaclusterbinary>newbinaries = cluster.getbinaries();
				for (FMToperatingareaclusterbinary& neighbor : newbinaries)
					{
					neighbor.setvariable(variableid);
					variablesadded.push_back(variableid);
					this->addCol(0, nullptr, nullptr, 0.0, 1.0, 0.0);
					++variableid;
					}
				newclusterswithvariables.push_back(FMToperatingareacluster(newcentroid, newbinaries));
				}
			this->setInteger(&variablesadded[0], static_cast<int>(variablesadded.size()));
			clusters = newclusterswithvariables;
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::buildclustersvariables", __LINE__, __FILE__);
			}

		}

	void FMToperatingareaclusterer::addmaxminobjective(const FMToperatingareacluster& cluster, const FMToperatingareaclusterbinary& binary,
		const std::vector<FMToperatingareaclusterbinary>& binaries)
	{
		try {
			std::vector<double>maxvariables;
			maxvariables.push_back(binary.getstatistic());
			maxvariables.push_back(-1.0);
			std::vector<int>indexesofmaxvariables;
			indexesofmaxvariables.push_back(binary.getvariable());
			indexesofmaxvariables.push_back(cluster.getmaximalobjectivevariable());
			this->addRow(2, &indexesofmaxvariables[0], &maxvariables[0], -COIN_DBL_MAX,1.0);
			std::vector<double>minvariables;
			std::vector<int>indexesofminvariables;
			for (const FMToperatingareaclusterbinary& localbinary : binaries)
				{
				if (binary.getvariable()!=localbinary.getvariable())
					{
					minvariables.push_back(-1.0);
					indexesofminvariables.push_back(localbinary.getvariable());
				}else {
					minvariables.push_back(-binary.getstatistic());
					indexesofminvariables.push_back(binary.getvariable());
					}
				}
			minvariables.push_back(-1.0);
			indexesofmaxvariables.push_back(cluster.getminimalobjectivevariable());
			this->addRow(static_cast<int>(indexesofminvariables.size()), &indexesofminvariables[0], &minvariables[0], -COIN_DBL_MAX, 1.0);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addmaxminobjective", __LINE__, __FILE__);
			}
		}

	void FMToperatingareaclusterer::addobjective()
		{
		try {
			for (FMToperatingareacluster& cluster : clusters)
			{
				int columnid = this->getNumCols();
				cluster.setmaximalobjectivevariable(columnid);
				this->addCol(0, nullptr, nullptr,0.0, 1.0, 1.0);
				++columnid;
				cluster.setminimalobjectivevariable(columnid);
				this->addCol(0, nullptr, nullptr,-1.0,0, 1.0);
				std::vector<double>variableclosed(2,-1);
				std::vector<int>columnidclosed;
				columnidclosed.push_back(cluster.getcentroid().getvariable());
				columnidclosed.push_back(cluster.getminimalobjectivevariable());
				this->addRow(2, &columnidclosed[0], &variableclosed[0],-COIN_DBL_MAX, 0.0);
				this->addmaxminobjective(cluster, cluster.getcentroid(), cluster.getbinaries());
				/*for (const FMToperatingareaclusterbinary& binary : clusters.getbinaries())
					{

					}*/


			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addobjective", __LINE__, __FILE__);
		}
		}


	void FMToperatingareaclusterer::addareaconstraints()
		{
		try {
			for (FMToperatingareacluster& cluster : clusters)
				{

				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addareaconstraints", __LINE__, __FILE__);
			}
		}

}

#endif