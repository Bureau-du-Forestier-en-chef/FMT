/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlpheuristic.hpp"
#include "FMToperatingareacluster.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <vector>

#ifdef FMTWITHOSI

#ifndef FMToperatingareaclusterer_H_INCLUDED
#define FMToperatingareaclusterer_H_INCLUDED
namespace Heuristics
{
// DocString: FMToperatingareaclusterer
/**
FMToperatingareaclusterer is made to do spatial clustering of operatingarea across a defined landscape from
a shapefile. You can define the minimal and maximal area of the resulting clusters and minimize the heteroginity
of a given statistic output of the whole clustered map. The initialsolve will attend to find a good initialsolution
then the user could try to get an optimal solution with the branchandbound().
*/
class FMTEXPORT FMToperatingareaclusterer : public FMTlpheuristic
	{
    friend class boost::serialization::access;
		template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTlpheuristic", boost::serialization::base_object<FMTlpheuristic>(*this));
			ar & BOOST_SERIALIZATION_NVP(clusters);
			ar & BOOST_SERIALIZATION_NVP(numberofsimulationpass);

		}
    // DocString: FMToperatingareaclusterer::clusters
    ///Operating area clusters enumeration containing informations for the lpformulation.
	std::vector<FMToperatingareacluster>clusters;
	// DocString: FMToperatingareaclusterer::numberofsimulationpass
    ///Number of tries made by the heuristic to find a feasible and/or a better solution.
	int numberofsimulationpass;
	// DocString: FMToperatingareaclusterer::minimalnumberofclusters
	///Minimal number of cluster to generate. If -1 none
	int minimalnumberofclusters;
	// DocString: FMToperatingareaclusterer::maximalnumberofclusters
	///Maximal number of cluster to generate. If -1 none
	int maximalnumberofclusters;
	// DocString: FMToperatingareaclusterer::totalarea
	///total area of the full map
	double totalarea;
        // DocString: FMToperatingareaclusterer::getallbinaries
		/**
		Returns all the binary decicions contained in the clusters data member, mapped by the mask.
		*/
        std::map<Core::FMTmask,std::vector<FMToperatingareaclusterbinary>>getallbinaries() const;
		// DocString: FMToperatingareaclusterer::gettotalarea()
		/**
		Get total area.
		*/
		double calculatetotalarea() const;
		// DocString: FMToperatingareaclusterer::gettotalarea
		/**
		Get total area.
		*/
		inline const double& gettotalarea() const
			{
			return totalarea;
			}
        // DocString: FMToperatingareaclusterer::addmaxminobjective
		/**
		This function adds objective rows to the lpmodel for a given cluster. Called by add objective.
		*/
        void addmaxminobjective(const FMToperatingareacluster& cluster, const FMToperatingareaclusterbinary& binary,
            const std::vector<FMToperatingareaclusterbinary>& binaries,
			const int& clusterid);
        // DocString: FMToperatingareaclusterer::buildclustersvariables
		/**
		First function to call to construct the lp formulation used by the heuristic, the function add and map variables for
		each operating area binaries.
		*/
        void buildclustersvariables();
         // DocString: FMToperatingareaclusterer::addobjective
		/**
		This function build the objective functions for the whole lp formulation. It will also generates rows in the matrix.
		The objective function is based on a minimization of the heterogenity found in the cluster.
		*/
        void addobjective();
         // DocString: FMToperatingareaclusterer::addlinksrows
		/**
		This function adds link rows when an OParea is selected to be part of a given cluster the opareas located between the
		cluster centroid and the selected OParea has to be also part of the oparea.
		*/
        void addlinksrows();
        // DocString: FMToperatingareaclusterer::addlinksrows
		/**
		This function adds rows to make sure that an OParea is part of only one OParea cluster. An Oparea cannot be part of
		two Clusters at the same time we force the solver to choose between those two.
		*/
        void addforcingrows();
		// DocString: FMToperatingareaclusterer::addnumberofclusterrows
		/**
		Add the minimal and maximal number of clusters.
		*/
		void addnumberofclusterrows();
        // DocString: FMToperatingareaclusterer::addareaconstraints
		/**
		This function adds minimal and maximal area constraints for each potential cluster.
		*/
        void addareaconstraints();
       
