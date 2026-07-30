/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#include "FMTOperatingAreaClusterer.h"
#include <random>
#include "FMTExceptionHandler.h"



namespace Heuristics
{
	FMTOperatingAreaClusterer::FMTOperatingAreaClusterer(const Models::FMTsolverinterface& interfacetype,const size_t& lseed,
		const std::vector<FMTOperatingAreaCluster>& lclusters,int p_minimalnumberofclusters, int p_maximalnumberofclusters):
	    FMTLpHeuristic(interfacetype,lseed),
		m_clusters(lclusters),
		m_numberofsimulationpass(100),
		m_minimalnumberofclusters(p_minimalnumberofclusters),
		m_maximalnumberofclusters(p_maximalnumberofclusters),
		m_totalarea(_calculateTotalArea())
	{
		
	}

	FMTOperatingAreaClusterer::FMTOperatingAreaClusterer(const FMTOperatingAreaClusterer& rhs):
		FMTLpHeuristic(rhs),
		m_clusters(rhs.m_clusters),
		m_numberofsimulationpass(rhs.m_numberofsimulationpass),
		m_minimalnumberofclusters(rhs.m_minimalnumberofclusters),
		m_maximalnumberofclusters(rhs.m_maximalnumberofclusters),
		m_totalarea(rhs.m_totalarea)
	{

	}

	void FMTOperatingAreaClusterer::_setAllInteger()
		{
		try {
		    const std::vector<int>integervariables = this->_getBinariesVariables();
            this->setInteger(&integervariables[0], static_cast<int>(integervariables.size()));
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_setAllInteger", __LINE__, __FILE__);
			}
		}

    void FMTOperatingAreaClusterer::_unboundAll()
		{
		try {
		    const std::vector<int>integervariables = this->_getBinariesVariables();
		    std::vector<double>colsbounds;
		    for (const int& variable : integervariables)
                {
                colsbounds.push_back(0);
                colsbounds.push_back(1);
                }
            this->setColSetBounds(&integervariables[0],&integervariables.back() + 1,&colsbounds[0]);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_unboundAll", __LINE__, __FILE__);
			}
		}

    double FMTOperatingAreaClusterer::_getSpreadProbability(const std::vector<FMTOperatingAreaClusterBinary>& incluster,const FMTOperatingAreaClusterBinary& target) const
        {
        double maxdifference = 0;
        try {
        for (const FMTOperatingAreaClusterBinary& binary : incluster)
            {
            const double difference = std::abs(binary.getStatistic()-target.getStatistic());
            if (difference > maxdifference)
                {
                maxdifference = difference;
                }

            }
        }catch(...)
            {
            _exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_getSpreadProbability", __LINE__, __FILE__);
            }
        return (1-maxdifference);
        }
	bool FMTOperatingAreaClusterer::_isValidarea(const FMTOperatingAreaCluster& cluster, const double& area, const size_t& actives) const
	{
		try {
			if (cluster.isValidArea(area))
			{
				if (m_maximalnumberofclusters>0)
				{
					const double totalarea = _getTotalArea();
					const double minimal = std::max((totalarea / m_maximalnumberofclusters), cluster.getMinimalArea());
					return (area >= minimal || (area < minimal && actives==0));
				}else {
					return true;
				}
				
			}

		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_getTargetedOperatingAreaSize", __LINE__, __FILE__);
			}
		return false;
	}
   double FMTOperatingAreaClusterer::_getTargetedOperatingAreaSize(const FMTOperatingAreaCluster& target)
        {
        double returnedsize = 0;
        try{
            double minimaltarget = target.getMinimalArea();
            double maximaltarget = target.getMaximalArea();
			if (m_minimalnumberofclusters>0)
				{
				minimaltarget = std::max(_getTotalArea()/ m_minimalnumberofclusters, minimaltarget);
				}
            std::uniform_real_distribution<double>areadistribution(minimaltarget,maximaltarget);
            returnedsize = areadistribution(m_generator);
        }catch(...)
            {
            _exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_getTargetedOperatingAreaSize", __LINE__, __FILE__);
            }
        return returnedsize;
        }

