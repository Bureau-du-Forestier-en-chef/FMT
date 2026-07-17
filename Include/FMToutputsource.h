/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTOSR_Hm_included
#define FMTOSR_Hm_included

#include "FMTbounds.hpp"
#include "FMTmask.h"
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <string>
#include "FMTutility.h"
#include <unordered_set>
#include "FMTdevelopment.h"


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
class FMTEXPORT FMTOutputSource : public FMTSpec
    {
    public:
		FMTOutputSource();
		~FMTOutputSource() = default;
        FMTOutputSource(const FMTotar ltarget,double lvalue = 0,
			std::string lyield= "", std::string laction = "",int origin=-1, int ttarget=-1);
        FMTOutputSource(const FMTotar ltarget,
			std::vector<double>lvalues, int origin = -1, int ttarget=-1, std::string llevelname = "");
        FMTOutputSource(const FMTSpec& spec,const FMTMask& lmask,
               const FMTotar ltarget, std::string lyield= "",
               std::string laction = "", int origin = -1, int ttarget=-1);
        FMTOutputSource(const FMTOutputSource& rhs);
        FMTOutputSource& operator = (const FMTOutputSource& rhs);
        operator std::string() const override;
		bool operator == (const FMTOutputSource& rhs) const;
		bool operator != (const FMTOutputSource& rhs) const;
		bool operator < (const FMTOutputSource& rhs) const;
		inline int getOutputOrigin() const
			{
			return outputorigin;
			}
		inline int getThemeTarget() const
			{
			return themetarget;
			}
		bool isInAggregate(const FMTOutputSource& rhs, const std::vector<Core::FMTAction>& actions) const;
		bool isSubsetOf(const FMTOutputSource& rhs, const std::vector<Core::FMTAction>& actions) const;
		bool isSubsetOf(const FMTOutputSource& rhs) const;
		bool isSameButDifferentAction(const FMTOutputSource& rhs) const;
		inline const FMTMask& getMask() const
			{
			return mask;
			}
		void setMask(const FMTMask& newmask);
		void setAction(const std::string& actionname);
		void setTarget(const FMTotar& newtype);
		void setThemeTarget(const int& newttarget);
		void setOutputOrigin(const int& neworigin);
		void resetValues(const FMTOperator& op,const FMTOutputSource& other);
		inline const std::string& getAction() const
			{
			return action;
			}
		inline const std::string& getYield() const
			{
			return yield;
			}
		inline const FMTotar& getTarget() const
			{
			return target;
			}
		double getValue(int period = 0) const;
		inline bool isVariable() const
			{
			return bool(mask);
			}
		inline bool isLevel() const
			{
			return (target == FMTotar::level);
			}
		inline bool isConstant() const
			{
			return (target == FMTotar::val);
			}
		inline bool isVariableLevel() const
			{
			return (!action.empty() && isLevel());
			}
		inline const std::string& getLevel() const
			{
			return yield;
			}
		inline const std::vector<double>& getValues() const
		{
			return values;
		}
		bool isNull(const FMTYields& ylds) const;
		inline bool isTimeYield() const
			{
			return (target == FMTotar::timeyld);
			}
		std::vector<const FMTAction*>targets(const std::vector<FMTAction>& actions) const;
		std::unordered_set<int>targetsSet(const std::vector<FMTAction>& actions) const;
		inline bool isInventory() const
			{
			return (target == FMTotar::inventory);
			}
		inline bool useInEdges() const
			{
			return (target == FMTotar::inventory);
			}
		inline bool isNextPeriod() const
			{
			return (target == FMTotar::inventory && action.empty());
			}
		inline bool useOutEdges() const
			{
			return (target == FMTotar::actual);
			}
		double getCoef(const FMTDevelopment& development,
			const FMTYields& yields,
			const Graph::FMTGraphVertexToYield* graphinfo =nullptr,
			std::vector<FMTDevelopmentPath> const * paths=nullptr,
			 FMTAction const * action=nullptr) const;
		bool use(const FMTDevelopment& development, const FMTYields& ylds,
			const Graph::FMTGraphVertexToYield* graphinfo = nullptr) const;
		FMTOutputSource presolve(const FMTMaskFilter& filter,const std::vector<FMTTheme>& newthemes) const;
		void setAverage();
		inline bool isAverage() const
			{
			return average;
			}
		void setSum();
		inline bool isSum() const
		{
			return sum;
		}
		bool canBeDeducedToConstant() const;
		double getConstantValue(const std::vector<Core::FMTActualDevelopment>& area,
								const std::vector<Core::FMTAction>&actions, const FMTYields& yields) const;
		size_t hash(int period = -1,bool withyield = false) const;
		void fillHashMask(Core::FMTMask& baseMask) const;
		void fillHashSpec(Core::FMTMask& baseMask) const;
		bool isEqualByValue(const FMTOutputSource& rhs) const;
		inline bool isAction() const
			{
			return !action.empty();
			}
		// DocString: FMTOutputSource::trimDouble
		/**
		* @brief Trim en string les nombres a virgule.
		*
		* Cette fonction supprime le superflus des nombre en string par ex: 1400.0000 en 1400 ou 14.0300 en 14.03
		*
		* @param number Sous format de string&

		* @return Retourne la valeur trimmé en std::string
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
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTSpec>(*this));
			ar& BOOST_SERIALIZATION_NVP(mask);
			ar& BOOST_SERIALIZATION_NVP(target);
			ar& BOOST_SERIALIZATION_NVP(action);
			ar& BOOST_SERIALIZATION_NVP(yield);
			ar& BOOST_SERIALIZATION_NVP(values);
			ar& BOOST_SERIALIZATION_NVP(themetarget);
		}
		FMTMask mask;
		FMTotar target;
		std::string action, yield, levelname;//levelname is only for constant level...(for parsing)
		std::vector<double>values;
		bool average;
		bool sum;
		int outputorigin;
		int themetarget;
		//target =-1 or themetarget constructor
		void pushValues(const std::vector<double>& newvalues);
    };

class FMTOutputSourceComparator
	{
		bool variable;
	public:
		FMTOutputSourceComparator(bool lvariable);
		bool operator()(const FMTOutputSource& source) const;

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
