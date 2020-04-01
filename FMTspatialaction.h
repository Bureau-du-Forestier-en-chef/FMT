/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef FMTSPACT_H_INCLUDED
#define FMTSPACT_H_INCLUDED

#include "FMTaction.h"
#include "FMTconstraint.h"

namespace Spatial

{

    class FMTspatialaction : public Core::FMTaction
        {
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("action", boost::serialization::base_object<Core::FMTaction>(*this));
			ar & BOOST_SERIALIZATION_NVP(neighbors);
			ar & BOOST_SERIALIZATION_NVP(green_up);
			ar & BOOST_SERIALIZATION_NVP(adjacency);
			ar & BOOST_SERIALIZATION_NVP(minimal_size);
			ar & BOOST_SERIALIZATION_NVP(maximal_size);
			ar & BOOST_SERIALIZATION_NVP(neighbors_size);
		}
        public:
			std::vector<std::string>neighbors;
            size_t green_up;
            size_t adjacency;
            size_t minimal_size;
            size_t maximal_size;
            size_t neighbors_size;
            double greenup_weight;
            double adjacency_weight;
            double size_weight;
            FMTspatialaction();
            FMTspatialaction(const Core::FMTaction& action,
                     const std::vector<std::string>& lneighbors,
                     const size_t& lgreen_up,
                     const size_t& ladjacency,
                     const size_t& lminimal_size,
                     const size_t& lmaximal_size,
                     const size_t& lneighbors_size);
            FMTspatialaction(const Core::FMTaction& action,
                     const std::vector<std::string>& lneighbors,
                     const size_t& lgreen_up,
                     const size_t& ladjacency,
                     const size_t& lminimal_size,
                     const size_t& lmaximal_size,
                     const size_t& lneighbors_size,
                     const double& lgreenup_weight,
                     const double& ladjacency_weight,
                     const double& lsize_weight);
            FMTspatialaction(const Core::FMTaction& action);
            FMTspatialaction(const FMTspatialaction& rhs);
            FMTspatialaction& operator = (const FMTspatialaction& rhs);
            bool simulated_annealing_valid()const;
			std::vector<Core::FMTconstraint> to_constraints() const;
        };
}



#endif // FMTSPACT_H_INCLUDED
