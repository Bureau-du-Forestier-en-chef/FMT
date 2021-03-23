/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#include "FMToperatingareaclusterer.h"
#include <random>



namespace Heuristics
{
	FMToperatingareaclusterer::FMToperatingareaclusterer(const Models::FMTsolverinterface& interfacetype,const size_t& lseed,
		const std::vector<FMToperatingareacluster>& lclusters,int minimalnumberofclusters, int maximalnumberofclusters):
	    FMTlpheuristic(interfacetype,lseed),
		clusters(lclusters),
		numberofsimulationpass(100),
		minimalnumberofclusters(minimalnumberofclusters),
		maximalnumberofclusters(maximalnumberofclusters)
	{
		
	}

	FMToperatingareaclusterer::FMToperatingareaclusterer(const FMToperatingareaclusterer& rhs):
		FMTlpheuristic(rhs),
		clusters(rhs.clusters),
		numberofsimulationpass(rhs.numberofsimulationpass),
		minimalnumberofclusters(rhs.minimalnumberofclusters),
		maximalnumberofclusters(rhs.maximalnumberofclusters)
	{

	}

	void FMToperatingareaclusterer::setallinteger()
		{
		try {
		    const std::vector<int>integervariables = this->getbinariesvariables();
            this->setInteger(&integervariables[0], static_cast<int>(integervariables.size()));
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::setallinteger", __LINE__, __FILE__);
			}
		}

    void FMToperatingareaclusterer::unboundall()
		{
		try {
		    const std::vector<int>integervariables = this->getbinariesvariables();
		    std::vector<double>colsbounds;
		    for (const int& variable : integervariables)
                {
                colsbounds.push_back(0);
                colsbounds.push_back(1);
                }
            this->setColSetBounds(&integervariables[0],&integervariables.back() + 1,&colsbounds[0]);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::unboundall", __LINE__, __FILE__);
			}
		}

    double FMToperatingareaclusterer::getspreadprobability(const std::vector<FMToperatingareaclusterbinary>& incluster,const FMToperatingareaclusterbinary& target) const
        {
        double maxdifference = 0;
        try {
        for (const FMToperatingareaclusterbinary& binary : incluster)
            {
            const double difference = std::abs(binary.getstatistic()-target.getstatistic());
            if (difference > maxdifference)
                {
                maxdifference = difference;
                }

            }
        }catch(...)
            {
            _exhandler->raisefromcatch("", "FMToperatingareaclusterer::getspreadprobability", __LINE__, __FILE__);
            }
        return (1-maxdifference);
        }
	bool FMToperatingareaclusterer::isvalidarea(const FMToperatingareacluster& cluster, const double& area, const size_t& actives) const
	{
		try {
			if (cluster.isvalidarea(area))
			{
				if (maximalnumberofclusters>0)
				{
					const double totalarea = gettotalarea();
					const double minimal = std::max((totalarea / maximalnumberofclusters), cluster.getminimalarea());
					return (area >= minimal || (area < minimal && actives==0));
				}else {
					return true;
				}
				
			}

		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::gettargetedoperatingareasize", __LINE__, __FILE__);
			}
		return false;
	}
   double FMToperatingareaclusterer::gettargetedoperatingareasize(const FMToperatingareacluster& target)
        {
        double returnedsize = 0;
        try{
            double minimaltarget = target.getminimalarea();
            double maximaltarget = target.getmaximalarea();
			if (minimalnumberofclusters>0)
				{
				minimaltarget = std::max(gettotalarea()/ minimalnumberofclusters, minimaltarget);
				}
            std::uniform_real_distribution<double>areadistribution(minimaltarget,maximaltarget);
            returnedsize = areadistribution(generator);
        }catch(...)
            {
            _exhandler->raisefromcatch("", "FMToperatingareaclusterer::gettargetedoperatingareasize", __LINE__, __FILE__);
            }
        return returnedsize;
        }

