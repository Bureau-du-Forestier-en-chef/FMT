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

#ifndef FMTMASKFILTER_H_INCLUDED
#define FMTMASKFILTER_H_INCLUDED

#include "FMTmask.h"
#include <boost/dynamic_bitset.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


namespace Core
{


class FMTmaskfilter
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(fullset);
		ar & BOOST_SERIALIZATION_NVP(selection);
		ar & BOOST_SERIALIZATION_NVP(flippedselection);
		ar & BOOST_SERIALIZATION_NVP(index);
		}
    protected:
		boost::dynamic_bitset<> fullset;
        boost::dynamic_bitset<> selection;
        boost::dynamic_bitset<> flippedselection;
		std::vector<int>index;
    public:
        FMTmaskfilter();
        virtual ~FMTmaskfilter()=default;
        FMTmaskfilter(const FMTmaskfilter& rhs);
        FMTmaskfilter& operator = (const FMTmaskfilter& rhs);
        FMTmaskfilter(std::vector<FMTmask>& masks);
        FMTmaskfilter(std::vector<FMTmask>& masks,const std::vector<FMTtheme>& themes);
        FMTmask filter(const FMTmask& devmask) const;
        bool within(const FMTmask& intersect) const;
		inline bool empty() const
			{
			return selection.empty();
			}
		inline size_t hash() const
			{
			return (boost::hash<boost::dynamic_bitset<>>()(selection) ^ boost::hash<boost::dynamic_bitset<>>()(flippedselection));
			}
    };



}

namespace boost {

	template <>
	struct hash<Core::FMTmaskfilter>
	{
		std::size_t operator()(const Core::FMTmaskfilter& filter) const
		{

			return (filter.hash());
		}
	};


}

#endif // FMTMASKFILTER_H_INCLUDED
