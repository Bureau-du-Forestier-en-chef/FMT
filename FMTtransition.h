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

#ifndef FMTTRA_H_INCLUDED
#define FMTTRA_H_INCLUDED

#include "FMTfork.h"
#include "FMTmask.h"
#include "FMTlist.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTdevelopment.h"
#include <boost/serialization/serialization.hpp>

#include <vector>

using namespace std;

namespace Core
{
class FMTfork;

class FMTtransition : public FMTlist<FMTfork>
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("data", boost::serialization::base_object<FMTlist<FMTfork>>(*this));
		ar & BOOST_SERIALIZATION_NVP(name);
	}
    public:
        string name;
        FMTtransition();
        FMTtransition(string& lname);
		FMTtransition(const string& lname);
        //FMTtransition(const string& lname,map<FMTmask,vector<FMTfork>>& mapping);
        FMTtransition(const FMTtransition& rhs);
        FMTtransition& operator = (const FMTtransition& rhs);
        operator string() const;
        FMTtransition single() const;
		bool isleaking() const;
        unsigned int age_after(const vector<FMTdevelopment>& devs,
                               const FMTaction& action,
                               const FMTyields& ylds,
                               const vector<FMTtheme>& themes) const;
        const FMTfork* getfork(const FMTdevelopment& dev,
                               const FMTyields& ylds) const;
		vector<FMTtheme>getstaticthemes(const vector<FMTtheme>& themes) const;
        FMTmask main_target(const vector<FMTdevelopment>& devs,
                            const FMTyields& ylds) const;
		map<string,vector<FMTdevelopment>> attribute_targets(const vector<FMTdevelopment>& devs,
											const FMTyields& ylds, const vector<FMTtheme>& themes) const;
        bool operator < (const FMTtransition& rhs) const;
        bool operator == (const FMTtransition& rhs) const;
        bool operator != (const FMTtransition& rhs) const;
    };

class FMTtransitioncomparator
{
	string transition_name;
public:
	FMTtransitioncomparator(string name);
	bool operator()(const FMTtransition& transition) const;

};

}

#endif // FMTTRA_H_INCLUDED
