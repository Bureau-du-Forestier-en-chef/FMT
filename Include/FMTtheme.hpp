/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTtheme_H_INCLUDED
#define FMTtheme_H_INCLUDED

#include <boost/dynamic_bitset_fwd.hpp>
#include <boost/serialization/serialization.hpp>
#include <string>
#include <vector>
#include <map>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>
#include "FMTexceptionhandler.hpp"
#include "FMTobject.hpp"
#include <unordered_map>

#if defined FMTWITHR
	#include <Rcpp.h>
#endif 

namespace Parser
{
	class FMTlandscapeparser;
}




namespace Core
{
class FMTmask;
class FMTmaskfilter;
// DocString: FMTtheme
/**
FMTtheme hold multiple attributes for only one theme. FMTtheme gives the description of a part of a FMTmask
from the start bit to the start + theme.size() bit. It also hold the information about attribute aggregates.
FMTtheme is realy close to FMTmask class.
*/
class FMTEXPORT FMTtheme : public FMTobject
    {
    friend class FMTmask;
	friend class Parser::FMTlandscapeparser;
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
		FMTtheme(const std::vector<std::string>& p_attributes,
			const std::vector<std::string>& p_attributenames,
			const std::vector<std::string>& p_aggregates,
			const std::vector<std::vector<std::string>>& p_aggregatenames,
			const std::vector<std::map<std::string, double>>& p_indexes,
			const size_t& p_id, const size_t& p_start, const std::string& p_name);
		// DocString: FMTtheme(const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::vector<std::string>>,const size_t&, const size_t&, const std::string&)
		/**
		FMTtheme constructor to use for FMTtheme indexing.
		*/
		FMTtheme(const std::vector<std::string>& p_attributes,
			const std::vector<std::string>& p_attributenames,
			const std::vector<std::string>& p_aggregates,
			const std::vector<std::vector<std::string>>& p_aggregatenames,
			const size_t& p_id, const size_t& p_start, const std::string& p_name);
		// DocString: FMTtheme(const std::vector<std::string>& lattributes,const size_t&,const size_t&,const std::string&)
		/**
		A more simple constructor for FMTtheme without aggregates and indexing.
		*/
		FMTtheme(const std::vector<std::string>& p_attributes,
			const size_t& p_id, const size_t& p_start, const std::string& p_name);
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
		inline bool isattribute(const std::string& p_value) const
		{
			return (getAttribute(p_value) != m_attribute_locations.end());
		}
		// DocString: FMTtheme::isaggregate
		/**
		Return true if the (value) is an aggregate of the FMTtheme.
		*/
		inline bool isaggregate(const std::string& p_value) const
		{
			return (std::find(m_aggregates.begin(), m_aggregates.end(), p_value) != m_aggregates.end());
		}
		// DocString: FMTtheme::isindex
		/**
		Return true if the (value) is an index of the FMTtheme (only use in yields section).
		*/
		bool isindex(const std::string& p_attribute, const std::string& p_value) const;
		// DocString: FMTtheme::isindex
		/**
		Return true if the (value) is an index of the FMTtheme (only use in yields section).
		*/
		bool isindex(const std::string& p_value) const;
		// DocString: FMTtheme::useindex
		/**
		Return true if the FMTtheme uses index.
		*/
		bool useindex() const;
		// DocString: FMTtheme::getindex
		/**
		Get the index for the given (attribute) of the index (value).
		*/
		double getindex(const std::string& p_attribute, const std::string& p_value) const;
		// DocString: FMTtheme::inAggregate
		/**
		Check if the (value) is part of the (aggregate).
		*/
		bool inAggregate(const std::string& p_value, const std::string& p_aggregate);
		// DocString: FMTtheme::isValid
		/**
		Check if the (value) is a valid attribute | aggregate | ? for the FMTtheme.
		*/
		bool isValid(const std::string& p_value) const;
		// DocString: FMTtheme::operator==
		/**
		Comparison operator of FMTtheme.
		*/
		bool operator == (const FMTtheme& p_rhs) const;
		// DocString: FMTtheme::size
		/**
		Return the size of FMTtheme can be 0 if the theme has no attribute.
		*/
		inline size_t size() const
		{
			return m_attributes.size();
		}
		// DocString: FMTtheme::getstart
		/**
		Return the bit location of the mask at which the theme starts.
		*/
		inline const size_t& getstart() const
		{
			return m_start;
		}
		// DocString: FMTtheme::getid
		/**
		Getter for the FMTtheme id.
		*/
		inline const size_t& getid() const
		{
			return m_id;
		}
		// DocString: FMTtheme::getname
		/**
		Getter for the FMTtheme name.
		*/
		inline std::string getname() const
		{
			return m_name;
		}
		// DocString: FMTtheme::empty
		/**
		Check if the FMTtheme is empty (no attribute).
		*/
		inline bool empty() const
		{
			return m_attributes.empty();
		}
		// DocString: FMTtheme::getattributes
		/**
		Get the attributes of a aggregate (value) for the FMTtheme if aggregate_source == true then the
		aggregate source map is used.
		*/
		std::vector<std::string>getattributes(const std::string& p_value, bool p_aggregate_source = false) const;
		// DocString: FMTtheme::getbaseattributes
		/**
		Get a reference to the base attributes of the theme.
		*/
		inline const std::vector<std::string>& getbaseattributes() const
		{
			return m_attributes;
		}
		// DocString: FMTtheme::getattributenames
		/**
		Get the names of the attributes.
		*/
		const std::vector<std::string>& getattributenames() const
		{
			return m_attributenames;
		}
		// DocString: FMTtheme::presolve
		/**
		The function presolve the FMTtheme so it removes non used attributes base on the basemask and fill up the
		selected mask we the selected attribute is the presolved FMTtheme is not empty and size > 1 then
		it gets an newid and a newstart and increment both.
		*/
		FMTtheme presolve(FMTmaskfilter& p_maskfilter, size_t& p_newid, size_t& p_newstart) const;
		// DocString: FMTtheme::updateFromMask
		/**
		Base on a global mask it will update the theme with the aggregate in the mask or with newly created aggregates.
		It will also return the new attribute value.
		*/
		std::string updateFromMask(const Core::FMTmask& p_globalmask);
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
		// DocString: FMTtheme::validate
	/**
	The funciton validate a the construction of a valid FMTmask using the mask string based on the
	themes. If their's less themes that the number present in the string mask then the string mask is
	going to be trim for the good number of FMTthemes.
	*/
		static bool validate(const std::vector<Core::FMTtheme>& p_themes,
			std::string& p_mask, std::string p_otherinformation = std::string());
		// DocString: FMTobject::theme
		/**
		This function validate the mask string for a given vector of themes and throw exception if
		something is not right. It'S called by the validate function.
		*/
		static bool checkMask(const std::vector<Core::FMTtheme>& p_themes,
			const std::vector<std::string>& p_values, std::string& p_mask,
			const std::string& p_otherinformation);
		// DocString: FMTobject::getaggregates
		/**
		Returns the aggregates of the theme.
		*/
		std::vector<std::string>getaggregates() const;
	protected:
		// DocString: FMTtheme::id
		/// Id is the id of the theme first theme in the landscape section is number 1.
		size_t m_id;
		// DocString: FMTtheme::start
		/// Start is the bit location in the FMTmask at which the attributes of the theme starts.
		size_t m_start;
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
				ar & BOOST_SERIALIZATION_NVP(m_id);
				ar & BOOST_SERIALIZATION_NVP(m_start);
				ar & BOOST_SERIALIZATION_NVP(m_attributes);
				ar & BOOST_SERIALIZATION_NVP(m_attributenames);
				ar & BOOST_SERIALIZATION_NVP(m_aggregates);
				ar & BOOST_SERIALIZATION_NVP(m_aggregatenames);
				ar & BOOST_SERIALIZATION_NVP(m_indexes);
				ar & BOOST_SERIALIZATION_NVP(m_name);
				buildAttributeLocations();
			}catch (...)
				{
				_exhandler->printexceptions("", "FMTtheme::serialize", __LINE__, __FILE__);
				}
		}
		// DocString: FMTtheme::attributes
		///Attributes of the theme
		std::vector<std::string>m_attributes;
		// DocString: FMTtheme::attributenames
		///Attributes names of the theme (can be empty).
		std::vector<std::string>m_attributenames;
		// DocString: FMTtheme::aggregates
		///Aggregates of the theme (can be empty).
		std::vector<std::string>m_aggregates;
		// DocString: FMTtheme::aggregates
		///Aggregate names of the theme (can be empty).
		std::vector<std::vector<std::string>>m_aggregatenames;
		// DocString: FMTtheme::indexes
		///Indexes for each attributes (can be empty).
		std::vector<std::map<std::string, double>>m_indexes;
		// DocString: FMTtheme::attribute_locations
		///Attributes location used for optimization of the whole class.
		std::unordered_map<std::string, std::vector<size_t>>m_attribute_locations;
		// DocString: FMTtheme::lookiterator
		///Lookup iterator for the attributes.
		typedef typename std::unordered_map<std::string, std::vector<size_t>>::const_iterator lookiterator;
		// DocString: FMTtheme::name
		///The name of the FMTtheme if their's a name provided by the user.
		std::string m_name;
		// DocString: FMTtheme::strToBits
		/**
		Convert an attribute|aggregate|? (value) to a bitset for the entire theme size.
		*/
		 boost::dynamic_bitset<uint8_t> strToBits(const std::string& p_value) const;
		// DocString: FMTtheme::bitsToStr
		/**
		Convert a bitset to an attribute|aggregate|? (value) for the entire theme size.
		*/
		std::string bitsToStr(const boost::dynamic_bitset<uint8_t>& p_bits) const;
		// DocString: FMTtheme::getAttribute
		/**
		Get the attribute iterator, will raise if not found.
		*/
		lookiterator getAttribute(const std::string& p_value, bool p_raiseifnotfound = false) const;
		// DocString: FMTtheme::buildAttributeLocations
		/**
		For optimization build the attribute location of the theme. If only new aggregates added
		specify the aggregate id to skip some process if 0 will do everything.
		*/
		void buildAttributeLocations();
		// DocString: FMTtheme::fillupAggregates
		/**
		Fill up the aggregates vectors, vector<int> = themeid startingfrom 1, vector<std::string> = attributes, vector<std::string> = aggregates.
		*/
		void fillupAggregates(std::vector<int>& p_themeids, std::vector<std::string>& p_locattributes, std::vector<std::string>& p_locaggregates) const;
		// DocString: FMTtheme::push_aggregate
		/**
		Push a new aggregate.
		*/
		void push_aggregate(const std::string& p_aggregatename);
		// DocString: FMTtheme::push_aggregate
		/**
		Push a new aggregate value.
		*/
		void push_aggregate_value(const std::string& p_aggregatename, const std::string& p_value);
	
	
    };

	// DocString: FMTthemecomparator
	/**
	FMTthemecomparator to check two themes are the same.
	*/
	class FMTthemecomparator
	{
	public:
		// DocString: FMTthemecomparator(const FMTtheme&,const bool&)
		/**
		FMTthemecomparator constructor ltheme_mask is the theme of that we want to match.
		*/
		FMTthemecomparator(const FMTtheme& p_lbase_theme,const bool& p_lcomparedwithpresolved=false);
		// DocString: FMTthemecomparatorr::operator()(const FMTtheme&)
		/**
		Matching test operator for FMTthemecomparator.
		*/
		bool operator()(const FMTtheme& p_theme) const;
	private:
		// DocString: FMTthemecomparator::base_theme
		///The theme that we are looking for
		FMTtheme m_base_theme;
		// DocString: FMTthemecomparator::comparedwithpresolved
		///If true, the comparator is different
		bool m_comparedwithpresolved;

	};



}

BOOST_CLASS_EXPORT_KEY(Core::FMTtheme)

#endif // FMTtheme_H_INCLUDED