    bool FMToperatingareaclusterer::spread(const FMToperatingareacluster& ignition,std::vector<FMToperatingareaclusterbinary>& assigned)
        {
        try{
			if (std::find_if(assigned.begin(), assigned.end(), FMToperatingareacomparator(ignition.getcentroid())) != assigned.end())
				{
				return true;
				}
            double firesize = ignition.getcentroid().getarea();
            std::vector<FMToperatingareaclusterbinary>incluster(1,ignition.getcentroid());
            std::vector<FMToperatingareaclusterbinary>outcluster;
            std::vector<FMToperatingareaclusterbinary>actives;
			
            for (const FMToperatingareaclusterbinary& binary : ignition.getbinaries())
                {
                if (std::find_if(assigned.begin(),assigned.end(),FMToperatingareacomparator(binary))==assigned.end())
                    {
                    if (binary.getneighbors().empty())
                        {
                        actives.push_back(binary);
                    }else{
                        outcluster.push_back(binary);
                        }
                    }
                }
            const double maximalfiresize = this->gettargetedoperatingareasize(ignition);
			while((!actives.empty()|| isvalidarea(ignition,firesize, actives.size())) && firesize <= maximalfiresize)
                {
				//*_logger << "fire size " << firesize << " " << maximalfiresize << "\n";
				if (!isvalidarea(ignition,firesize,actives.size()) && !actives.empty())
					{
						std::vector<double>probabilities;
						double totaldifference = 0;
						for (const FMToperatingareaclusterbinary& active : actives)
							{
							const double difference = getspreadprobability(incluster,active);
							totaldifference+=difference;
							probabilities.push_back(difference);
							}
						std::vector<int>intprobability;
						for(const double& probability : probabilities)
							{
							intprobability.push_back(static_cast<int>(probability/totaldifference)*100);
							}
						std::discrete_distribution<int>spreaddistribution(intprobability.begin(),intprobability.end());
						const int selection = spreaddistribution(generator);
						//*_logger << "active size " << actives.size() << "\n";
						const FMToperatingareaclusterbinary selected = actives.at(selection);
						actives.erase(actives.begin()+selection);
						incluster.push_back(selected);
						firesize+=selected.getarea();
						std::vector<FMToperatingareaclusterbinary>updatedoutcluster;
						for (const FMToperatingareaclusterbinary& outbinary : outcluster)
							{
							const std::vector<Core::FMTmask>neighbors = outbinary.getneighbors();
							if (std::find_if(neighbors.begin(),neighbors.end(),Core::FMTmaskcomparator(selected.getmask()))!=neighbors.end())
								{
									int fullsize = static_cast<int>(neighbors.size());
									for (const FMToperatingareaclusterbinary& inbinary : incluster)
										{
										if (std::find_if(neighbors.begin(),neighbors.end(),Core::FMTmaskcomparator(inbinary.getmask()))!=neighbors.end())
											{
											--fullsize;
											}

										}
									if(fullsize == 0)
										{
										actives.push_back(outbinary);
										}else{
										updatedoutcluster.push_back(outbinary);
										}

								}else{
								updatedoutcluster.push_back(outbinary);
								}
							}
						outcluster = updatedoutcluster;
					}
                  
                    if (isvalidarea(ignition,firesize, actives.size()))
                        {
						//*_logger << "valid! fire size " << firesize << " " << maximalfiresize << "\n";
						std::vector<double>bounds;
						std::vector<int>indexes;
                        for (const FMToperatingareaclusterbinary& inbinary : incluster)
                            {
                            assigned.push_back(inbinary);
                            bounds.push_back(1.0);
                            bounds.push_back(1.0);
                            indexes.push_back(inbinary.getvariable());
                            }
                        this->setColSetBounds(&indexes[0],&indexes.back() + 1,&bounds[0]);
                        return true;
                        }
                    }
			//*_logger << "out! "<< isvalidarea(ignition, firesize, actives.size()) << "\n";
        }catch(...)
            {
            _exhandler->raisefromcatch("", "FMToperatingareaclusterer::spread", __LINE__, __FILE__);
            }
        return false;
        }

    void FMToperatingareaclusterer::setnumberofsimulationpass(const int& pass)
        {
        numberofsimulationpass = pass;
        }