        // DocString: FMToperatingareaclusterer::setallinteger
		/**
		This functions sets the binary variable has integer, normaly used before sending the problem to the branchandbound solve.
		*/
        void setallinteger();
        // DocString: FMToperatingareaclusterer::getspreadprobability
		/**
		For a given potential (target) binarycluster and all the binarycluster present in the actual cluster (incluster), it calculates the probability
        of the fire to spread to a binary present in the cluster.
		*/
        double getspreadprobability(const std::vector<FMToperatingareaclusterbinary>& incluster,const FMToperatingareaclusterbinary& target) const;
        // DocString: FMToperatingareaclusterer::spread
		/**
		For a (ignition) binary this function makes spread the fire to its neighbors and update the (assigned) binaries vector. If the generated fire patch
		is between the area bounds for the cluster it will return true else false and the problem will be ready to be optimized.
		*/
        bool spread(const FMToperatingareacluster& ignition,std::vector<FMToperatingareaclusterbinary>& assigned);
         // DocString: FMToperatingareaclusterer::gettargetedoperatingareasize
		/**
		For a given cluster (target) this function generates a random targeted cluster size within the minimal and maximal area of the cluster.
		*/
        double gettargetedoperatingareasize(const FMToperatingareacluster& target);
		// DocString: FMToperatingareaclusterer::isvalidarea
		/**
		Check if the area calculated is valid.
		*/
		bool isvalidarea(const FMToperatingareacluster& cluster,
			const double& area, const size_t& actives) const;
         // DocString: FMToperatingareaclusterer::unboundall
		/**
		This function release the bounds set by the heuristic on the binaries decision variables for the whole model.
		*/
        void unboundall();
         // DocString: FMToperatingareaclusterer::getbinariesvariables
		/**
		This function returns the Lp problem variables of the cluster binaries of the whole problem.
		*/
        std::vector<int>getbinariesvariables() const;
	public:
	    // DocString: FMToperatingareaclusterer::FMToperatingareaclusterer()
		/**
		Default constructor for FMToperatingareaclusterer.
		*/
		FMToperatingareaclusterer() = default;
		// DocString: FMToperatingareaclusterer::FMToperatingareaclusterer(const Models::FMTsolverinterface&,const size_t&,const std::vector<FMToperatingareacluster>&)
		/**
		Constructor for FMToperatingareaclusterer taking a solver (interfacetype) a seed (lseed) an a vector of (clusters) to aggregate.
		*/
		FMToperatingareaclusterer(const Models::FMTsolverinterface& interfacetype,
			const size_t& lseed,const std::vector<FMToperatingareacluster>& lclusters,
			int minimalnumberofclusters = -1,int maximalnumberofclusters = -1);
		 // DocString: FMToperatingareaclusterer::FMToperatingareaclusterer(const FMToperatingareaclusterer&)
		/**
		Default copy constructor for FMToperatingareaclusterer.
		*/
		FMToperatingareaclusterer(const FMToperatingareaclusterer&);
		 // DocString: FMToperatingareaclusterer::operator =
		/**
		Default copy assignement for FMToperatingareaclusterer.
		*/
		FMToperatingareaclusterer& operator = (const FMToperatingareaclusterer&) = default;
		 // DocString: FMToperatingareaclusterer::setnumberofsimulationpass
		/**
		Sets the number of pass that will be made in the heuristics. It's the number of try made to get a better objective and/or a feasible solution
        value during the initialsolve phase.
		*/
		void setnumberofsimulationpass(const int& pass);
		 // DocString: FMToperatingareaclusterer::~FMToperatingareaclusterer()
		/**
		Default destructor for FMToperatingareaclusterer.
		*/
		~FMToperatingareaclusterer() = default;
		// DocString: FMToperatingareaclusterer::getbinariescount
		/**
		Get number of binaries in the whole problem.
		*/
		size_t getbinariescount() const;
		// DocString: FMToperatingareaclusterer::initialsolve
		/**
		Solve the heuristic problem using the original heuristic resolving the problem till finding a initial solution
		for each operating area.The fire spread heuristic will try to get a good initial clustering solution.
		*/
		bool initialsolve() final;
		// DocString: FMToperatingareaclusterer::branchnboundsolve
		/**
		Solve problem using Branch and bound on the primal formulation. If the function is called after a call to initialsolve()
		it's going to use the heuristic solution has a starting MIP solution, if not it's going to directly use the BnB on the formulated problem.
		*/
		bool branchnboundsolve() final;
		 // DocString: FMToperatingareaclusterer::getsolution
		/**
		Returns the clustering solution with cleaned operatingclusters, the user can then use the solution.
		*/
		std::vector<FMToperatingareacluster>getsolution() const;
		// DocString: FMToperatingareaclusterer::buildproblem
	   /**
	   This function build the whole problem by calling in this order buildclustersvariable,addobjective,addlinksrows,addforcingrows,addareaconstraints.
	   to generate a complete formulation for minimizing the heterogenity.
	   */
		void buildproblem();

	};
}

BOOST_CLASS_EXPORT_KEY(Heuristics::FMToperatingareaclusterer)


#endif
#endif
