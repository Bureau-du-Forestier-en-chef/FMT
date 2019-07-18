#ifndef FMTSPACT_H_INCLUDED
#define FMTSPACT_H_INCLUDED

#include "FMTaction.h"
#include "FMTconstraint.h"
using namespace Core;

namespace Spatial

{

    class FMTspatialaction : public FMTaction
        {
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("action", boost::serialization::base_object<FMTaction>(*this));
			ar & BOOST_SERIALIZATION_NVP(neighbors);
			ar & BOOST_SERIALIZATION_NVP(green_up);
			ar & BOOST_SERIALIZATION_NVP(adjacency);
			ar & BOOST_SERIALIZATION_NVP(minimal_size);
			ar & BOOST_SERIALIZATION_NVP(maximal_size);
			ar & BOOST_SERIALIZATION_NVP(neighbors_size);
		}
        public:
            vector<string>neighbors;
            size_t green_up;
            size_t adjacency;
            size_t minimal_size;
            size_t maximal_size;
            size_t neighbors_size;
            double greenup_weight;
            double adjacency_weight;
            double size_weight;
            FMTspatialaction();
            FMTspatialaction(const FMTaction& action,
                     const vector<string>& lneighbors,
                     const size_t& lgreen_up,
                     const size_t& ladjacency,
                     const size_t& lminimal_size,
                     const size_t& lmaximal_size,
                     const size_t& lneighbors_size);
            FMTspatialaction(const FMTaction& action,
                     const vector<string>& lneighbors,
                     const size_t& lgreen_up,
                     const size_t& ladjacency,
                     const size_t& lminimal_size,
                     const size_t& lmaximal_size,
                     const size_t& lneighbors_size,
                     const double& lgreenup_weight,
                     const double& ladjacency_weight,
                     const double& lsize_weight);
            FMTspatialaction(const FMTaction& action);
            FMTspatialaction(const FMTspatialaction& rhs);
            FMTspatialaction& operator = (const FMTspatialaction& rhs);
            bool simulated_annealing_valid()const;
            vector<FMTconstraint> to_constraints() const;
        };
}



#endif // FMTSPACT_H_INCLUDED