    std::vector<int>FMToperatingareaclusterer::getbinariesvariables() const
        {
        std::vector<int>varindexes;
        for (const FMToperatingareacluster& cluster : clusters)
            {
            varindexes.push_back(cluster.getcentroid().getvariable());
            for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
                {
                varindexes.push_back(binary.getvariable());
                }
            }
        return varindexes;
        }

	size_t FMToperatingareaclusterer::getbinariescount() const
		{
		size_t returnsize = 0;
		try {
			returnsize = getallbinaries().size();
		}catch (...)
			{
			_exhandler->printexceptions("", "FMToperatingareaclusterer::getbinariescount", __LINE__, __FILE__);
			}
		return returnsize;
		}

    bool FMToperatingareaclusterer::initialsolve()
        {
        try{
            double passleft =  numberofsimulationpass;
			bool gotonesolution = false;
            if (Models::FMTlpsolver::initialsolve())
                {
                double bestobjectivevalue = 0;
                std::vector<double>bestcolsbound;
                const std::vector<int>varindexes = this->getbinariesvariables();
				size_t iteration = 0;
                while(passleft>0)
                    {
                    this->unboundall();
                    std::vector<FMToperatingareaclusterbinary>assigned;
                    if (Models::FMTlpsolver::resolve())
                        {
                        std::vector<FMToperatingareacluster>clustertospread=clusters;
                        std::shuffle(clustertospread.begin(),clustertospread.end(),generator);
                        size_t iterationdone = 0;
                        while(!clustertospread.empty() && iterationdone < (clusters.size()*2))
                            {
                            if (this->spread(*clustertospread.begin(),assigned))
                                {
                                clustertospread.erase(clustertospread.begin());
                                }
                            ++iterationdone;
                            }
                        if (clustertospread.empty() && Models::FMTlpsolver::resolve() &&
                            (!gotonesolution||(Models::FMTlpsolver::getObjValue() < bestobjectivevalue)))
                            {
                            bestcolsbound.clear();
                            const double* upperbound = Models::FMTlpsolver::getColUpper();
                            const double* lowerbound = Models::FMTlpsolver::getColLower();
                            for (const FMToperatingareacluster& cluster : clusters)
                                {
                                const int centroidvar = cluster.getcentroid().getvariable();
                                bestcolsbound.push_back(*(lowerbound+centroidvar));
                                bestcolsbound.push_back(*(upperbound+centroidvar));
                                for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
                                    {
                                    const int binaryvar = binary.getvariable();
                                    bestcolsbound.push_back(*(lowerbound+ binaryvar));
                                    bestcolsbound.push_back(*(upperbound+ binaryvar));
									
                                    }
                                }
                            bestobjectivevalue = Models::FMTlpsolver::getObjValue();
							if (!gotonesolution)
								{
								_logger->logwithlevel<std::string>("Feasible solution found\n", 0);
								}
							_logger->logwithlevel<std::string>("Obj(" +
								std::to_string(bestobjectivevalue) + ") it(" + std::to_string(iteration) + ")\n", 0);
							gotonesolution = true;
                            }
                        }
                    --passleft;
					++iteration;
                    }
                if (bestcolsbound.empty())
                    {
                    this->unboundall();
                    return false;
                    }else{
                        this->setColSetBounds(&varindexes[0],&varindexes.back() + 1,&bestcolsbound[0]);
					
					return Models::FMTlpsolver::resolve();
                    }
                }
            }catch(...)
                {
                _exhandler->printexceptions("", "FMToperatingareaclusterer::initialsolve", __LINE__, __FILE__);
                }
        return false;
        }

    std::vector<FMToperatingareacluster>FMToperatingareaclusterer::getsolution() const
        {
        std::vector<FMToperatingareacluster>solution;
        try{
            const double* primalsolution = Models::FMTlpsolver::getColSolution();
            for (const FMToperatingareacluster& cluster : clusters)
                {
                if (*(primalsolution+cluster.getcentroid().getvariable())>0.5)//active cluster
                    {
                    std::vector<FMToperatingareaclusterbinary>selectedbinaries;
                    for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
                        {
                        if (*(primalsolution+binary.getvariable())>0.5)//active binary
                            {
                            selectedbinaries.push_back(binary);
                            }
                        }
                    solution.push_back(FMToperatingareacluster(FMToperatingareacluster(cluster.getcentroid(),selectedbinaries),cluster.getrealminimalarea(),cluster.getrealmaximalarea()));
                    }
                }
        }catch(...)
            {
            _exhandler->printexceptions("", "FMToperatingareaclusterer::getsolution", __LINE__, __FILE__);
            }
        return solution;
        }


