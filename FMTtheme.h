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

#ifndef FMTtheme_H_INCLUDED
#define FMTtheme_H_INCLUDED

#include <boost/serialization/serialization.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/unordered_map.hpp>
#include <string>
#include <vector>
#include <map>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>
#include "FMTexception.h"
#include "FMTmask.h"



namespace Core
{
/**
FMTtheme hold multiple attributes for only one theme. FMTtheme gives the description of a part of a FMTmask
from the start bit to the start + theme.size() bit. It also hold the information about attribute aggregates.
FMTtheme is realy close to FMTmask class.
*/
class FMTtheme
    {
    friend class FMTmask;
    protected:
		/// Id is the id of the theme first theme in the landscape section is number 1
		/// Start is the bit location in the FMTmask at which the attributes of the theme starts.
        int id,start;
    private:
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(id);
			ar & BOOST_SERIALIZATION_NVP(start);
			ar & BOOST_SERIALIZATION_NVP(aggregates);
			ar & BOOST_SERIALIZATION_NVP(source_aggregates);
			ar & BOOST_SERIALIZATION_NVP(valuenames);
			ar & BOOST_SERIALIZATION_NVP(indexes);
			ar & BOOST_SERIALIZATION_NVP(name);
		}
		///This map hold the aggregates (key) of the theme and all attributes of the theme (vector item)
        std::map<std::string, std::vector<std::string>>aggregates;
		///This map hold the aggregates (key) of the theme and all attributes or aggregates of the theme (vector item)
		std::map<std::string, std::vector<std::string>>source_aggregates;
		///This is the main map that keep track of the attributes for the theme and the relative name of the attribute
		std::map<std::string, std::string>valuenames;
		///This data member is only used for the usage of the key word _index in the yield seciton
		std::map<std::string, std::map<std::string,double>>indexes;
		/**
		Using (aggregates) the function turns the map with aggregates as key and aggregates as value to a map with
		aggregates as key and attributes as values.
		*/
		std::map<std::string,std::vector<std::string>>desagregate(std::map<std::string, std::vector<std::string>>aggregates,const std::vector<std::string>&bases);
		///The name of the FMTtheme if their's a name provided by the user.
		std::string name;
		/**
		Convert an attribute|aggregate|? (value) to a bitset for the entire theme size.
		*/
		boost::dynamic_bitset<> strtobits(const std::string& value) const;
		/**
		Convert a bitset to an attribute|aggregate|? (value) for the entire theme size.
		*/
		std::string bitstostr(const boost::dynamic_bitset<>& bits) const;
	public:
		/**
		Default constructor for FMTtheme.
		*/
        FMTtheme();
		/**
		Default destructor for FMTtheme.
		*/
		~FMTtheme() = default;
		/**
		FMTtheme main constructor.
		*/
        FMTtheme(const std::map<std::string, std::vector<std::string>>&laggregates,
			    const  std::map<std::string,std::string>&lvaluenames, 
				const int& lid,const int& lstart, const std::string& lname);
		/**
		FMTtheme constructor to use for FMTtheme indexing.
		*/
        FMTtheme(const std::map<std::string, std::vector<std::string>>&laggregates,
				 const std::map<std::string, std::string>&lvaluenames,
                 const std::map<std::string, std::map<std::string,double>>& indexing,
				const int& lid,const int& lstart,const std::string& lname);
		/**
		A more simple constructor for FMTtheme without aggregates and indexing.
		*/
		FMTtheme(const std::vector<std::string>& lvaluenames, const int& lid, const int& lstart, const std::string& lname);
		/**
		Copy constructor for FMTtheme.
		*/
		FMTtheme(const FMTtheme& rhs);
		/**
		Copy assignment for FMTtheme.
		*/
		FMTtheme& operator = (const FMTtheme& rhs);
       
		/**
		Return true if the (value) is an attribute of the FMTtheme.
		*/
		inline bool isattribute(const std::string& value) const
			{
			return (valuenames.find(value) != valuenames.end());
			}
		/**
		Return true if the (value) is an aggregate of the FMTtheme.
		*/
		inline bool isaggregate(const std::string& value) const
			{
			return (aggregates.find(value) != aggregates.end());
			}
		/**
		Return true if the (value) is an index of the FMTtheme (only use in yields section).
		*/
        bool isindex(const std::string& value) const;
		/**
		Return true if the FMTtheme uses index.
		*/
        bool useindex() const;
		/**
		Get the index for the given (attribute) of the index (value).
		*/
        double getindex(const std::string& attribute,const std::string& value) const;
		/**
		Check if the (value) is part of the (aggregate).
		*/
        bool inaggregate(const std::string& value,const std::string& aggregate);
		/**
		Check if the (value) is a valid attribute | aggregate | ? for the FMTtheme.
		*/
		bool isvalid(const std::string& value) const;
		/**
		Comparison operator of FMTtheme.
		*/
		bool operator == (const FMTtheme& rhs) const;
		/**
		Return the size of FMTtheme can be 0 if the theme has no attribute.
		*/
		inline size_t size() const
			{
			return valuenames.size();
			}
		/**
		Getter for the FMTtheme id.
		*/
		inline int getid() const
			{
			return id;
			}
		/**
		Getter for the FMTtheme name.
		*/
		inline std::string getname() const
			{
			return name;
			}
		/**
		Check if the FMTtheme is empty (no attribute).
		*/
		inline bool empty() const
			{
			return valuenames.empty();
			}
		/**
		Getter the mapping of the attributes of the FMTtheme (attribute for key and attribute name for value).
		*/
		std::map<std::string, std::string>getvaluenames() const;
		
		/**
		Get the attributes of a aggregate (value) for the FMTtheme if aggregate_source == true then the
		aggregate source map is used.
		*/
		std::vector<std::string>getattributes(const std::string& value, bool aggregate_source = false) const;
		/**
		The function presolve the FMTtheme so it removes non used attributes base on the basemask and fill up the 
		selected mask we the selected attribute is the presolved FMTtheme is not empty and size > 1 then
		it gets an newid and a newstart and increment both.
		*/
		FMTtheme presolve(const FMTmask& basemask,int& newid,int& newstart, FMTmask& selected) const;
		/**
		Return a string representation of the FMTtheme seen in a landscape file 
		*/
        operator std::string() const;
    };
}

#endif // FMTtheme_H_INCLUDED