    bool FMTOperatingAreaClusterer::_spread(const FMTOperatingAreaCluster& ignition,std::vector<FMTOperatingAreaClusterBinary>& assigned)
        {
        try{
			if (std::find_if(assigned.begin(), assigned.end(), FMTOperatingAreaComparator(ignition.getCentroid())) != assigned.end())
				{
				return true;
				}
            double firesize = ignition.getCentroid().getArea();
            std::vector<FMTOperatingAreaClusterBinary>incluster(1,ignition.getCentroid());
            std::vector<FMTOperatingAreaClusterBinary>outcluster;
            std::vector<FMTOperatingAreaClusterBinary>actives;
			
            for (const FMTOperatingAreaClusterBinary& binary : ignition.getBinaries())
                {
                if (std::find_if(assigned.begin(),assigned.end(),FMTOperatingAreaComparator(binary))==assigned.end())
                    {
                    if (binary.getNeighbors().empty())
                        {
                        actives.push_back(binary);
                    }else{
                        outcluster.push_back(binary);
                        }
                    }
                }
            const double maximalfiresize = this->_getTargetedOperatingAreaSize(ignition);
			while((!actives.empty()|| _isValidarea(ignition,firesize, actives.size())) && firesize <= maximalfiresize)
                {
				//*_logger << "fire size " << firesize << " " << maximalfiresize << "\n";
				if (!_isValidarea(ignition,firesize,actives.size()) && !actives.empty())
					{
						std::vector<double>probabilities;
						double totaldifference = 0;
						for (const FMTOperatingAreaClusterBinary& active : actives)
							{
							const double difference = _getSpreadProbability(incluster,active);
							totaldifference+=difference;
							probabilities.push_back(difference);
							}
						std::vector<int>intprobability;
						for(const double& probability : probabilities)
							{
							intprobability.push_back(static_cast<int>(probability/totaldifference)*100);
							}
						std::discrete_distribution<int>spreaddistribution(intprobability.begin(),intprobability.end());
						const int selection = spreaddistribution(m_generator);
						//*_logger << "active size " << actives.size() << "\n";
						const FMTOperatingAreaClusterBinary selected = actives.at(selection);
						actives.erase(actives.begin()+selection);
						incluster.push_back(selected);
						firesize+=selected.getArea();
						std::vector<FMTOperatingAreaClusterBinary>updatedoutcluster;
						for (const FMTOperatingAreaClusterBinary& outbinary : outcluster)
							{
							const std::vector<Core::FMTMask>neighbors = outbinary.getNeighbors();
							if (std::find_if(neighbors.begin(),neighbors.end(),Core::FMTMaskComparator(selected.getMask()))!=neighbors.end())
								{
									int fullsize = static_cast<int>(neighbors.size());
									for (const FMTOperatingAreaClusterBinary& inbinary : incluster)
										{
										if (std::find_if(neighbors.begin(),neighbors.end(),Core::FMTMaskComparator(inbinary.getMask()))!=neighbors.end())
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
                  
                    if (_isValidarea(ignition,firesize, actives.size()))
                        {
						//*_logger << "valid! fire size " << firesize << " " << maximalfiresize << "\n";
						std::vector<double>bounds;
						std::vector<int>indexes;
                        for (const FMTOperatingAreaClusterBinary& inbinary : incluster)
                            {
                            assigned.push_back(inbinary);
                            bounds.push_back(1.0);
                            bounds.push_back(1.0);
                            indexes.push_back(inbinary.getVariable());
                            }
                        this->setColSetBounds(&indexes[0],&indexes.back() + 1,&bounds[0]);
                        return true;
                        }
                    }
			//*_logger << "out! "<< isValidArea(ignition, firesize, actives.size()) << "\n";
        }catch(...)
            {
            _exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_spread", __LINE__, __FILE__);
            }
        return false;
        }

    void FMTOperatingAreaClusterer::setNumberOfSimulationPass(const int& pass)
        {
        m_numberofsimulationpass = pass;
        }

    std::vector<int>FMTOperatingAreaClusterer::_getBinariesVariables() const
        {
        std::vector<int>varindexes;
        for (const FMTOperatingAreaCluster& cluster : m_clusters)
            {
            varindexes.push_back(cluster.getCentroid().getVariable());
            for (const FMTOperatingAreaClusterBinary& binary : cluster.getBinaries())
                {
                varindexes.push_back(binary.getVariable());
                }
            }
        return varindexes;
        }

	size_t FMTOperatingAreaClusterer::getBinariesCount() const
		{
		size_t returnsize = 0;
		try {
			returnsize = _getAllBinaries().size();
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTOperatingAreaClusterer::getBinariesCount", __LINE__, __FILE__);
			}
		return returnsize;
		}

    bool FMTOperatingAreaClusterer::initialSolve()
        {
        try{
            double passleft =  m_numberofsimulationpass;
			bool gotonesolution = false;
            if (Models::FMTLpSolver::initialSolve())
                {
                double bestobjectivevalue = 0;
                std::vector<double>bestcolsbound;
                const std::vector<int>varindexes = this->_getBinariesVariables();
				size_t iteration = 0;
                while(passleft>0)
                    {
                    this->_unboundAll();
                    std::vector<FMTOperatingAreaClusterBinary>assigned;
                    if (Models::FMTLpSolver::resolve())
                        {
                        std::vector<FMTOperatingAreaCluster>clustertospread=m_clusters;
                        std::shuffle(clustertospread.begin(),clustertospread.end(),m_generator);
                        size_t iterationdone = 0;
                        while(!clustertospread.empty() && iterationdone < (m_clusters.size()*2))
                            {
                            if (this->_spread(clustertospread.back()/**clustertospread.begin()*/,assigned))
                                {
								clustertospread.pop_back();
                                //clustertospread.erase(clustertospread.begin());
                                }
                            ++iterationdone;
                            }
                        if (clustertospread.empty() && Models::FMTLpSolver::resolve() &&
                            (!gotonesolution||(Models::FMTLpSolver::getObjValue() < bestobjectivevalue)))
                            {
                            bestcolsbound.clear();
                            const double* upperbound = Models::FMTLpSolver::getColUpper();
                            const double* lowerbound = Models::FMTLpSolver::getColLower();
                            for (const FMTOperatingAreaCluster& cluster : m_clusters)
                                {
                                const int centroidvar = cluster.getCentroid().getVariable();
                                bestcolsbound.push_back(*(lowerbound+centroidvar));
                                bestcolsbound.push_back(*(upperbound+centroidvar));
                                for (const FMTOperatingAreaClusterBinary& binary : cluster.getBinaries())
                                    {
                                    const int binaryvar = binary.getVariable();
                                    bestcolsbound.push_back(*(lowerbound+ binaryvar));
                                    bestcolsbound.push_back(*(upperbound+ binaryvar));
									
                                    }
                                }
                            bestobjectivevalue = Models::FMTLpSolver::getObjValue();
							if (!gotonesolution)
								{
								_logger->logWithLevel("Feasible solution found\n", 0);
								}
							_logger->logWithLevel("Obj(" +
								std::to_string(bestobjectivevalue) + ") it(" + std::to_string(iteration) + ")\n", 0);
							gotonesolution = true;
                            }
                        }
                    --passleft;
					++iteration;
                    }
                if (bestcolsbound.empty())
                    {
                    this->_unboundAll();
                    return false;
                    }else{
                        this->setColSetBounds(&varindexes[0],&varindexes.back() + 1,&bestcolsbound[0]);
					
					return Models::FMTLpSolver::resolve();
                    }
                }
            }catch(...)
                {
                _exhandler->printExceptions("", "FMTOperatingAreaClusterer::initialSolve", __LINE__, __FILE__);
                }
        return false;
        }

    std::vector<FMTOperatingAreaCluster>FMTOperatingAreaClusterer::getSolution() const
        {
        std::vector<FMTOperatingAreaCluster>solution;
        try{
            const double* primalsolution = Models::FMTLpSolver::getColSolution();
            for (const FMTOperatingAreaCluster& cluster : m_clusters)
                {
                if (*(primalsolution+cluster.getCentroid().getVariable())>0.5)//active cluster
                    {
                    std::vector<FMTOperatingAreaClusterBinary>selectedbinaries;
                    for (const FMTOperatingAreaClusterBinary& binary : cluster.getBinaries())
                        {
                        if (*(primalsolution+binary.getVariable())>0.5)//active binary
                            {
                            selectedbinaries.push_back(binary);
                            }
                        }
                    solution.push_back(FMTOperatingAreaCluster(FMTOperatingAreaCluster(cluster.getCentroid(),selectedbinaries),cluster.getRealMinimalArea(),cluster.getRealMaximalArea()));
                    }
                }
        }catch(...)
            {
            _exhandler->printExceptions("", "FMTOperatingAreaClusterer::getSolution", __LINE__, __FILE__);
            }
        return solution;
        }


	void FMTOperatingAreaClusterer::_buildClustersVariables()
		{
		try {
			std::vector<FMTOperatingAreaCluster>newclusterswithvariables;
			std::vector<int>variablesadded;
			int variableid = 0;
			int clusterid = 0;
			for (const FMTOperatingAreaCluster& cluster : m_clusters)
				{
				FMTOperatingAreaClusterBinary newcentroid = cluster.getCentroid();
				newcentroid.setVariable(variableid);
				this->addCol(0, nullptr, nullptr, 0.0, 1.0, 0.0);
				this->setColName("C_" + std::to_string(clusterid) + "_" + std::string(cluster.getCentroid().getMask()), variableid);
				variablesadded.push_back(variableid);
				++variableid;
				std::vector<FMTOperatingAreaClusterBinary>newbinaries = cluster.getBinaries();
				for (FMTOperatingAreaClusterBinary& neighbor : newbinaries)
					{
					neighbor.setVariable(variableid);
					this->setColName("B_" + std::to_string(clusterid) + "_" + std::string(neighbor.getMask()), variableid);
					variablesadded.push_back(variableid);
					this->addCol(0, nullptr, nullptr, 0.0, 1.0, 0.0);
					++variableid;
					}
				newclusterswithvariables.push_back(FMTOperatingAreaCluster(FMTOperatingAreaCluster(newcentroid, newbinaries),cluster.getRealMinimalArea(),cluster.getRealMaximalArea()));
				++clusterid;
				}
			m_clusters = newclusterswithvariables;
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_buildClustersVariables", __LINE__, __FILE__);
			}

		}

	void FMTOperatingAreaClusterer::_addMaxMinObjective(const FMTOperatingAreaCluster& cluster, const FMTOperatingAreaClusterBinary& binary,
		const std::vector<FMTOperatingAreaClusterBinary>& binaries,
		const int& clusterid)
	{
		try {
			std::vector<double>maxvariables;
			maxvariables.push_back(binary.getStatistic());
			maxvariables.push_back(-1.0);
			std::vector<int>indexesofmaxvariables;
			indexesofmaxvariables.push_back(binary.getVariable());
			indexesofmaxvariables.push_back(cluster.getMaximalObjectiveVariable());
			setRowName("MAXO_" + std::to_string(clusterid) + "_" +std::string(binary.getMask()), getNumRows());
			this->addRow(2, &indexesofmaxvariables[0], &maxvariables[0], -std::numeric_limits<double>::max(),0);
			/*std::vector<double>baseminvariables;
			baseminvariables.push_back(-binary.getStatistic());
			baseminvariables.push_back(-1.0);
			std::vector<int>baseindexesofminvariables;
			baseindexesofminvariables.push_back(binary.getVariable());
			baseindexesofminvariables.push_back(cluster.getMinimalObjectiveVariable());
			this->addRow(2, &baseindexesofminvariables[0], &baseminvariables[0], -COIN_DBL_MAX, 0);*/

			
			std::vector<double>minvariables;
			std::vector<int>indexesofminvariables;
			const double maxincluster = cluster.getMaximalStats();
			minvariables.push_back(-binary.getStatistic());
			indexesofminvariables.push_back(binary.getVariable());
			for (const FMTOperatingAreaClusterBinary& localbinary : binaries)
				{
				if (binary.getVariable()!=localbinary.getVariable())
					{
					minvariables.push_back(-maxincluster);
					indexesofminvariables.push_back(localbinary.getVariable());
					}
				}
			minvariables.push_back(-1.0);
			indexesofminvariables.push_back(cluster.getMinimalObjectiveVariable());
			setRowName("MINO_" + std::to_string(clusterid) + "_" +std::string(binary.getMask()), getNumRows());
			this->addRow(static_cast<int>(indexesofminvariables.size()), &indexesofminvariables[0], &minvariables[0],-std::numeric_limits<double>::max(), 0.0);
		
		
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_addMaxMinObjective", __LINE__, __FILE__);
			}
		}

	double FMTOperatingAreaClusterer::_calculateTotalArea() const
	{
		double totalarea = 0;
		try {
			for (const auto& all : _getAllBinaries())
			{
				totalarea += (all.second.begin()->getArea());
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "MToperatingareaclusterer::_getTotalArea", __LINE__, __FILE__);
		}
		return totalarea;
	}

    std::map<Core::FMTMask,std::vector<FMTOperatingAreaClusterBinary>>FMTOperatingAreaClusterer::_getAllBinaries() const
        {
        std::map<Core::FMTMask,std::vector<FMTOperatingAreaClusterBinary>>allbinaries;
        try {
            for (const FMTOperatingAreaCluster& cluster : m_clusters)
                {
				const Core::FMTMask centroidmask = cluster.getCentroid().getMask();
                if (allbinaries.find(centroidmask)!=allbinaries.end())
                    {
                    allbinaries[centroidmask].push_back(cluster.getCentroid());
                    }else{
                    allbinaries[centroidmask]=std::vector<FMTOperatingAreaClusterBinary>(1,cluster.getCentroid());
                    }
                for (const FMTOperatingAreaClusterBinary& binary : cluster.getBinaries())
                    {
					const Core::FMTMask binarymask = binary.getMask();
                    if (allbinaries.find(binarymask)!=allbinaries.end())
                        {
                        allbinaries[binarymask].push_back(binary);
                        }else{
                        allbinaries[binarymask]=std::vector<FMTOperatingAreaClusterBinary>(1,binary);
                        }
                    }
                }
        }catch (...)
            {
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_getAllBinaries", __LINE__, __FILE__);
            }
        return allbinaries;
        }

	void FMTOperatingAreaClusterer::_addObjective()
		{
		try {
		    std::map<Core::FMTMask,std::vector<FMTOperatingAreaClusterBinary>>choices = this->_getAllBinaries();
			int clusterid = 0;
			for (FMTOperatingAreaCluster& cluster : m_clusters)
                {
				int columnid = this->getNumCols();
				cluster.setMaximalObjectiveVariable(columnid);
				this->addCol(0, nullptr, nullptr,0.0, std::numeric_limits<double>::max(), 1.0);
				setColName("MAX" + std::to_string(clusterid), columnid);
				++columnid;
				cluster.setMinimalObjectiveVariable(columnid);
				this->addCol(0, nullptr, nullptr,-std::numeric_limits<double>::max(),0, 1.0);
				setColName("MIN" + std::to_string(clusterid), columnid);
				std::vector<double>variableclosed;
				const double maxincluster = cluster.getMaximalStats();
				variableclosed.push_back(-maxincluster);
				variableclosed.push_back(-1);
				std::vector<int>columnidclosed;
				columnidclosed.push_back(cluster.getCentroid().getVariable());
				columnidclosed.push_back(cluster.getMinimalObjectiveVariable());
				setRowName("OF" + std::to_string(clusterid), getNumRows());
				this->addRow(2, &columnidclosed[0], &variableclosed[0], -std::numeric_limits<double>::max(),0.0);
				this->_addMaxMinObjective(cluster, cluster.getCentroid(), choices.at(cluster.getCentroid().getMask()),clusterid);
				for (const FMTOperatingAreaClusterBinary& binary : cluster.getBinaries())
					{
                    this->_addMaxMinObjective(cluster,binary,choices.at(binary.getMask()), clusterid);
					}

				/*std::vector<double>alllowerpar;
				std::vector<int>alllowervar;
				alllowervar.push_back(cluster.getCentroid().getVariable());
				alllowerpar.push_back(-cluster.getCentroid().getStatistic());
				for (const FMTOperatingAreaClusterBinary& binary : cluster.getBinaries())
					{
					alllowervar.push_back(binary.getVariable());
					alllowerpar.push_back(-binary.getStatistic());
					}
				alllowerpar.push_back(-1);
				alllowervar.push_back(cluster.getMinimalObjectiveVariable());
				this->addRow(static_cast<int>(alllowervar.size()), &alllowervar[0], &alllowerpar[0], -COIN_DBL_MAX, 0);*/
				++clusterid;
				}
			this->setObjSense(1.0);
		}catch (...)
            {
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_addObjective", __LINE__, __FILE__);
            }
		}

    void FMTOperatingAreaClusterer::_addLinksRows()
        {
        try {
			int clusterid = 0;
            for (const FMTOperatingAreaCluster& cluster : m_clusters)
                {
                const std::vector<FMTOperatingAreaClusterBinary>clusterbinaries = cluster.getBinaries();
                for (const FMTOperatingAreaClusterBinary& binary : clusterbinaries)
                    {
                    std::vector<double>linkvalues;
                    std::vector<int>linkindex;
                    linkvalues.push_back(-(static_cast<double>(binary.getNeighbors().size())+1));
                    linkindex.push_back(binary.getVariable());
                    linkvalues.push_back(1.0);
                    linkindex.push_back(cluster.getCentroid().getVariable());
                    for (const Core::FMTMask& neighbor : binary.getNeighbors())
                        {
                        std::vector<FMTOperatingAreaClusterBinary>::const_iterator neighborit = std::find_if(clusterbinaries.begin(),clusterbinaries.end(),FMTOperatingAreaComparator(neighbor));
                        linkvalues.push_back(1.0);
                        linkindex.push_back(neighborit->getVariable());
                        }
					setRowName("L" + std::to_string(clusterid) + "_" + std::string(binary.getMask()), getNumRows());
                    this->addRow(static_cast<int>(linkindex.size()),&linkindex[0],&linkvalues[0],0);
                    }
				++clusterid;
                }
        }catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_addLinksRows", __LINE__, __FILE__);
			}
        }

	void FMTOperatingAreaClusterer::_addNumberOfClusterRows()
	{
		try {
			std::vector<int>centroidvariables;
			for (const FMTOperatingAreaCluster& cluster : m_clusters)
				{
				centroidvariables.push_back(cluster.getCentroid().getVariable());
				}
			std::vector<double>params(centroidvariables.size(), 1.0);
			if (m_maximalnumberofclusters != -1)
			{
				
				setRowName("MAXCLUSTER", getNumRows());
				this->addRow(static_cast<int>(centroidvariables.size()), &centroidvariables[0], &params[0],-std::numeric_limits<double>::max(), m_maximalnumberofclusters);
			}
			if (m_minimalnumberofclusters != -1)
			{
				const int numberofpot = static_cast<int>(centroidvariables.size());
				if (m_minimalnumberofclusters>numberofpot)
					{
					_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
						"of minimal number of cluster " + std::to_string(numberofpot) +
						" min cluster(" + std::to_string(m_minimalnumberofclusters) + ")",
						"FMTOperatingAreaClusterer::_addAreaConstraints", __LINE__, __FILE__);
					}
				setRowName("MINCLUSTER", getNumRows());
				this->addRow(static_cast<int>(centroidvariables.size()), &centroidvariables[0], &params[0],m_minimalnumberofclusters, std::numeric_limits<double>::max());
			}

		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_addNumberOfClusterRows", __LINE__, __FILE__);
		}
	}

    void FMTOperatingAreaClusterer::_addForcingRows()
        {
        try {
		    std::map<Core::FMTMask,std::vector<FMTOperatingAreaClusterBinary>>choices = this->_getAllBinaries();
		    for (std::map<Core::FMTMask,std::vector<FMTOperatingAreaClusterBinary>>::const_iterator binit = choices.begin();binit!=choices.end();binit++)
                {
                std::vector<double>values;
                std::vector<int>indexes;
                for(const FMTOperatingAreaClusterBinary& neighbor : binit->second)
                    {
                    values.push_back(1.0);
                    indexes.push_back(neighbor.getVariable());
                    }
				setRowName("F" +std::string(binit->first), getNumRows());
                this->addRow(static_cast<int>(indexes.size()),&indexes[0],&values[0],1.0,1.0);
                }

		}catch (...)
            {
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_addForcingRows", __LINE__, __FILE__);
            }

        }


	void FMTOperatingAreaClusterer::_addAreaConstraints()
		{
		try {
			int clusterid = 0;
			for (const FMTOperatingAreaCluster& cluster : m_clusters)
				{
                std::vector<double>maxvalues(1,cluster.getCentroid().getArea());
                std::vector<int>maxindexes(1,cluster.getCentroid().getVariable());
				const double rest = cluster.getRealMinimalArea() - cluster.getCentroid().getArea();
                std::vector<double>minvalues(1,-rest);
                std::vector<int>minindexes(1,cluster.getCentroid().getVariable());
                for (const FMTOperatingAreaClusterBinary& binary : cluster.getBinaries())
                    {
                    maxvalues.push_back(binary.getArea());
                    maxindexes.push_back(binary.getVariable());
                    minvalues.push_back(binary.getArea());
                    minindexes.push_back(binary.getVariable());
                    }
                if(cluster.getRealMinimalArea()>0)
                    {
					const double potentialarea = cluster.getTotalPotentialArea();
					if (potentialarea< cluster.getRealMinimalArea())
						{
						_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
							"at value of "+std::to_string(potentialarea)+" cluster centroid " +
							std::string(cluster.getCentroid().getMask()) + 
							" min area("+std::to_string(cluster.getRealMinimalArea())+")",
							"FMTOperatingAreaClusterer::_addAreaConstraints", __LINE__, __FILE__);
						}
					setRowName("MINA" + std::to_string(clusterid), getNumRows());
                    this->addRow(static_cast<int>(minvalues.size()),&minindexes[0],&minvalues[0],0);
                    }
                if (cluster.getRealMaximalArea()>0)
                    {
					double minimalbinarea = cluster.getCentroid().getArea();
					for (const FMTOperatingAreaClusterBinary& binary : cluster.getBinaries())
						{
							if (binary.getArea() < minimalbinarea)
							{
								minimalbinarea = binary.getArea();
							}
						}
					if (minimalbinarea > cluster.getRealMaximalArea())
							{
								_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
									"at value of " + std::to_string(minimalbinarea) + " cluster centroid " +
									std::string(cluster.getCentroid().getMask()) +
									" max area(" + std::to_string(cluster.getRealMaximalArea()) + ")",
									"FMTOperatingAreaClusterer::_addAreaConstraints", __LINE__, __FILE__);
							}
					setRowName("MAXA" + std::to_string(clusterid), getNumRows());
                    this->addRow(static_cast<int>(maxvalues.size()),&maxindexes[0],&maxvalues[0],-std::numeric_limits<double>::max(),cluster.getRealMaximalArea());
                    }
				++clusterid;
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::_addAreaConstraints", __LINE__, __FILE__);
			}
		}

    bool FMTOperatingAreaClusterer::branchNBoundSolve()
        {
        try{
			this->_setAllInteger();
			if (this->isProvenOptimal())//If you got a solution push it
				{	
				//In that order it seems to work...
				this->branchAndBound();
				}
            this->_unboundAll();
			//this->writeLp("C:/Users/cyrgu3/Desktop/test/integer");
            this->branchAndBound();
			if (this->isProvenOptimal())
				{
				_logger->logWithLevel("Optimal solution found\nObj(" +
					std::to_string(this->getObjValue()) + ")\n", 0);
				}
        }catch(...)
            {
            _exhandler->printExceptions("", "FMTOperatingAreaClusterer::branchNBoundSolve", __LINE__, __FILE__);
            }
		return this->isProvenOptimal();
        }

   void FMTOperatingAreaClusterer::buildProblem()
        {
        try{
			this->_buildClustersVariables();
            this->_addObjective();
            this->_addLinksRows();
            this->_addForcingRows();
            this->_addAreaConstraints();
			this->_addNumberOfClusterRows();
			this->updateRowsAndColsNames(false);
         }catch(...)
            {
            _exhandler->raiseFromCatch("", "FMTOperatingAreaClusterer::buildProblem", __LINE__, __FILE__);
            }
        }

}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTOperatingAreaClusterer)
#endif
