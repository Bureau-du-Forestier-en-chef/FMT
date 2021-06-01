/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
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
#include "FMTobject.h"

#if defined FMTWITHR
	#include <Rcpp.h>
#endif 



namespace Core
{
// DocString: FMTtheme
/**
FMTtheme hold multiple attributes for only one theme. FMTtheme gives the description of a part of a FMTmask
from the start bit to the start + theme.size() bit. It also hold the information about attribute aggregates.
FMTtheme is realy close to FMTmask class.
*/
class FMTEXPORT FMTtheme : public FMTobject
    {
    friend class FMTmask;
    protected:
		// DocString: FMTtheme::id
		/// Id is the id of the theme first theme in the landscape section is number 1.
		size_t id;
		// DocString: FMTtheme::start
		/// Start is the bit location in the FMTmask at which the attributes of the theme starts.
		size_t start;
    private:
		// DocString: FMTtheme::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try{
				ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
				ar & BOOST_SERIALIZATION_NVP(id);
				ar & BOOST_SERIALIZATION_NVP(start);
				ar & BOOST_SERIALIZATION_NVP(attributes);
				ar & BOOST_SERIALIZATION_NVP(attributenames);
				ar & BOOST_SERIALIZATION_NVP(aggregates);
				ar & BOOST_SERIALIZATION_NVP(aggregatenames);
				ar & BOOST_SERIALIZATION_NVP(indexes);
				ar & BOOST_SERIALIZATION_NVP(name);
				buildattributelocations();
			}catch (...)
				{
				_exhandler->printexceptions("", "FMTtheme::serialize", __LINE__, __FILE__);
				}
		}
		// DocString: FMTtheme::attributes
		///Attributes of the theme
		std::vector<std::string>attributes;
		// DocString: FMTtheme::attributenames
		///Attributes names of the theme (can be empty).
		std::vector<std::string>attributenames;
		// DocString: FMTtheme::aggregates
		///Aggregates of the theme (can be empty).
		std::vector<std::string>aggregates;
		// DocString: FMTtheme::aggregates
		///Aggregate names of the theme (can be empty).
		std::vector<std::vector<std::string>>aggregatenames;
		// DocString: FMTtheme::indexes
		///Indexes for each attributes (can be empty).
		std::vector<std::map<std::string, double>>indexes;
		// DocString: FMTtheme::attribute_locations
		///Attributes location used for optimization of the whole class.
		boost::unordered_map<std::string, std::vector<size_t>>attribute_locations;
		// DocString: FMTtheme::lookiterator
		///Lookup iterator for the attributes.
		typedef typename boost::unordered_map<std::string, std::vector<size_t>>::const_iterator lookiterator;
		// DocString: FMTtheme::name
		///The name of the FMTtheme if their's a name provided by the user.
		std::string name;
		// DocString: FMTtheme::strtobits
		/**
		Convert an attribute|aggregate|? (value) to a bitset for the entire theme size.
		*/
		boost::dynamic_bitset<> strtobits(const std::string& value) const;
		// DocString: FMTtheme::bitstostr
		/**
		Convert a bitset to an attribute|aggregate|? (value) for the entire theme size.
		*/
		std::string bitstostr(const boost::dynamic_bitset<>& bits) const;
		// DocString: FMTtheme::getattribute
		/**
		Get the attribute iterator, will raise if not found.
		*/
		lookiterator getattribute(const std::string& value, bool raiseifnotfound = false) const;
		// DocString: FMTtheme::buildattributelocations
		/**
		For optimization build the attribute location of the theme.
		*/
		void buildattributelocations();
		// DocString: FMTtheme::fillupaggregates
		/**
		Fill up the aggregates vectors, vector<int> = themeid startingfrom 1, vector<std::string> = attributes, vector<std::string> = aggregates.
		*/
		void fillupaggregates(std::vector<int>& themeids, std::vector<std::string>& locattributes, std::vector<std::string>& locaggregates) const;
	public:
		// DocString: FMTtheme()
		/**
		Default constructor for FMTtheme.
		*/
        FMTtheme() = default;
		// DocString: ~FMTtheme()
		/**
		Default destructor for FMTtheme.
		*/
		~FMTtheme() = default;
		// DocString: FMTtheme(const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::vector<std::string>>,const std::vector<std::map<std::string, double>>&,const size_t&, const size_t&, const std::string&)
		/**
		FMTtheme constructor to use for FMTtheme indexing.
		*/
		FMTtheme(const std::vector<std::string>& lattributes,
			const std::vector<std::string>& lattributenames,
			const std::vector<std::string>& laggregates,
			const std::vector<std::vector<std::string>>& laggregatenames,
			const std::vector<std::map<std::string, double>>& lindexes,
			const size_t& lid, const size_t& lstart, const std::string& lname);
		// DocString: FMTtheme(const std::vector<std::string>& lattributes,const size_t&,const size_t&,const std::string&)
		/**
		A more simple constructor for FMTtheme without aggregates and indexing.
		*/
		FMTtheme(const std::vector<std::string>& lattributes,
			const size_t& lid, const size_t& lstart, const std::string& lname);


		// DocString: FMTtheme(const FMTtheme&)
		/**
		Copy constructor for FMTtheme.
		*/
		FMTtheme(const FMTtheme& rhs);
		// DocString: FMTtheme::operator=
		/**
		Copy assignment for FMTtheme.
		*/
		FMTtheme& operator = (const FMTtheme& rhs);
		// DocString: FMTtheme::isattribute
		/**
		Return true if the (value) is an attribute of the FMTtheme.
		*/
		inline bool isattribute(const std::string& value) const
			{
			return (getattribute(value) != attribute_locations.end());
			}
		// DocString: FMTtheme::isaggregate
		/**
		Return true if the (value) is an aggregate of the FMTtheme.
		*/
		inline bool isaggregate(const std::string& value) const
			{
			return (std::find(aggregates.begin(),aggregates.end(),value)!= aggregates.end());
			}
		// DocString: FMTtheme::isindex
		/**
		Return true if the (value) is an index of the FMTtheme (only use in yields section).
		*/
        bool isindex(const std::string& attribute, const std::string& value) const;
		// DocString: FMTtheme::isindex
		/**
		Return true if the (value) is an index of the FMTtheme (only use in yields section).
		*/
		bool isindex(const std::string& value) const;
		// DocString: FMTtheme::useindex
		/**
		Return true if the FMTtheme uses index.
		*/
        bool useindex() const;
		// DocString: FMTtheme::getindex
		/**
		Get the index for the given (attribute) of the index (value).
		*/
        const double& getindex(const std::string& attribute,const std::string& value) const;
		// DocString: FMTtheme::inaggregate
		/**
		Check if the (value) is part of the (aggregate).
		*/
        bool inaggregate(const std::string& value,const std::string& aggregate);
		// DocString: FMTtheme::isvalid
		/**
		Check if the (value) is a valid attribute | aggregate | ? for the FMTtheme.
		*/
		bool isvalid(const std::string& value) const;
		// DocString: FMTtheme::operator==
		/**
		Comparison operator of FMTtheme.
		*/
		bool operator == (const FMTtheme& rhs) const;
		// DocString: FMTtheme::size
		/**
		Return the size of FMTtheme can be 0 if the theme has no attribute.
		*/
		inline size_t size() const
			{
			return attributes.size();
			}
		// DocString: FMTtheme::getstart
		/**
		Return the bit location of the mask at which the theme starts.
		*/
		inline const size_t& getstart() const
			{
			return start;
			}
		// DocString: FMTtheme::getid
		/**
		Getter for the FMTtheme id.
		*/
		inline const size_t& getid() const
			{
			return id;
			}
		// DocString: FMTtheme::getname
		/**
		Getter for the FMTtheme name.
		*/
		inline std::string getname() const
			{
			return name;
			}
		// DocString: FMTtheme::empty
		/**
		Check if the FMTtheme is empty (no attribute).
		*/
		inline bool empty() const
			{
			return attributes.empty();
			}
		// DocString: FMTtheme::getattributes
		/**
		Get the attributes of a aggregate (value) for the FMTtheme if aggregate_source == true then the
		aggregate source map is used.
		*/
		std::vector<std::string>getattributes(const std::string& value, bool aggregate_source = false) const;
		// DocString: FMTtheme::getbaseattributes
		/**
		Get a reference to the base attributes of the theme.
		*/
		inline const std::vector<std::string>& getbaseattributes() const
			{
			return attributes;
			}
		// DocString: FMTtheme::presolve
		/**
		The function presolve the FMTtheme so it removes non used attributes base on the basemask and fill up the 
		selected mask we the selected attribute is the presolved FMTtheme is not empty and size > 1 then
		it gets an newid and a newstart and increment both.
		*/
		FMTtheme presolve(const FMTmask& basemask, size_t& newid, size_t& newstart, FMTmask& selected) const;
		// DocString: FMTtheme::operator std::string
		/**
		Return a string representation of the FMTtheme seen in a landscape file 
		*/
        operator std::string() const;
	#if defined FMTWITHR
			// DocString:  FMTtheme::getaggregatesdataframe
			/**
			Returns a dataframe filled up with the aggregates of each themes (col1 = THEME, col2 = ATTRIBUTES,col3 = AGGREGATES)
			*/
			Rcpp::DataFrame getaggregatesasdataframe() const;
			// DocString:  FMTtheme::getattributesasdataframe
			/**
			Returns a dataframe filled up with the attributes (col1 = ATTRIBUTE, col2 = NAMES)
			*/
			Rcpp::DataFrame getattributesasdataframe() const;
	#endif
    };

	// DocString: FMTthemecomparator
	/**
	FMTthemecomparator to check two themes are the same.
	*/
	class FMTthemecomparator
	{
		// DocString: FMTthemecomparator::base_theme
		///The theme that we are looking for
		FMTtheme base_theme;
		// DocString: FMTthemecomparator::comparedwithpresolved
		///If true, the comparator is different
		bool comparedwithpresolved;
	public:
		// DocString: FMTthemecomparator(const FMTtheme&,const bool&)
		/**
		FMTthemecomparator constructor ltheme_mask is the theme of that we want to match.
		*/
		FMTthemecomparator(const FMTtheme& lbase_theme,const bool& lcomparedwithpresolved=false);
		// DocString: FMTthemecomparatorr::operator()(const FMTtheme&)
		/**
		Matching test operator for FMTthemecomparator.
		*/
		bool operator()(const FMTtheme& theme) const;

	};



}

BOOST_CLASS_EXPORT_KEY(Core::FMTtheme)

#endif // FMTtheme_H_INCLUDED
