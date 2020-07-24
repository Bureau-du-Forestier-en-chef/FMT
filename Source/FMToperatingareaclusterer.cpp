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
	void FMToperatingareaclusterer::setinteger()
		{
		try {
		    std::vector<int>integervariables;
		    for (const FMToperatingareacluster& cluster : clusters)
				{
                integervariables.push_back(cluster.getcentroid().getvariable());
                for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
                    {
                    integervariables.push_back(binary.getvariable());
                    }
				}
            this->setInteger(&integervariables[0], static_cast<int>(integervariables.size()));
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::setinteger", __LINE__, __FILE__);
			}
		}

    double FMToperatingareaclusterer::getspreadprobability(const std::vector<FMToperatingareaclusterbinary>& incluster,const FMToperatingareaclusterbinary& target) const
        {
        double maxdifference = 0;
        for (const FMToperatingareaclusterbinary& binary : incluster)
            {
            const double difference = std::abs(binary.getstatistic()-target.getstatistic());
            if (difference > maxdifference)
                {
                maxdifference = difference;
                }

            }
        return (1-maxdifference);
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

    std::map<Core::FMTmask,std::vector<FMToperatingareaclusterbinary>>FMToperatingareaclusterer::getallbinaries() const
        {
        std::map<Core::FMTmask,std::vector<FMToperatingareaclusterbinary>>allbinaries;
        try {
            for (const FMToperatingareacluster& cluster : clusters)
                {
                if (allbinaries.find(cluster.getcentroid().getmask())!=allbinaries.end())
                    {
                    allbinaries[cluster.getcentroid().getmask()].push_back(cluster.getcentroid());
                    }else{
                    allbinaries[cluster.getcentroid().getmask()]=std::vector<FMToperatingareaclusterbinary>(1,cluster.getcentroid());
                    }
                for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
                    {
                    if (allbinaries.find(binary.getmask())!=allbinaries.end())
                        {
                        allbinaries[binary.getmask()].push_back(binary);
                        }else{
                        allbinaries[binary.getmask()]=std::vector<FMToperatingareaclusterbinary>(1,binary);
                        }
                    }
                }
        }catch (...)
            {
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::getallbinaries", __LINE__, __FILE__);
            }
        return allbinaries;
        }

	void FMToperatingareaclusterer::addobjective()
		{
		try {
		    std::map<Core::FMTmask,std::vector<FMToperatingareaclusterbinary>>choices = this->getallbinaries();
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
				for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
					{
                    this->addmaxminobjective(cluster,binary,choices.at(binary.getmask()));
					}
                }
			this->setObjSense(1.0);
		}catch (...)
            {
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addobjective", __LINE__, __FILE__);
            }
		}

    void FMToperatingareaclusterer::addlinksrows()
        {
        try {
            for (const FMToperatingareacluster& cluster : clusters)
                {
                const std::vector<FMToperatingareaclusterbinary>clusterbinaries = cluster.getbinaries();
                for (const FMToperatingareaclusterbinary& binary : clusterbinaries)
                    {
                    std::vector<double>linkvalues;
                    std::vector<int>linkindex;
                    linkvalues.push_back(-(static_cast<double>(binary.getneighbors().size())+1));
                    linkindex.push_back(binary.getvariable());
                    linkvalues.push_back(1.0);
                    linkindex.push_back(cluster.getcentroid().getvariable());
                    for (const Core::FMTmask& neighbor : binary.getneighbors())
                        {
                        std::vector<FMToperatingareaclusterbinary>::const_iterator neighborit = std::find_if(clusterbinaries.begin(),clusterbinaries.end(),FMToperatingareacomparator(neighbor));
                        linkvalues.push_back(1.0);
                        linkindex.push_back(neighborit->getvariable());
                        }
                    this->addRow(static_cast<int>(linkindex.size()),&linkindex[0],&linkvalues[0],0);
                    }

                }
        }catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addlinksrows", __LINE__, __FILE__);
			}
        }

    void FMToperatingareaclusterer::addforcingrows()
        {
        try {
		    std::map<Core::FMTmask,std::vector<FMToperatingareaclusterbinary>>choices = this->getallbinaries();
		    for (std::map<Core::FMTmask,std::vector<FMToperatingareaclusterbinary>>::const_iterator binit = choices.begin();binit!=choices.end();binit++)
                {
                std::vector<double>values;
                std::vector<int>indexes;
                for(const FMToperatingareaclusterbinary& neighbor : binit->second)
                    {
                    values.push_back(1.0);
                    indexes.push_back(neighbor.getvariable());
                    }
                this->addRow(static_cast<int>(indexes.size()),&indexes[0],&values[0],1.0,1.0);
                }

		}catch (...)
            {
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addforcingrows", __LINE__, __FILE__);
            }

        }


	void FMToperatingareaclusterer::addareaconstraints()
		{
		try {
			for (const FMToperatingareacluster& cluster : clusters)
				{
                std::vector<double>maxvalues(1,cluster.getcentroid().getarea());
                std::vector<int>maxindexes(1,cluster.getcentroid().getvariable());
                std::vector<double>minvalues(1,-minimalarea+cluster.getcentroid().getarea());
                std::vector<int>minindexes(1,cluster.getcentroid().getvariable());
                for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
                    {
                    maxvalues.push_back(binary.getarea());
                    maxindexes.push_back(binary.getvariable());
                    minvalues.push_back(binary.getarea());
                    minindexes.push_back(binary.getvariable());
                    }
                if(minimalarea>0)
                    {
                    this->addRow(static_cast<int>(minvalues.size()),&minindexes[0],&minvalues[0],0);
                    }
                if (maximalarea>0)
                    {
                    this->addRow(static_cast<int>(maxvalues.size()),&maxindexes[0],&maxvalues[0],-COIN_DBL_MAX,maximalarea);
                    }

				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addareaconstraints", __LINE__, __FILE__);
			}
		}

    void FMToperatingareaclusterer::addclusternumberconstraints()
        {
        try{
            std::vector<double>values;
            std::vector<int>indexes;
            for (const FMToperatingareacluster& cluster : clusters)
				{
                values.push_back(1);
                indexes.push_back(cluster.getcentroid().getvariable());
				}
            if(minimalnumberofclusters>0)
                {
                this->addRow(static_cast<int>(indexes.size()),&indexes[0],&values[0],minimalnumberofclusters);
                }
            if(maximalnumberofclusters>0)
                {
                this->addRow(static_cast<int>(indexes.size()),&indexes[0],&values[0],-COIN_DBL_MAX,maximalnumberofclusters);
                }
        }catch(...)
            {
            _exhandler->raisefromcatch("", "FMToperatingareaclusterer::addclusternumberconstraints", __LINE__, __FILE__);
            }
        }

   void FMToperatingareaclusterer::buildproblem()
        {
        try{
            this->addobjective();
            this->addlinksrows();
            this->addforcingrows();
            this->addclusternumberconstraints();
            this->addareaconstraints();
         }catch(...)
            {
            _exhandler->raisefromcatch("", "FMToperatingareaclusterer::buildproblem", __LINE__, __FILE__);
            }
        }

}

#endif
