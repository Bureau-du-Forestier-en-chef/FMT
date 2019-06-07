#ifndef FMTCON_H_INCLUDED
#define FMTCON_H_INCLUDED

#include <boost/unordered_map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
using namespace std;

namespace Core
{

class FMTconstants
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(data);
		}
    boost::unordered_map<string,vector<string>>data;
    public:
        FMTconstants();
    FMTconstants(const FMTconstants& rhs);
    void set(string key,vector<string>values);
    string getstr(string key,int period = 0) const;
    template<typename T>
    T get(string key,int period = 0) const
        {
        T value;
        value = T(stod(getstr(key,period)));
        return value;
        }
    template<typename T>
    vector<T>getall(string key) const
        {
        vector<T>values;
        size_t klength = length(key);
        for (size_t period = 0 ; period < klength; ++period)
            {
            values.push_back(T(stod(getstr(key,period))));
            }
        return values;
        }
    bool isconstant(string value) const;
    size_t length(string value) const;

    FMTconstants& operator = (const FMTconstants& rhs);
    operator string() const;
    };
}

#endif // FMTCON_H_INCLUDED
