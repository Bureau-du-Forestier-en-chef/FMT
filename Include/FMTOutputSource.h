/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTOSR_Hm_included
#define FMTOSR_Hm_included

#include "FMTBounds.hpp"
#include "FMTMask.h"
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <string>
#include "FMTutility.h"
#include <unordered_set>
#include "FMTDevelopment.h"


namespace Graph
{
	class FMTGraphVertexToYield;
}

namespace Parser
{
	class FMTOutputParser;
}


namespace Core
{
	class FMTAction;
	class FMTYields;
	class FMTActualDevelopment;
	class FMTDevelopmentPath;
	class FMTOperator;
// DocString: FMTOutputSource
/**
@brief Source of an output holding a mask, a target and values, used to compute an output.
*/
class FMTEXPORT FMTOutputSource : public FMTSpec
    {
    public:
		// DocString: FMTOutputSource()
		/**
		@brief Default constructor for FMTOutputSource.
		*/
		FMTOutputSource();
		// DocString: ~FMTOutputSource()
		/**
		@brief Default destructor for FMTOutputSource.
		*/
		~FMTOutputSource() = default;
        // DocString: FMTOutputSource(const FMTotar,double,std::string,std::string,int,int)
        /**
        @brief Construct an output source from a target and optional value, yield, action, origin and theme target.
        @param[in] ltarget the output target.
        @param[in] lvalue the value.
        @param[in] lyield the yield.
        @param[in] laction the action.
        @param[in] origin the output origin.
        @param[in] ttarget the theme target.
        */
        FMTOutputSource(const FMTotar ltarget,double lvalue = 0,
			std::string lyield= "", std::string laction = "",int origin=-1, int ttarget=-1);
        // DocString: FMTOutputSource(const FMTotar,std::vector<double>,int,int,std::string)
        /**
        @brief Construct an output source from a target and a vector of values.
        @param[in] ltarget the output target.
        @param[in] lvalues the values.
        @param[in] origin the output origin.
        @param[in] ttarget the theme target.
        @param[in] llevelname the level name.
        */
        FMTOutputSource(const FMTotar ltarget,
			std::vector<double>lvalues, int origin = -1, int ttarget=-1, std::string llevelname = "");
        // DocString: FMTOutputSource(const FMTSpec&,const FMTMask&,const FMTotar,std::string,std::string,int,int)
        /**
        @brief Construct an output source from a specification, a mask and a target.
        @param[in] spec the specification.
        @param[in] lmask the mask.
        @param[in] ltarget the output target.
        @param[in] lyield the yield.
        @param[in] laction the action.
        @param[in] origin the output origin.
        @param[in] ttarget the theme target.
        */
        FMTOutputSource(const FMTSpec& spec,const FMTMask& lmask,
               const FMTotar ltarget, std::string lyield= "",
               std::string laction = "", int origin = -1, int ttarget=-1);
        // DocString: FMTOutputSource(const FMTOutputSource&)
        /**
        @brief Copy constructor for FMTOutputSource.
        @param[in] rhs the output source to copy.
        */
        FMTOutputSource(const FMTOutputSource& rhs);
        // DocString: FMTOutputSource::operator=
        /**
        @brief Copy assignment operator for FMTOutputSource.
        @param[in] rhs the output source to copy.
        @return a reference to this FMTOutputSource.
        */
        FMTOutputSource& operator = (const FMTOutputSource& rhs);
        // DocString: FMTOutputSource::operator std::string
        /**
        @brief Return the string representation of the output source.
        @return the string representation of the output source.
        */
        operator std::string() const override;
		// DocString: FMTOutputSource::operator==
		/**
		@brief Equality comparison operator of FMTOutputSource.
		@param[in] rhs the output source to compare with.
		@return true if both output sources are equal else false.
		*/
		bool operator == (const FMTOutputSource& rhs) const;
		// DocString: FMTOutputSource::operator!=
		/**
		@brief Inequality comparison operator of FMTOutputSource.
		@param[in] rhs the output source to compare with.
		@return true if both output sources are different else false.
		*/
		bool operator != (const FMTOutputSource& rhs) const;
		// DocString: FMTOutputSource::operator<
		/**
		@brief Less than comparison operator of FMTOutputSource.
		@param[in] rhs the output source to compare with.
		@return true if this output source is less than rhs else false.
		*/
		bool operator < (const FMTOutputSource& rhs) const;
		// DocString: FMTOutputSource::getOutputOrigin
		/**
		@brief Return the output origin.
		@return the output origin.
		*/
		inline int getOutputOrigin() const
			{
			return m_outputorigin;
			}
		// DocString: FMTOutputSource::getThemeTarget
		/**
		@brief Return the theme target.
		@return the theme target.
		*/
		inline int getThemeTarget() const
			{
			return m_themetarget;
			}
		// DocString: FMTOutputSource::isInAggregate
		/**
		@brief Return true if this output source is in the same aggregate as another.
		@param[in] rhs the other output source.
		@param[in] actions the actions.
		@return true if in the same aggregate else false.
		*/
		bool isInAggregate(const FMTOutputSource& rhs, const std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTOutputSource::isSubsetOf(const FMTOutputSource&, const std::vector<Core::FMTAction>&)
		/**
		@brief Return true if this output source is a subset of another considering the actions.
		@param[in] rhs the other output source.
		@param[in] actions the actions.
		@return true if this output source is a subset of rhs else false.
		*/
		bool isSubsetOf(const FMTOutputSource& rhs, const std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTOutputSource::isSubsetOf(const FMTOutputSource&)
		/**
		@brief Return true if this output source is a subset of another.
		@param[in] rhs the other output source.
		@return true if this output source is a subset of rhs else false.
		*/
		bool isSubsetOf(const FMTOutputSource& rhs) const;
		// DocString: FMTOutputSource::isSameButDifferentAction
		/**
		@brief Return true if this output source is the same as another but with a different action.
		@param[in] rhs the other output source.
		@return true if same but different action else false.
		*/
		bool isSameButDifferentAction(const FMTOutputSource& rhs) const;
		// DocString: FMTOutputSource::getMask
		/**
		@brief Return a const reference to the mask of the output source.
		@return a const reference to the mask.
		*/
		inline const FMTMask& getMask() const
			{
			return m_mask;
			}
		// DocString: FMTOutputSource::setMask
		/**
		@brief Set the mask of the output source.
		@param[in] newmask the mask to set.
		*/
		void setMask(const FMTMask& newmask);
		// DocString: FMTOutputSource::setAction
		/**
		@brief Set the action of the output source.
		@param[in] actionname the action to set.
		*/
		void setAction(const std::string& actionname);
		// DocString: FMTOutputSource::setTarget
		/**
		@brief Set the target of the output source.
		@param[in] newtype the target to set.
		*/
		void setTarget(const FMTotar& newtype);
		// DocString: FMTOutputSource::setThemeTarget
		/**
		@brief Set the theme target of the output source.
		@param[in] newttarget the theme target to set.
		*/
		void setThemeTarget(const int& newttarget);
		// DocString: FMTOutputSource::setOutputOrigin
		/**
		@brief Set the output origin of the output source.
		@param[in] neworigin the output origin to set.
		*/
		void setOutputOrigin(const int& neworigin);
		// DocString: FMTOutputSource::resetValues
		/**
		@brief Reset the values of the output source using an operator and another output source.
		@param[in] op the operator.
		@param[in] other the other output source.
		*/
		void resetValues(const FMTOperator& op,const FMTOutputSource& other);
		// DocString: FMTOutputSource::getAction
		/**
		@brief Return a const reference to the action of the output source.
		@return a const reference to the action.
		*/
		inline const std::string& getAction() const
			{
			return m_action;
			}
		// DocString: FMTOutputSource::getYield
		/**
		@brief Return a const reference to the yield of the output source.
		@return a const reference to the yield.
		*/
		inline const std::string& getYield() const
			{
			return m_yield;
			}
		// DocString: FMTOutputSource::getTarget
		/**
		@brief Return a const reference to the target of the output source.
		@return a const reference to the target.
		*/
		inline const FMTotar& getTarget() const
			{
			return m_target;
			}
		// DocString: FMTOutputSource::getValue
		/**
		@brief Return the value of the output source for a given period.
		@param[in] period the period.
		@return the value.
		*/
		double getValue(int period = 0) const;
		// DocString: FMTOutputSource::isVariable
		/**
		@brief Return true if the output source is a variable.
		@return true if the output source is a variable else false.
		*/
		inline bool isVariable() const
			{
			return bool(m_mask);
			}
		// DocString: FMTOutputSource::isLevel
		/**
		@brief Return true if the output source is a level.
		@return true if the output source is a level else false.
		*/
		inline bool isLevel() const
			{
			return (m_target == FMTotar::level);
			}
		// DocString: FMTOutputSource::isConstant
		/**
		@brief Return true if the output source is a constant.
		@return true if the output source is a constant else false.
		*/
		inline bool isConstant() const
			{
			return (m_target == FMTotar::val);
			}
		// DocString: FMTOutputSource::isVariableLevel
		/**
		@brief Return true if the output source is a variable level.
		@return true if the output source is a variable level else false.
		*/
		inline bool isVariableLevel() const
			{
			return (!m_action.empty() && isLevel());
			}
		// DocString: FMTOutputSource::getLevel
		/**
		@brief Return a const reference to the level of the output source.
		@return a const reference to the level.
		*/
		inline const std::string& getLevel() const
			{
			return m_yield;
			}
		// DocString: FMTOutputSource::getValues
		/**
		@brief Return a const reference to the values of the output source.
		@return a const reference to the values.
		*/
		inline const std::vector<double>& getValues() const
		{
			return m_values;
		}
		// DocString: FMTOutputSource::isNull
		/**
		@brief Return true if the output source is null.
		@param[in] ylds the yields.
		@return true if the output source is null else false.
		*/
		bool isNull(const FMTYields& ylds) const;
		// DocString: FMTOutputSource::isTimeYield
		/**
		@brief Return true if the output source is a time yield.
		@return true if the output source is a time yield else false.
		*/
		inline bool isTimeYield() const
			{
			return (m_target == FMTotar::timeyld);
			}
		// DocString: FMTOutputSource::targets
		/**
		@brief Return the actions targeted by the output source.
		@param[in] actions the actions.
		@return the targeted actions.
		*/
		std::vector<const FMTAction*>targets(const std::vector<FMTAction>& actions) const;
		// DocString: FMTOutputSource::targetsSet
		/**
		@brief Return the ids of the actions targeted by the output source.
		@param[in] actions the actions.
		@return the ids of the targeted actions.
		*/
		std::unordered_set<int>targetsSet(const std::vector<FMTAction>& actions) const;
		// DocString: FMTOutputSource::isInventory
		/**
		@brief Return true if the output source targets the inventory.
		@return true if the output source targets the inventory else false.
		*/
		inline bool isInventory() const
			{
			return (m_target == FMTotar::inventory);
			}
		// DocString: FMTOutputSource::useInEdges
		/**
		@brief Return true if the output source uses the in edges.
		@return true if the output source uses the in edges else false.
		*/
		inline bool useInEdges() const
			{
			return (m_target == FMTotar::inventory);
			}
		// DocString: FMTOutputSource::isNextPeriod
		/**
		@brief Return true if the output source targets the next period.
		@return true if the output source targets the next period else false.
		*/
		inline bool isNextPeriod() const
			{
			return (m_target == FMTotar::inventory && m_action.empty());
			}
		// DocString: FMTOutputSource::useOutEdges
		/**
		@brief Return true if the output source uses the out edges.
		@return true if the output source uses the out edges else false.
		*/
		inline bool useOutEdges() const
			{
			return (m_target == FMTotar::actual);
			}
		// DocString: FMTOutputSource::getCoef
		/**
		@brief Return the coefficient of the output source for a development based on the yields.
		@param[in] development the development.
		@param[in] yields the yields.
		@param[in] graphinfo an optional graph vertex to yield.
		@param[in] paths optional development paths.
		@param[in] p_action an optional action.
		@return the coefficient.
		*/
		double getCoef(const FMTDevelopment& development,
			const FMTYields& yields,
			const Graph::FMTGraphVertexToYield* graphinfo =nullptr,
			std::vector<FMTDevelopmentPath> const * paths=nullptr,
			 FMTAction const * p_action=nullptr) const;
		// DocString: FMTOutputSource::use
		/**
		@brief Return true if the output source is used by a development based on the yields.
		@param[in] development the development.
		@param[in] ylds the yields.
		@param[in] graphinfo an optional graph vertex to yield.
		@return true if the output source is used else false.
		*/
		bool use(const FMTDevelopment& development, const FMTYields& ylds,
			const Graph::FMTGraphVertexToYield* graphinfo = nullptr) const;
		// DocString: FMTOutputSource::presolve
		/**
		@brief Return a presolved copy of the output source.
		@param[in] filter the mask filter.
		@param[in] newthemes the presolved themes.
		@return the presolved output source.
		*/
		FMTOutputSource presolve(const FMTMaskFilter& filter,const std::vector<FMTTheme>& newthemes) const;
		// DocString: FMTOutputSource::setAverage
		/**
		@brief Set the output source as an average.
		*/
		void setAverage();
		// DocString: FMTOutputSource::isAverage
		/**
		@brief Return true if the output source is an average.
		@return true if the output source is an average else false.
		*/
		inline bool isAverage() const
			{
			return m_average;
			}
		// DocString: FMTOutputSource::setSum
		/**
		@brief Set the output source as a sum.
		*/
		void setSum();
		// DocString: FMTOutputSource::isSum
		/**
		@brief Return true if the output source is a sum.
		@return true if the output source is a sum else false.
		*/
		inline bool isSum() const
		{
			return m_sum;
		}
		// DocString: FMTOutputSource::canBeDeducedToConstant
		/**
		@brief Return true if the output source can be deduced to a constant.
		@return true if the output source can be deduced to a constant else false.
		*/
		bool canBeDeducedToConstant() const;
		// DocString: FMTOutputSource::getConstantValue
		/**
		@brief Return the constant value of the output source.
		@param[in] area the actual developments.
		@param[in] actions the actions.
		@param[in] yields the yields.
		@return the constant value.
		*/
		double getConstantValue(const std::vector<Core::FMTActualDevelopment>& area,
								const std::vector<Core::FMTAction>&actions, const FMTYields& yields) const;
		// DocString: FMTOutputSource::hash
		/**
		@brief Return the hash of the output source.
		@param[in] period the period.
		@param[in] withyield if true includes the yield in the hash.
		@return the hash value.
		*/
		size_t hash(int period = -1,bool withyield = false) const;
		// DocString: FMTOutputSource::fillHashMask
		/**
		@brief Fill a base mask with the hash mask of the output source.
		@param[in,out] baseMask the base mask to fill.
		*/
		void fillHashMask(Core::FMTMask& baseMask) const;
		// DocString: FMTOutputSource::fillHashSpec
		/**
		@brief Fill a base mask with the hash specification of the output source.
		@param[in,out] baseMask the base mask to fill.
		*/
		void fillHashSpec(Core::FMTMask& baseMask) const;
		// DocString: FMTOutputSource::isEqualByValue
		/**
		@brief Return true if this output source is equal by value to another.
		@param[in] rhs the other output source.
		@return true if equal by value else false.
		*/
		bool isEqualByValue(const FMTOutputSource& rhs) const;
		// DocString: FMTOutputSource::isAction
		/**
		@brief Return true if the output source is an action.
		@return true if the output source is an action else false.
		*/
		inline bool isAction() const
			{
			return !m_action.empty();
			}
		// DocString: FMTOutputSource::trimDouble
		/**
		@brief Trim the trailing zeros of a number in string format, for example 1400.0000 to 1400 or 14.0300 to 14.03.
		@param[in] string_number the number in string format.
		@return the trimmed value in string format.
		*/
		static std::string trimDouble(const std::string& string_number);
		// DocString: FMTOutputSource::setYield
		/**
		@brief set Yield to the output source.
		@param[in] p_Yield the yield we want to set.
		*/
		void setYield(const std::string& p_Yield);
	private:
		friend class boost::serialization::access;
		friend class Parser::FMTOutputParser;
		// DocString: FMTOutputSource::serialize
		/**
		@brief Serialize the FMTOutputSource through its base FMTSpec for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTSpec>(*this));
			ar& boost::serialization::make_nvp("mask", m_mask);
			ar& boost::serialization::make_nvp("target", m_target);
			ar& boost::serialization::make_nvp("action", m_action);
			ar& boost::serialization::make_nvp("yield", m_yield);
			ar& boost::serialization::make_nvp("values", m_values);
			ar& boost::serialization::make_nvp("themetarget", m_themetarget);
		}
		FMTMask m_mask;
		FMTotar m_target;
		std::string m_action, m_yield, m_levelname;//m_levelname is only for constant level...(for parsing)
		std::vector<double>m_values;
		bool m_average;
		bool m_sum;
		int m_outputorigin;
		int m_themetarget;
		//target =-1 or themetarget constructor
		// DocString: FMTOutputSource::_pushValues
		/**
		@brief Push new values to the output source.
		@param[in] newvalues the values to push.
		*/
		void _pushValues(const std::vector<double>& newvalues);
    };

// DocString: FMTOutputSourceComparator
/**
@brief Comparator used to match output sources by whether they are variable.
*/
class FMTOutputSourceComparator
	{
	public:
		// DocString: FMTOutputSourceComparator(bool)
		/**
		@brief Construct the comparator.
		@param[in] lvariable if true matches variable output sources.
		*/
		FMTOutputSourceComparator(bool lvariable);
		// DocString: FMTOutputSourceComparator::operator()
		/**
		@brief Matching test operator for the comparator.
		@param[in] source the output source to test.
		@return true if the output source matches else false.
		*/
		bool operator()(const FMTOutputSource& source) const;

	private:
		bool m_variable;
	};
}

namespace boost {
	/**
	boost hash function for FMTOutputSource
	*/
	template <>
	struct hash<Core::FMTOutputSource>
	{
		std::size_t operator()(const Core::FMTOutputSource& p_source) const
		{
			return p_source.hash(-1, true);
		}
	};


}



#endif // FMTOSR_Hm_included
