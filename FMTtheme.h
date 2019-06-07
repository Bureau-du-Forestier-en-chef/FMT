#ifndef FMTtheme_H_INCLUDED
#define FMTtheme_H_INCLUDED

#include <boost/serialization/serialization.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/unordered_map.hpp>
#include <string>
#include <vector>
#include <map>
#include <boost/serialization/nvp.hpp>

#include "FMTexception.h"
#include "FMTmask.h"

using namespace boost::python;
using namespace std;
using namespace Exception;

namespace Core
{
class FMTtheme
    {

    friend class FMTmask;
    protected:
        int id,start;
    private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(id);
			ar & BOOST_SERIALIZATION_NVP(start);
			ar & BOOST_SERIALIZATION_NVP(aggregates);
			ar & BOOST_SERIALIZATION_NVP(source_aggregates);
			ar & BOOST_SERIALIZATION_NVP(valuenames);
			ar & BOOST_SERIALIZATION_NVP(name);
		}
        map<string,vector<string>>aggregates;
		map<string, vector<string>>source_aggregates;
        map<string,string>valuenames;
        map<string,map<string,double>>indexes;
		map<string,vector<string>>desagregate(map<string,vector<string>>aggregates,vector<string>&bases);
    public:
        string name;
        FMTtheme();
        FMTtheme(map<string,vector<string>>&laggregates,map<string,string>&lvaluenames,int& lid,int& lstart,string& lname);
        FMTtheme(map<string,vector<string>>&laggregates,map<string,string>&lvaluenames,
                 const map<string,map<string,double>>& indexing,int& lid,int& lstart,string& lname);
		FMTtheme(const FMTtheme& rhs);
		FMTtheme& operator = (const FMTtheme& rhs);
        FMTtheme(const vector<string> lvaluenames,const int& lid,const int& lstart,const string& lname);
        bool usefull();
        bool isattribute(const string& value) const;
        bool isindex(const string& value) const;
        bool useindex() const;
        double getindex(const string& attribute,const string& value) const;
        bool isaggregate(const string& value) const;
        bool inaggregate(const string& value,const string& aggregate);
		bool operator == (const FMTtheme& rhs) const;
        size_t size() const;
		int getid() const;
		bool empty() const;
        map<string,string>getvaluenames() const;
        boost::dynamic_bitset<> strtobits(const string& value) const;
        string bitstostr(boost::dynamic_bitset<>& bits) const;
        vector<string>getattributes(const string& value, bool aggregate_source = false) const;
        operator string() const;
    };
}
#endif // FMTtheme_H_INCLUDED
