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

#ifndef FMTYLDDATA_H_INCLUDED
#define FMTYLDDATA_H_INCLUDED

#include <string>
#include <map>
#include <algorithm>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTdata.h"
#include "FMTmask.h"
#include <vector>

namespace Core
{

class FMTyieldhandler
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{

		ar & BOOST_SERIALIZATION_NVP(yldtype);
		ar & BOOST_SERIALIZATION_NVP(mask);
		ar & BOOST_SERIALIZATION_NVP(bases);
		ar & BOOST_SERIALIZATION_NVP(elements);
	}
        FMTyldwstype yldtype;
        FMTmask mask;
		std::vector<int>bases;

    public:
		std::map<std::string,FMTdata>elements;
        operator std::string() const;
		FMTyieldhandler();
        FMTyieldhandler(FMTyldwstype ltype,const FMTmask& lmask);
        FMTyieldhandler(const FMTyieldhandler& rhs);
        FMTyieldhandler& operator = (const FMTyieldhandler& rhs);
		std::vector<std::string> compare(const std::vector<std::string>& keys) const;
        bool push_base(const int& base);
        bool push_data(const std::string& yld,const double& value);
        bool push_data(const std::string& yld,const FMTdata& data);
		std::vector<std::string> indexes(const std::vector<std::string>& names) const;
		double get(const std::vector<const FMTyieldhandler*>& datas,
                   const std::string yld,const int& age,const int& period, const FMTmask& resume_mask) const;
		int getlastbase() const;
		const std::vector<int>& getbases() const;
		double getlinearvalue(const std::vector<double>& dls, const int& agetarget) const;
		double getlastvalue(const std::string yld) const;
		std::map<std::string,double> getsources(const std::map<std::string, const FMTyieldhandler*>& srcdata, const std::vector<const FMTyieldhandler*>& datas,
										const int& age, const int& period ,const FMTmask& resume_mask, bool& age_only) const;
		std::map<std::string,const FMTyieldhandler*> getdata(const std::vector<const FMTyieldhandler*>& datas,
                                    const std::vector<std::string>& names, const std::string& original) const;
		bool operator == (const FMTyieldhandler& rhs) const;
		double getchangesfrom(const int& targetage,const int& peakstep) const;
		int getendpoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const;
		double getpeakfrom(const std::string& yld,double maxvalue = std::numeric_limits<double>::lowest()) const;
        double getpeak(const std::string& yld, const int& targetage) const;
        int getage(const std::string yld, const double& value,const int& starting_age) const;
		std::map<std::string,FMTdata>getdataelements() const;
        FMTyldwstype gettype() const;
		FMTmask getmask() const;
    };
}
#endif // FMTYLDDATA_H_INCLUDED
