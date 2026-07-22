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
#include "FMTExceptionHandler.h"
#include "FMTObject.h"
#include <unordered_map>

#if defined FMTWITHR
	#include <Rcpp.h>
#endif 

namespace Parser
{
	class FMTLandscapeParser;
}




namespace Core
{
class FMTMask;
class FMTMaskFilter;
// DocString: FMTTheme
/**
@brief Class holding the attributes and aggregates of a single theme, describing a part of a FMTMask.
@details A FMTTheme describes a part of a FMTMask from its start bit to start + size, and holds the attribute aggregates. It is closely related to FMTMask.
*/
class FMTEXPORT FMTTheme : public FMTObject
    {
    friend class FMTMask;
	friend class Parser::FMTLandscapeParser;
	public:
		// DocString: FMTTheme()
		/**
		@brief Default constructor for FMTTheme.
		*/
		FMTTheme();
		// DocString: ~FMTTheme()
		/**
		@brief Default destructor for FMTTheme.
		*/
		~FMTTheme() = default;
		// DocString: FMTTheme(const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::vector<std::string>>,const std::vector<std::map<std::string, double>>&,const size_t&, const size_t&, const std::string&)
		/**
		@brief Constructor used for theme indexing with indexes.
		@param[in] p_attributes the attributes.
		@param[in] p_attributenames the attribute names.
		@param[in] p_aggregates the aggregates.
		@param[in] p_aggregatenames the aggregate names.
		@param[in] p_indexes the indexes of each attribute.
		@param[in] p_id the id of the theme.
		@param[in] p_start the start bit location.
		@param[in] p_name the name of the theme.
		*/
		FMTTheme(const std::vector<std::string>& p_attributes,
			const std::vector<std::string>& p_attributenames,
			const std::vector<std::string>& p_aggregates,
			const std::vector<std::vector<std::string>>& p_aggregatenames,
			const std::vector<std::map<std::string, double>>& p_indexes,
			const size_t& p_id, const size_t& p_start, const std::string& p_name);
		// DocString: FMTTheme(const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::vector<std::string>>,const size_t&, const size_t&, const std::string&)
		/**
		@brief Constructor used for theme indexing.
		@param[in] p_attributes the attributes.
		@param[in] p_attributenames the attribute names.
		@param[in] p_aggregates the aggregates.
		@param[in] p_aggregatenames the aggregate names.
		@param[in] p_id the id of the theme.
		@param[in] p_start the start bit location.
		@param[in] p_name the name of the theme.
		*/
		FMTTheme(const std::vector<std::string>& p_attributes,
			const std::vector<std::string>& p_attributenames,
			const std::vector<std::string>& p_aggregates,
			const std::vector<std::vector<std::string>>& p_aggregatenames,
			const size_t& p_id, const size_t& p_start, const std::string& p_name);
		// DocString: FMTTheme(const std::vector<std::string>& lattributes,const size_t&,const size_t&,const std::string&)
		/**
		@brief Simpler constructor for a theme without aggregates or indexing.
		@param[in] p_attributes the attributes.
		@param[in] p_id the id of the theme.
		@param[in] p_start the start bit location.
		@param[in] p_name the name of the theme.
		*/
		FMTTheme(const std::vector<std::string>& p_attributes,
			const size_t& p_id, const size_t& p_start, const std::string& p_name);
		// DocString: FMTTheme(const FMTTheme&)
		/**
		@brief Copy constructor for FMTTheme.
		@param[in] rhs the FMTTheme to copy.
		*/
		FMTTheme(const FMTTheme& rhs);
		// DocString: FMTTheme::operator=
		/**
		@brief Copy assignment operator for FMTTheme.
		@param[in] rhs the FMTTheme to copy.
		@return a reference to this FMTTheme.
		*/
		FMTTheme& operator = (const FMTTheme& rhs);
		// DocString: FMTTheme::isAttribute
		/**
		@brief Return true if the value is an attribute of the theme.
		@param[in] p_value the value to check.
		@return true if the value is an attribute else false.
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
		@brief Return true if the value is an aggregate of the theme.
		@param[in] p_value the value to check.
		@return true if the value is an aggregate else false.
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
		@brief Return true if the value is an index of the theme for a given attribute (yields section only).
		@param[in] p_attribute the attribute.
		@param[in] p_value the value to check.
		@return true if the value is an index else false.
		*/
		bool isIndex(const std::string& p_attribute, const std::string& p_value) const;
		// DocString: FMTTheme::isIndex
		/**
		@brief Return true if the value is an index of the theme (yields section only).
		@param[in] p_value the value to check.
		@return true if the value is an index else false.
		*/
		bool isIndex(const std::string& p_value) const;
		// DocString: FMTTheme::useIndex
		/**
		@brief Return true if the theme uses indexes.
		@return true if the theme uses indexes else false.
		*/
		bool useIndex() const;
		// DocString: FMTTheme::getIndex
		/**
		@brief Get the index value for a given attribute and index.
		@param[in] p_attribute the attribute.
		@param[in] p_value the index.
		@return the index value.
		*/
		double getIndex(const std::string& p_attribute, const std::string& p_value) const;
		// DocString: FMTTheme::inAggregate
		/**
		@brief Check if a value is part of an aggregate.
		@param[in] p_value the value to check.
		@param[in] p_aggregate the aggregate.
		@return true if the value is part of the aggregate else false.
		*/
		bool inAggregate(const std::string& p_value, const std::string& p_aggregate);
		// DocString: FMTTheme::isValid
		/**
		@brief Check if a value is a valid attribute, aggregate or question mark for the theme.
		@param[in] p_value the value to check.
		@return true if the value is valid else false.
		*/
		bool isValid(const std::string& p_value) const;
		// DocString: FMTTheme::operator==
		/**
		@brief Equality comparison operator of FMTTheme.
		@param[in] p_rhs the theme to compare with.
		@return true if both themes are equal else false.
		*/
		bool operator == (const FMTTheme& p_rhs) const;
		// DocString: FMTTheme::size
		/**
		@brief Return the size of the theme, 0 if it has no attribute.
		@return the size of the theme.
		*/
		inline size_t size() const
		{
			return m_attributes.size();
		}
		// DocString: FMTTheme::getStart
		/**
		@brief Return the bit location of the mask at which the theme starts.
		@return the start bit location.
		*/
		inline const size_t& getStart() const
		{
			return m_start;
		}
		// DocString: FMTTheme::getId
		/**
		@brief Return the id of the theme.
		@return the id of the theme.
		*/
		inline const size_t& getId() const
		{
			return m_id;
		}
		// DocString: FMTTheme::getName
		/**
		@brief Return the name of the theme.
		@return the name of the theme.
		*/
		inline std::string getName() const
		{
			return m_name;
		}
		// DocString: FMTTheme::empty
		/**
		@brief Return true if the theme has no attribute.
		@return true if the theme is empty else false.
		*/
		inline bool empty() const
		{
			return m_attributes.empty();
		}
		// DocString: FMTTheme::getAttributes
		/**
		@brief Get the attributes of an aggregate for the theme.
		@param[in] p_value the aggregate.
		@param[in] p_aggregate_source if true uses the aggregate source map.
		@return the attributes of the aggregate.
		*/
		std::vector<std::string>getAttributes(const std::string& p_value, bool p_aggregate_source = false) const;
		// DocString: FMTTheme::getBaseAttributes
		/**
		@brief Return a reference to the base attributes of the theme.
		@return a reference to the base attributes.
		*/
		inline const std::vector<std::string>& getBaseAttributes() const
		{
			return m_attributes;
		}
		// DocString: FMTTheme::getAttributeNames
		/**
		@brief Return the names of the attributes.
		@return the names of the attributes.
		*/
		const std::vector<std::string>& getAttributeNames() const
		{
			return m_attributenames;
		}
		// DocString: FMTTheme::presolve
		/**
		@brief Presolve the theme, removing unused attributes based on the filter and assigning a new id and start.
		@param[in,out] p_maskfilter the mask filter.
		@param[in,out] p_newid the new id, incremented if the presolved theme is not empty and has size greater than 1.
		@param[in,out] p_newstart the new start, incremented accordingly.
		@return the presolved theme.
		*/
		FMTTheme presolve(FMTMaskFilter& p_maskfilter, size_t& p_newid, size_t& p_newstart) const;
		// DocString: FMTTheme::updateFromMask
		/**
		@brief Update the theme with the aggregates in a global mask or with newly created aggregates and return the new attribute value.
		@param[in] p_globalmask the global mask.
		@return the new attribute value.
		*/
		std::string updateFromMask(const Core::FMTMask& p_globalmask);
		// DocString: FMTTheme::operator std::string
		/**
		@brief Return a string representation of the theme as seen in a landscape file.
		@return the string representation of the theme.
		*/
		operator std::string() const;
#if defined FMTWITHR
		// DocString:  FMTTheme::getaggregatesdataframe
		/**
		@brief Return a dataframe filled with the aggregates of each theme (THEME, ATTRIBUTES, AGGREGATES).
		@return the aggregates dataframe.
		*/
		Rcpp::DataFrame getAggregatesAsDataFrame() const;
		// DocString:  FMTTheme::getAttributesAsDataFrame
		/**
		@brief Return a dataframe filled with the attributes (ATTRIBUTE, NAMES).
		@return the attributes dataframe.
		*/
		Rcpp::DataFrame getAttributesAsDataFrame() const;
#endif
		// DocString: FMTTheme::validate
		/**
		@brief Validate the construction of a valid mask string based on the themes, trimming the mask if there are fewer themes than in the string.
		@param[in] p_themes the themes.
		@param[in,out] p_mask the mask string.
		@param[in] p_otherinformation optional additional information.
		@return true if the mask is valid else false.
		*/
		static bool validate(const std::vector<Core::FMTTheme>& p_themes,
			std::string& p_mask, std::string p_otherinformation = std::string());
		// DocString: FMTObject::theme
		/**
		@brief Validate the mask string for a vector of themes and throw an exception if something is wrong, called by validate.
		@param[in] p_themes the themes.
		@param[in] p_values the attribute values.
		@param[in,out] p_mask the mask string.
		@param[in] p_otherinformation additional information.
		@return true if the mask is valid else false.
		*/
		static bool checkMask(const std::vector<Core::FMTTheme>& p_themes,
			const std::vector<std::string>& p_values, std::string& p_mask,
			const std::string& p_otherinformation);
		// DocString: FMTObject::getAggregates
		/**
		@brief Return the aggregates of the theme.
		@return the aggregates of the theme.
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
		@brief Serialize the FMTTheme through its base FMTObject for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
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
		@brief Convert an attribute, aggregate or question mark to a bitset for the whole theme size.
		@param[in] p_value the value to convert.
		@return the bitset of the value.
		*/
		 boost::dynamic_bitset<uint8_t> strToBits(const std::string& p_value) const;
		// DocString: FMTTheme::_getCount
		/**
		@brief Get the number of bits set in the theme subset of a mask.
		@param[in] p_mask the mask.
		@return the number of bits set.
		*/
		size_t _getCount(const Core::FMTMask& p_mask) const;
		// DocString: FMTTheme::_getFlipCount
		/**
		@brief Get the number of bits unset in the theme subset of a mask.
		@param[in] p_mask the mask.
		@return the number of bits unset.
		*/
		size_t _getFlipCount(const Core::FMTMask& p_mask) const;
		// DocString: FMTTheme::_findFirstFlip
		/**
		@brief Get the position of the first bit unset in the theme subset of a mask.
		@param[in] p_mask the mask.
		@return the position of the first bit unset.
		*/
		size_t _findFirstFlip(const Core::FMTMask& p_mask) const;
		// DocString: FMTTheme::_findFirst
		/**
		@brief Get the position of the first bit set in the theme subset of a mask.
		@param[in] p_mask the mask.
		@return the position of the first bit set.
		*/
		size_t _findFirst(const Core::FMTMask& p_mask) const;
		// DocString: FMTTheme::_isEqual
		/**
		@brief Check if the theme subset of a mask is equal to given bits.
		@param[in] p_mask the mask.
		@param[in] p_bits the bits to check.
		@return true if equal else false.
		*/
		bool _isEqual(const Core::FMTMask& p_mask,
			const boost::dynamic_bitset<uint8_t>& p_bits) const;
		// DocString: FMTTheme::_isFlipEqual
		/**
		@brief Check if the theme subset of a mask is equal to the flipped given bits.
		@param[in] p_mask the mask.
		@param[in] p_bits the bits to check.
		@return true if equal else false.
		*/
		bool _isFlipEqual(const Core::FMTMask& p_mask,
			const boost::dynamic_bitset<uint8_t>& p_bits) const;
		// DocString: FMTTheme::bitsToStr
		/**
		@brief Convert a bitset to an attribute, aggregate or question mark for the whole theme size.
		@param[in] p_mask the mask.
		@return the string value.
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
		@brief Get the attribute iterator, raising if not found.
		@param[in] p_value the attribute value.
		@param[in] p_raiseifnotfound if true raises if the attribute is not found.
		@return the attribute iterator.
		*/
		lookiterator getAttribute(const std::string& p_value, bool p_raiseifnotfound = false) const;
		// DocString: FMTTheme::buildAttributeLocations
		/**
		@brief Build the attribute locations of the theme for optimization.
		*/
		void buildAttributeLocations();
		// DocString: FMTTheme::fillupAggregates
		/**
		@brief Fill up the aggregates vectors of the theme.
		@param[in,out] p_themeids the theme ids starting from 1.
		@param[in,out] p_locattributes the attributes.
		@param[in,out] p_locaggregates the aggregates.
		*/
		void fillupAggregates(std::vector<int>& p_themeids, std::vector<std::string>& p_locattributes, std::vector<std::string>& p_locaggregates) const;
		// DocString: FMTTheme::pushAggregate
		/**
		@brief Push a new aggregate to the theme.
		@param[in] p_aggregatename the name of the aggregate.
		*/
		void pushAggregate(const std::string& p_aggregatename);
		// DocString: FMTTheme::pushAggregate
		/**
		@brief Push a new value to an aggregate of the theme.
		@param[in] p_aggregatename the name of the aggregate.
		@param[in] p_value the value to push.
		*/
		void pushAggregateValue(const std::string& p_aggregatename, const std::string& p_value);
	
	
    };

	// DocString: FMTThemeComparator
	/**
	@brief Comparator used to check if two themes are the same.
	*/
	class FMTThemeComparator
	{
	public:
		// DocString: FMTThemeComparator(const FMTTheme&,const bool&)
		/**
		@brief Construct the comparator from the theme to match.
		@param[in] p_lbase_theme the theme to match.
		@param[in] p_lcomparedwithpresolved if true compares with the presolved theme.
		*/
		FMTThemeComparator(const FMTTheme& p_lbase_theme,const bool& p_lcomparedwithpresolved=false);
		// DocString: FMTthemecomparatorr::operator()(const FMTTheme&)
		/**
		@brief Matching test operator for the comparator.
		@param[in] p_theme the theme to test.
		@return true if the theme matches else false.
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