	void FMToperatingareaclusterer::buildclustersvariables()
		{
		try {
			std::vector<FMToperatingareacluster>newclusterswithvariables;
			std::vector<int>variablesadded;
			int variableid = 0;
			int clusterid = 0;
			for (const FMToperatingareacluster& cluster : clusters)
				{
				FMToperatingareaclusterbinary newcentroid = cluster.getcentroid();
				newcentroid.setvariable(variableid);
				this->addCol(0, nullptr, nullptr, 0.0, 1.0, 0.0);
				this->setcolname("C_" + std::to_string(clusterid) + "_" + std::string(cluster.getcentroid().getmask()), variableid);
				variablesadded.push_back(variableid);
				++variableid;
				std::vector<FMToperatingareaclusterbinary>newbinaries = cluster.getbinaries();
				for (FMToperatingareaclusterbinary& neighbor : newbinaries)
					{
					neighbor.setvariable(variableid);
					this->setcolname("B_" + std::to_string(clusterid) + "_" + std::string(neighbor.getmask()), variableid);
					variablesadded.push_back(variableid);
					this->addCol(0, nullptr, nullptr, 0.0, 1.0, 0.0);
					++variableid;
					}
				newclusterswithvariables.push_back(FMToperatingareacluster(FMToperatingareacluster(newcentroid, newbinaries),cluster.getrealminimalarea(),cluster.getrealmaximalarea()));
				++clusterid;
				}
			clusters = newclusterswithvariables;
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::buildclustersvariables", __LINE__, __FILE__);
			}

		}

	void FMToperatingareaclusterer::addmaxminobjective(const FMToperatingareacluster& cluster, const FMToperatingareaclusterbinary& binary,
		const std::vector<FMToperatingareaclusterbinary>& binaries,
		const int& clusterid)
	{
		try {
			std::vector<double>maxvariables;
			maxvariables.push_back(binary.getstatistic());
			maxvariables.push_back(-1.0);
			std::vector<int>indexesofmaxvariables;
			indexesofmaxvariables.push_back(binary.getvariable());
			indexesofmaxvariables.push_back(cluster.getmaximalobjectivevariable());
			setrowname("MAXO_" + std::to_string(clusterid) + "_" +std::string(binary.getmask()), getNumRows());
			this->addRow(2, &indexesofmaxvariables[0], &maxvariables[0], -COIN_DBL_MAX,0);
			/*std::vector<double>baseminvariables;
			baseminvariables.push_back(-binary.getstatistic());
			baseminvariables.push_back(-1.0);
			std::vector<int>baseindexesofminvariables;
			baseindexesofminvariables.push_back(binary.getvariable());
			baseindexesofminvariables.push_back(cluster.getminimalobjectivevariable());
			this->addRow(2, &baseindexesofminvariables[0], &baseminvariables[0], -COIN_DBL_MAX, 0);*/

			
			std::vector<double>minvariables;
			std::vector<int>indexesofminvariables;
			const double maxincluster = cluster.getmaximalstats();
			minvariables.push_back(-binary.getstatistic());
			indexesofminvariables.push_back(binary.getvariable());
			for (const FMToperatingareaclusterbinary& localbinary : binaries)
				{
				if (binary.getvariable()!=localbinary.getvariable())
					{
					minvariables.push_back(-maxincluster);
					indexesofminvariables.push_back(localbinary.getvariable());
					}
				}
			minvariables.push_back(-1.0);
			indexesofminvariables.push_back(cluster.getminimalobjectivevariable());
			setrowname("MINO_" + std::to_string(clusterid) + "_" +std::string(binary.getmask()), getNumRows());
			this->addRow(static_cast<int>(indexesofminvariables.size()), &indexesofminvariables[0], &minvariables[0], -COIN_DBL_MAX, 0.0);
		
		
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addmaxminobjective", __LINE__, __FILE__);
			}
		}

