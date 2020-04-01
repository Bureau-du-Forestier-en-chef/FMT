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

#ifndef FMTLIF_H_INCLUDED
#define FMTLIF_H_INCLUDED

#include "FMTlist.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
namespace Core
{

class FMTlifespans : public FMTlist<int>
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("data", boost::serialization::base_object<FMTlist<int>>(*this));
	}
    public:
        FMTlifespans();
        FMTlifespans(const FMTlifespans& rhs);
        FMTlifespans& operator = (const FMTlifespans& rhs);
		bool operator == (const FMTlifespans& rhs) const;
        operator std::string() const;
		void update() override;
		FMTlifespans presolve(const FMTmask& basemask,
			const std::vector<FMTtheme>& originalthemes,
			const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& newthemes) const;
    };

}
#endif // FMTLIF_H_INCLUDED
