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

#ifndef FMTFORK_H_INCLUDED
#define FMTFORK_H_INCLUDED

#include "FMTtransitionmask.h"
#include "FMTbounds.h"
#include "FMTdevelopment.h"
#include "FMTdevelopmentpath.h"
#include "FMTyields.h"
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace Core
{

class FMTdevelopment;
class FMTtransitionmask;
class FMTdevelopmentpath;

class FMTfork : public FMTspec
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("specifications", boost::serialization::base_object<FMTspec>(*this));
		ar & BOOST_SERIALIZATION_NVP(transitions);
		}
	std::vector<FMTtransitionmask>transitions;
    public:
        FMTfork();
        FMTfork(const FMTfork& rhs);
        FMTfork& operator = (const FMTfork& rhs);
        void add(FMTtransitionmask& transition);
		std::vector<FMTdevelopmentpath> getpaths(const FMTdevelopment& base,
			const FMTyields& ylds, const std::vector<FMTtheme>& themes,const bool& reset_age) const;
        FMTdevelopment getmax(const FMTdevelopment& base,const FMTyields& ylds,const std::vector<FMTtheme>& themes, const bool& reset_age) const;
		std::vector<FMTtransitionmask> getmasktrans() const;
		bool operator == (const FMTfork& rhs) const;
        FMTfork single() const;
        double sumprop() const;
        size_t size() const;
		FMTfork presolve(const FMTmask& selectedmask, const std::vector<FMTtheme>&presolvedthemes) const;
        operator std::string() const override;
    };

}

#endif // FMTFORK_H_INCLUDED