	double FMToperatingareaclusterer::gettotalarea() const
	{
		double totalarea = 0;
		try {
			for (const auto& all : getallbinaries())
			{
				totalarea += (all.second.begin()->getarea());
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "MToperatingareaclusterer::gettotalarea", __LINE__, __FILE__);
		}
		return totalarea;
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
			int clusterid = 0;
			for (FMToperatingareacluster& cluster : clusters)
                {
				int columnid = this->getNumCols();
				cluster.setmaximalobjectivevariable(columnid);
				this->addCol(0, nullptr, nullptr,0.0,COIN_DBL_MAX, 1.0);
				setcolname("MAX" + std::to_string(clusterid), columnid);
				++columnid;
				cluster.setminimalobjectivevariable(columnid);
				this->addCol(0, nullptr, nullptr,-COIN_DBL_MAX,0, 1.0);
				setcolname("MIN" + std::to_string(clusterid), columnid);
				std::vector<double>variableclosed;
				const double maxincluster = cluster.getmaximalstats();
				variableclosed.push_back(-maxincluster);
				variableclosed.push_back(-1);
				std::vector<int>columnidclosed;
				columnidclosed.push_back(cluster.getcentroid().getvariable());
				columnidclosed.push_back(cluster.getminimalobjectivevariable());
				setrowname("OF" + std::to_string(clusterid), getNumRows());
				this->addRow(2, &columnidclosed[0], &variableclosed[0], -COIN_DBL_MAX,0.0);
				this->addmaxminobjective(cluster, cluster.getcentroid(), choices.at(cluster.getcentroid().getmask()),clusterid);
				for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
					{
                    this->addmaxminobjective(cluster,binary,choices.at(binary.getmask()), clusterid);
					}

				/*std::vector<double>alllowerpar;
				std::vector<int>alllowervar;
				alllowervar.push_back(cluster.getcentroid().getvariable());
				alllowerpar.push_back(-cluster.getcentroid().getstatistic());
				for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
					{
					alllowervar.push_back(binary.getvariable());
					alllowerpar.push_back(-binary.getstatistic());
					}
				alllowerpar.push_back(-1);
				alllowervar.push_back(cluster.getminimalobjectivevariable());
				this->addRow(static_cast<int>(alllowervar.size()), &alllowervar[0], &alllowerpar[0], -COIN_DBL_MAX, 0);*/
				++clusterid;
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
			int clusterid = 0;
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
					setrowname("L" + std::to_string(clusterid) + "_" + std::string(binary.getmask()), getNumRows());
                    this->addRow(static_cast<int>(linkindex.size()),&linkindex[0],&linkvalues[0],0);
                    }
				++clusterid;
                }
        }catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addlinksrows", __LINE__, __FILE__);
			}
        }

	void FMToperatingareaclusterer::addnumberofclusterrows()
	{
		try {
			std::vector<int>centroidvariables;
			for (const FMToperatingareacluster& cluster : clusters)
				{
				centroidvariables.push_back(cluster.getcentroid().getvariable());
				}
			std::vector<double>params(centroidvariables.size(), 1.0);
			if (maximalnumberofclusters != -1)
			{
				
				setrowname("MAXCLUSTER", getNumRows());
				this->addRow(static_cast<int>(centroidvariables.size()), &centroidvariables[0], &params[0],-COIN_DBL_MAX, maximalnumberofclusters);
			}
			if (minimalnumberofclusters != -1)
			{
				const int numberofpot = static_cast<int>(centroidvariables.size());
				if (minimalnumberofclusters>numberofpot)
					{
					_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
						"of minimal number of cluster " + std::to_string(numberofpot) +
						" min cluster(" + std::to_string(minimalnumberofclusters) + ")",
						"FMToperatingareaclusterer::addareaconstraints", __LINE__, __FILE__);
					}
				setrowname("MINCLUSTER", getNumRows());
				this->addRow(static_cast<int>(centroidvariables.size()), &centroidvariables[0], &params[0],minimalnumberofclusters, COIN_DBL_MAX);
			}

		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addnumberofclusterrows", __LINE__, __FILE__);
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
				setrowname("F" +std::string(binit->first), getNumRows());
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
			int clusterid = 0;
			for (const FMToperatingareacluster& cluster : clusters)
				{
                std::vector<double>maxvalues(1,cluster.getcentroid().getarea());
                std::vector<int>maxindexes(1,cluster.getcentroid().getvariable());
				const double rest = cluster.getrealminimalarea() - cluster.getcentroid().getarea();
                std::vector<double>minvalues(1,-rest);
                std::vector<int>minindexes(1,cluster.getcentroid().getvariable());
                for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
                    {
                    maxvalues.push_back(binary.getarea());
                    maxindexes.push_back(binary.getvariable());
                    minvalues.push_back(binary.getarea());
                    minindexes.push_back(binary.getvariable());
                    }
                if(cluster.getrealminimalarea()>0)
                    {
					const double potentialarea = cluster.gettotalpotentialarea();
					if (potentialarea< cluster.getrealminimalarea())
						{
						_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
							"at value of "+std::to_string(potentialarea)+" cluster centroid " +
							std::string(cluster.getcentroid().getmask()) + 
							" min area("+std::to_string(cluster.getrealminimalarea())+")",
							"FMToperatingareaclusterer::addareaconstraints", __LINE__, __FILE__);
						}
					setrowname("MINA" + std::to_string(clusterid), getNumRows());
                    this->addRow(static_cast<int>(minvalues.size()),&minindexes[0],&minvalues[0],0);
                    }
                if (cluster.getrealmaximalarea()>0)
                    {
					double minimalbinarea = cluster.getcentroid().getarea();
					for (const FMToperatingareaclusterbinary& binary : cluster.getbinaries())
						{
							if (binary.getarea() < minimalbinarea)
							{
								minimalbinarea = binary.getarea();
							}
						}
					if (minimalbinarea > cluster.getrealmaximalarea())
							{
								_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
									"at value of " + std::to_string(minimalbinarea) + " cluster centroid " +
									std::string(cluster.getcentroid().getmask()) +
									" max area(" + std::to_string(cluster.getrealmaximalarea()) + ")",
									"FMToperatingareaclusterer::addareaconstraints", __LINE__, __FILE__);
							}
					setrowname("MAXA" + std::to_string(clusterid), getNumRows());
                    this->addRow(static_cast<int>(maxvalues.size()),&maxindexes[0],&maxvalues[0],-COIN_DBL_MAX,cluster.getrealmaximalarea());
                    }
				++clusterid;
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareaclusterer::addareaconstraints", __LINE__, __FILE__);
			}
		}

    void FMToperatingareaclusterer::branchnboundsolve()
        {
        try{
			this->setallinteger();
			if (this->isProvenOptimal())//If you got a solution push it
				{	
				//In that order it seems to work...
				this->branchAndBound();
				}
            this->unboundall();
			//this->writeLP("C:/Users/cyrgu3/Desktop/test/integer");
            this->branchAndBound();
			if (this->isProvenOptimal())
				{
				_logger->logwithlevel<std::string>("Optimal solution found\nObj(" +
					std::to_string(this->getObjValue()) + ")\n", 0);
				}
        }catch(...)
            {
            _exhandler->printexceptions("", "FMToperatingareaclusterer::branchnboundsolve", __LINE__, __FILE__);
            }
        }

   void FMToperatingareaclusterer::buildproblem()
        {
        try{
			this->buildclustersvariables();
            this->addobjective();
            this->addlinksrows();
            this->addforcingrows();
            this->addareaconstraints();
			this->addnumberofclusterrows();
			this->updaterowsandcolsnames(false);
			//this->writeLP("C:/Users/cyrgu3/Desktop/test/all");
         }catch(...)
            {
            _exhandler->raisefromcatch("", "FMToperatingareaclusterer::buildproblem", __LINE__, __FILE__);
            }
        }

}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMToperatingareaclusterer)
#endif
