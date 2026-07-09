/*
Copyright (c) 2019 Gouvernement du Qubec

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
	class FMTgraphvertextoyield;
}

namespace Parser
{
	class FMToutputparser;
}


namespace Core
{
	class FMTaction;
	class FMTyields;
	class FMTactualdevelopment;
	class FMTdevelopmentpath;
	class FMToperator;
class FMTEXPORT FMToutputsource : public FMTspec
    {
    public:
		FMToutputsource();
		~FMToutputsource() = default;
        FMToutputsource(const FMTotar ltarget,double lvalue = 0,
			std::string lyield= "", std::string laction = "",int origin=-1, int ttarget=-1);
        FMToutputsource(const FMTotar ltarget,
			std::vector<double>lvalues, int origin = -1, int ttarget=-1, std::string llevelname = "");
        FMToutputsource(const FMTspec& spec,const FMTmask& lmask,
               const FMTotar ltarget, std::string lyield= "",
               std::string laction = "", int origin = -1, int ttarget=-1);
        FMToutputsource(const FMToutputsource& rhs);
        FMToutputsource& operator = (const FMToutputsource& rhs);
        operator std::string() const override;
		bool operator == (const FMToutputsource& rhs) const;
		bool operator != (const FMToutputsource& rhs) const;
		bool operator < (const FMToutputsource& rhs) const;
		inline int getOutputOrigin() const
			{
			return outputorigin;
			}
		inline int getThemeTarget() const
			{
			return themetarget;
			}
		bool isInAggregate(const FMToutputsource& rhs, const std::vector<Core::FMTaction>& actions) const;
		bool isSubsetOf(const FMToutputsource& rhs, const std::vector<Core::FMTaction>& actions) const;
		bool isSubsetOf(const FMToutputsource& rhs) const;
		bool isSameButDifferentAction(const FMToutputsource& rhs) const;
		inline const FMTmask& getmask() const
			{
			return mask;
			}
		void setMask(const FMTmask& newmask);
		void setAction(const std::string& actionname);
		void setTarget(const FMTotar& newtype);
		void setThemeTarget(const int& newttarget);
		void setOutputOrigin(const int& neworigin);
		void resetValues(const FMToperator& op,const FMToutputsource& other);
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
		bool isNull(const FMTyields& ylds) const;
		inline bool isTimeYield() const
			{
			return (target == FMTotar::timeyld);
			}
		std::vector<const FMTaction*>targets(const std::vector<FMTaction>& actions) const;
		std::unordered_set<int>targetsSet(const std::vector<FMTaction>& actions) const;
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
		double getCoef(const FMTdevelopment& development,
			const FMTyields& yields,
			const Graph::FMTgraphvertextoyield* graphinfo =nullptr,
			std::vector<FMTdevelopmentpath> const * paths=nullptr,
			 FMTaction const * action=nullptr) const;
		bool use(const FMTdevelopment& development, const FMTyields& ylds,
			const Graph::FMTgraphvertextoyield* graphinfo = nullptr) const;
		FMToutputsource presolve(const FMTmaskfilter& filter,const std::vector<FMTtheme>& newthemes) const;
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
		double getConstantValue(const std::vector<Core::FMTactualdevelopment>& area,
								const std::vector<Core::FMTaction>&actions, const FMTyields& yields) const;
		size_t hash(int period = -1,bool withyield = false) const;
		void fillHashMask(Core::FMTmask& basemask) const;
		void fillHashSpec(Core::FMTmask& basemask) const;
		bool isEqualByValue(const FMToutputsource& rhs) const;
		inline bool isAction() const
			{
			return !action.empty();
			}
		// DocString: FMToutputsource::trimDouble
		/**
		* @brief Trim en string les nombres a virgule.
		*
		* Cette fonction supprime le superflus des nombre en string par ex: 1400.0000 en 1400 ou 14.0300 en 14.03
		*
		* @param number Sous format de string&

		* @return Retourne la valeur trimm en std::string
		*/
		static std::string trimDouble(const std::string& string_number);
		// DocString: FMToutputsource::setYield
		/**
		@brief set Yield to the output source.
		@param[in] p_Yield the yield we want to set.
		*/
		void setYield(const std::string& p_Yield);
	private:
		friend class boost::serialization::access;
		friend class Parser::FMToutputparser;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTspec>(*this));
			ar& BOOST_SERIALIZATION_NVP(mask);
			ar& BOOST_SERIALIZATION_NVP(target);
			ar& BOOST_SERIALIZATION_NVP(action);
			ar& BOOST_SERIALIZATION_NVP(yield);
			ar& BOOST_SERIALIZATION_NVP(values);
			ar& BOOST_SERIALIZATION_NVP(themetarget);
		}
		FMTmask mask;
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

class FMToutputsourcecomparator
	{
		bool variable;
	public:
		FMToutputsourcecomparator(bool lvariable);
		bool operator()(const FMToutputsource& source) const;

	};
}

namespace boost {
	/**
	boost hash function for FMToutputsource
	*/
	template <>
	struct hash<Core::FMToutputsource>
	{
		std::size_t operator()(const Core::FMToutputsource& p_source) const
		{
			return p_source.hash(-1, true);
		}
	};


}



#endif // FMTOSR_Hm_included
