/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTtheme_Hm_included
#define FMTtheme_Hm_included

#include <boost/dynamic_bitset_fwd.hpp>
#include <boost/serialization/serialization.hpp>
#include <string>
#include <vector>
#include <map>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>
#include "FMTexceptionhandler.h"
#include "FMTobject.h"
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
class FMTMask;
class FMTMaskFilter;
// DocString: FMTTheme
/**
FMTTheme hold multiple attributes for only one theme. FMTTheme gives the description of a part of a FMTMask
from the start bit to the start + theme.size() bit. It also hold the information about attribute aggregates.
FMTTheme is realy close to FMTMask class.
*/
class FMTEXPORT FMTTheme : public FMTObject
    {
    friend class FMTMask;
	friend class Parser::FMTlandscapeparser;
	public:
		// DocString: FMTTheme()
		/**
		Default constructor for FMTTheme.
		*/
		FMTTheme();
		// DocString: ~FMTTheme()
		/**
		Default destructor for FMTTheme.
		*/
		~FMTTheme() = default;
		// DocString: FMTTheme(const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::vector<std::string>>,const std::vector<std::map<std::string, double>>&,const size_t&, const size_t&, const std::string&)
		/**
		FMTTheme constructor to use for FMTTheme indexing.
		*/
		FMTTheme(const std::vector<std::string>& p_attributes,
			const std::vector<std::string>& p_attributenames,
			const std::vector<std::string>& p_aggregates,
			const std::vector<std::vector<std::string>>& p_aggregatenames,
			const std::vector<std::map<std::string, double>>& p_indexes,
			const size_t& p_id, const size_t& p_start, const std::string& p_name);
		// DocString: FMTTheme(const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::vector<std::string>>,const size_t&, const size_t&, const std::string&)
		/**
		FMTTheme constructor to use for FMTTheme indexing.
		*/
		FMTTheme(const std::vector<std::string>& p_attributes,
			const std::vector<std::string>& p_attributenames,
			const std::vector<std::string>& p_aggregates,
			const std::vector<std::vector<std::string>>& p_aggregatenames,
			const size_t& p_id, const size_t& p_start, const std::string& p_name);
		// DocString: FMTTheme(const std::vector<std::string>& lattributes,const size_t&,const size_t&,const std::string&)
		/**
		A more simple constructor for FMTTheme without aggregates and indexing.
		*/
		FMTTheme(const std::vector<std::string>& p_attributes,
			const size_t& p_id, const size_t& p_start, const std::string& p_name);
		// DocString: FMTTheme(const FMTTheme&)
		/**
		Copy constructor for FMTTheme.
		*/
		FMTTheme(const FMTTheme& rhs);
		// DocString: FMTTheme::operator=
		/**
		Copy assignment for FMTTheme.
		*/
		FMTTheme& operator = (const FMTTheme& rhs);
		// DocString: FMTTheme::isAttribute
		/**
		Return true if the (value) is an attribute of the FMTTheme.
		*/
		inline bool isAttribute(const std::string& p_value) const
		{
			if (!p_value.empty() && p_value.at(0) == '!')
			{
				return (getAttribute(p_value.substr(1, p_value.size())) != m_attributem_locations.end());
			}else {
				return (getAttribute(p_value) != m_attributem_locations.end());
				}
			
		}
		// DocString: FMTTheme::isAggregate
		/**
		Return true if the (value) is an aggregate of the FMTTheme.
		*/
		inline bool isAggregate(const std::string& p_value) const
		{
			if (!p_value.empty() && p_value.at(0) == '!')
			{
				return (std::find(m_aggregates.begin(), m_aggregates.end(), p_value.substr(1, p_value.size())) != 
										m_aggregates.end());
			}else {
				return (std::find(m_aggregates.begin(), m_aggregates.end(), p_value) != m_aggregates.end());
			}
			
		}
		// DocString: FMTTheme::isIndex
		/**
		Return true if the (value) is an index of the FMTTheme (only use in yields section).
		*/
		bool isIndex(const std::string& p_attribute, const std::string& p_value) const;
		// DocString: FMTTheme::isIndex
		/**
		Return true if the (value) is an index of the FMTTheme (only use in yields section).
		*/
		bool isIndex(const std::string& p_value) const;
		// DocString: FMTTheme::useIndex
		/**
		Return true if the FMTTheme uses index.
		*/
		bool useIndex() const;
		// DocString: FMTTheme::getIndex
		/**
		Get the index for the given (attribute) of the index (value).
		*/
		double getIndex(const std::string& p_attribute, const std::string& p_value) const;
		// DocString: FMTTheme::inAggregate
		/**
		Check if the (value) is part of the (aggregate).
		*/
		bool inAggregate(const std::string& p_value, const std::string& p_aggregate);
		// DocString: FMTTheme::isValid
		/**
		Check if the (value) is a valid attribute | aggregate | ? for the FMTTheme.
		*/
		bool isValid(const std::string& p_value) const;
		// DocString: FMTTheme::operator==
		/**
		Comparison operator of FMTTheme.
		*/
		bool operator == (const FMTTheme& p_rhs) const;
		// DocString: FMTTheme::size
		/**
		Return the size of FMTTheme can be 0 if the theme has no attribute.
		*/
		inline size_t size() const
		{
			return m_attributes.size();
		}
		// DocString: FMTTheme::getStart
		/**
		Return the bit location of the mask at which the theme starts.
		*/
		inline const size_t& getStart() const
		{
			return m_start;
		}
		// DocString: FMTTheme::getId
		/**
		Getter for the FMTTheme id.
		*/
		inline const size_t& getId() const
		{
			return m_id;
		}
		// DocString: FMTTheme::getName
		/**
		Getter for the FMTTheme name.
		*/
		inline std::string getName() const
		{
			return m_name;
		}
		// DocString: FMTTheme::empty
		/**
		Check if the FMTTheme is empty (no attribute).
		*/
		inline bool empty() const
		{
			return m_attributes.empty();
		}
		// DocString: FMTTheme::getAttributes
		/**
		Get the attributes of a aggregate (value) for the FMTTheme if aggregate_source == true then the
		aggregate source map is used.
		*/
		std::vector<std::string>getAttributes(const std::string& p_value, bool p_aggregate_source = false) const;
		// DocString: FMTTheme::getBaseAttributes
		/**
		Get a reference to the base attributes of the theme.
		*/
		inline const std::vector<std::string>& getBaseAttributes() const
		{
			return m_attributes;
		}
		// DocString: FMTTheme::getAttributeNames
		/**
		Get the names of the attributes.
		*/
		const std::vector<std::string>& getAttributeNames() const
		{
			return m_attributenames;
		}
		// DocString: FMTTheme::presolve
		/**
		The function presolve the FMTTheme so it removes non used attributes base on the baseMask and fill up the
		selected mask we the selected attribute is the presolved FMTTheme is not empty and size > 1 then
		it gets an newid and a newstart and increment both.
		*/
		FMTTheme presolve(FMTMaskFilter& p_maskfilter, size_t& p_newid, size_t& p_newstart) const;
		// DocString: FMTTheme::updateFromMask
		/**
		Base on a global mask it will update the theme with the aggregate in the mask or with newly created aggregates.
		It will also return the new attribute value.
		*/
		std::string updateFromMask(const Core::FMTMask& p_globalmask);
		// DocString: FMTTheme::operator std::string
		/**
		Return a string representation of the FMTTheme seen in a landscape file
		*/
		operator std::string() const;
#if defined FMTWITHR
		// DocString:  FMTTheme::getaggregatesdataframe
		/**
		Returns a dataframe filled up with the aggregates of each themes (col1 = THEME, col2 = ATTRIBUTES,col3 = AGGREGATES)
		*/
		Rcpp::DataFrame getAggregatesAsDataFrame() const;
		// DocString:  FMTTheme::getAttributesAsDataFrame
		/**
		Returns a dataframe filled up with the attributes (col1 = ATTRIBUTE, col2 = NAMES)
		*/
		Rcpp::DataFrame getAttributesAsDataFrame() const;
#endif
		// DocString: FMTTheme::validate
		/**
		The function validate a the construction of a valid FMTMask using the mask string based on the
		themes. If their's less themes that the number present in the string mask then the string mask is
		going to be trim for the good number of FMTthemes.
		*/
		static bool validate(const std::vector<Core::FMTTheme>& p_themes,
			std::string& p_mask, std::string p_otherinformation = std::string());
		// DocString: FMTObject::theme
		/**
		This function validate the mask string for a given vector of themes and throw exception if
		something is not right. It'S called by the validate function.
		*/
		static bool checkMask(const std::vector<Core::FMTTheme>& p_themes,
			const std::vector<std::string>& p_values, std::string& p_mask,
			const std::string& p_otherinformation);
		// DocString: FMTObject::getAggregates
		/**
		Returns the aggregates of the theme.
		*/
		std::vector<std::string>getAggregates() const;
	protected:
		// DocString: FMTTheme::id
		/// Id is the id of the theme first theme in the landscape section is number 1.
		size_t m_id;
		// DocString: FMTTheme::start
		/// Start is the bit location in the FMTMask at which the attributes of the theme starts.
		size_t m_start;
    private:
		// DocString: FMTTheme::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try{
				ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
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
				_exhandler->printExceptions("", "FMTTheme::serialize", __LINE__, __FILE__);
				}
		}
		// DocString: FMTTheme::attributes
		///Attributes of the theme
		std::vector<std::string>m_attributes;
		// DocString: FMTTheme::attributenames
		///Attributes names of the theme (can be empty).
		std::vector<std::string>m_attributenames;
		// DocString: FMTTheme::aggregates
		///Aggregates of the theme (can be empty).
		std::vector<std::string>m_aggregates;
		// DocString: FMTTheme::aggregates
		///Aggregate names of the theme (can be empty).
		std::vector<std::vector<std::string>>m_aggregatenames;
		// DocString: FMTTheme::indexes
		///Indexes for each attributes (can be empty).
		std::vector<std::map<std::string, double>>m_indexes;
		// DocString: FMTTheme::attributem_locations
		///Attributes location used for optimization of the whole class.
		std::unordered_map<std::string, std::vector<size_t>>m_attributem_locations;
		// DocString: FMTTheme::lookiterator
		///Lookup iterator for the attributes.
		typedef typename std::unordered_map<std::string, std::vector<size_t>>::const_iterator lookiterator;
		// DocString: FMTTheme::name
		///The name of the FMTTheme if their's a name provided by the user.
		std::string m_name;
		// DocString: FMTTheme::strToBits
		/**
		Convert an attribute|aggregate|? (value) to a bitset for the entire theme size.
		*/
		 boost::dynamic_bitset<uint8_t> strToBits(const std::string& p_value) const;
		// DocString: FMTTheme::_getCount
		/**
		@brief Get the turned on bits of the theme subset.
		@param[in] the mask.
		@return the count
		*/
		size_t _getCount(const Core::FMTMask& p_mask) const;
		// DocString: FMTTheme::_getFlipCount
		/**
		@brief Get the turned off bits of the theme subset.
		@param[in] the mask.
		@return the count
		*/
		size_t _getFlipCount(const Core::FMTMask& p_mask) const;
		// DocString: FMTTheme::_findFirstFlip
		/**
		@brief Get the turned off bits of the theme subset.
		@param[in] the mask.
		@return the count
		*/
		size_t _findFirstFlip(const Core::FMTMask& p_mask) const;
		// DocString: FMTTheme::_findFirst
		/**
		@brief Get the turned on bits of the theme subset.
		@param[in] the mask.
		@return the count
		*/
		size_t _findFirst(const Core::FMTMask& p_mask) const;
		// DocString: FMTTheme::_isEqual
		/**
		@brief check if mask subset is equal to p_bits
		@param[in] the mask.
		@param[in] the bits to check.
		@return true if equal
		*/
		bool _isEqual(const Core::FMTMask& p_mask,
			const boost::dynamic_bitset<uint8_t>& p_bits) const;
		// DocString: FMTTheme::_isFlipEqual
		/**
		@brief check if fmask subset is equal to flipped p_bits
		@param[in] the mask.
		@param[in] the bits to check.
		@return true if equal
		*/
		bool _isFlipEqual(const Core::FMTMask& p_mask,
			const boost::dynamic_bitset<uint8_t>& p_bits) const;
		// DocString: FMTTheme::bitsToStr
		/**
		@brief Convert a bitset to an attribute|aggregate|? (value) for the entire theme size.
		@param[in] the mask
		@return the string value;
		*/
		std::string bitsToStr(const Core::FMTMask& p_mask) const;
		// DocString: FMTTheme::_getAttribute
		/**
		@brief get a string reference to the attribute targeted by attributeId
		@param[in] p_attributeId the attribute Id
		@return the reference to the attribute targeted;
		*/
		const std::string& _getAttribute(size_t p_attributeId) const;
		// DocString: FMTTheme::getAttribute
		/**
		Get the attribute iterator, will raise if not found.
		*/
		lookiterator getAttribute(const std::string& p_value, bool p_raiseifnotfound = false) const;
		// DocString: FMTTheme::buildAttributeLocations
		/**
		For optimization build the attribute location of the theme. If only new aggregates added
		specify the aggregate id to skip some process if 0 will do everything.
		*/
		void buildAttributeLocations();
		// DocString: FMTTheme::fillupAggregates
		/**
		Fill up the aggregates vectors, vector<int> = themeid startingfrom 1, vector<std::string> = attributes, vector<std::string> = aggregates.
		*/
		void fillupAggregates(std::vector<int>& p_themeids, std::vector<std::string>& p_locattributes, std::vector<std::string>& p_locaggregates) const;
		// DocString: FMTTheme::pushAggregate
		/**
		Push a new aggregate.
		*/
		void pushAggregate(const std::string& p_aggregatename);
		// DocString: FMTTheme::pushAggregate
		/**
		Push a new aggregate value.
		*/
		void pushAggregateValue(const std::string& p_aggregatename, const std::string& p_value);
	
	
    };

	// DocString: FMTThemeComparator
	/**
	FMTThemeComparator to check two themes are the same.
	*/
	class FMTThemeComparator
	{
	public:
		// DocString: FMTThemeComparator(const FMTTheme&,const bool&)
		/**
		FMTThemeComparator constructor ltheme_mask is the theme of that we want to match.
		*/
		FMTThemeComparator(const FMTTheme& p_lbase_theme,const bool& p_lcomparedwithpresolved=false);
		// DocString: FMTthemecomparatorr::operator()(const FMTTheme&)
		/**
		Matching test operator for FMTThemeComparator.
		*/
		bool operator()(const FMTTheme& p_theme) const;
	private:
		// DocString: FMTThemeComparator::base_theme
		///The theme that we are looking for
		FMTTheme m_base_theme;
		// DocString: FMTThemeComparator::comparedwithpresolved
		///If true, the comparator is different
		bool m_comparedwithpresolved;

	};



}

BOOST_CLASS_EXPORT_KEY(Core::FMTTheme)

#endif // FMTtheme_Hm_included
