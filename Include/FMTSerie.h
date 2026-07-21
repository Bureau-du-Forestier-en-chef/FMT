/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSerie_Hm_included
#define FMTSerie_Hm_included

#include <string>
#include <vector>
#include "FMTutility.h"


namespace Core
{
// DocString: FMTSerie
/**
@brief Class keeping the information of a serie of actions in a given order.
*/
class FMTEXPORT FMTSerie
    {
	public:
		// DocString: FMTSerie()
		/**
		@brief Default constructor for FMTSerie.
		*/
		FMTSerie();
		// DocString: FMTSerie(const std::string&, int)
		/**
		@brief Initialize a serie from a string description.
		@param[in] p_serie the serie string.
		@param[in] p_length the length of the serie in periods.
		*/
		FMTSerie(const std::string& p_serie, int p_length);
		// DocString: FMTSerie(const std::vector<std::string>&, int)
		/**
		@brief Initialize a serie from a vector of strings.
		@param[in] p_serie the serie strings.
		@param[in] p_length the length of the serie in periods.
		*/
		FMTSerie(const std::vector<std::string>& p_serie, int p_length);
		// DocString: FMTSerie(const std::vector<std::string>&, bool, bool)
		/**
		@brief Initialize a serie from a vector of strings with ASAP and ALAP flags.
		@param[in] p_serie the serie strings.
		@param[in] p_asap if true the serie is as soon as possible.
		@param[in] p_alap if true the serie is as late as possible.
		*/
		FMTSerie(const std::vector<std::string>& p_serie, 
							bool p_asap, bool p_alap);
		// DocString: FMTSerie::getLength
		/**
		@brief Return the length of the serie.
		@return the length of the serie.
		*/
		int getLength() const;
		// DocString: FMTSerie::getActions
		/**
		@brief Return the actions of the serie.
		@return the actions of the serie.
		*/
		const std::vector<std::string>& getActions() const;
		// DocString: FMTSerie::getSerie
		/**
		@brief Return the string of the serie.
		@return the string of the serie.
		*/
		const std::string getSerie() const;
		// DocString: FMTSerie::operator std::string
		/**
		@brief return in string format
		@return string formated with _ASAP _ALAP
		*/
		operator std::string() const;
		// DocString: FMTSerie::operator==
		/**
		@brief Equality comparison operator of FMTSerie.
		@param[in] p_RHS the serie to compare with.
		@return true if both series are equal else false.
		*/
		bool operator == (const FMTSerie& p_RHS) const;
		// DocString: FMTSerie::operator!=
		/**
		@brief Inequality comparison operator of FMTSerie.
		@param[in] p_RHS the serie to compare with.
		@return true if both series are different else false.
		*/
		bool operator != (const FMTSerie& p_RHS) const;
		// DocString: FMTSerie::operator<
		/**
		@brief Less than comparison operator of FMTSerie.
		@param[in] p_RHS the serie to compare with.
		@return true if this serie is less than p_RHS else false.
		*/
		bool operator < (const FMTSerie & p_RHS) const;
		// DocString: FMTSerie::operator>
		/**
		@brief Greater than comparison operator of FMTSerie.
		@param[in] p_RHS the serie to compare with.
		@return true if this serie is greater than p_RHS else false.
		*/
		bool operator > (const FMTSerie& p_RHS) const;
		// DocString: FMTSerie::isASAP
		/**
		@brief Return true if the serie is as soon as possible.
		@return true if the serie is as soon as possible else false.
		*/
		bool isASAP() const;
		// DocString: FMTSerie::isALAP
		/**
		@brief Return true if the serie is as late as possible.
		@return true if the serie is as late as possible else false.
		*/
		bool isALAP() const;
		// DocString: FMTSerie::getLength
		/**
		@brief Return true if the given actions are allowed in the serie.
		@param[in] p_SerieMask the serie mask.
		@return true if the actions are allowed in the serie else false.
		*/
		bool isAllowedInSerie(const std::vector<std::string>& p_SerieMask) const;
		// DocString: FMTSerie::isEmpty
		/**
		@brief Return true if the serie is empty.
		@return true if the serie is empty else false.
		*/
		bool isEmpty() const;
	private:
		// DocString: FMTSerie::m_serie
		///The description of the serie in string
		std::vector<std::string> m_serie;
		// DocString: FMTSerie::m_length
		///The length of the serie using a int.
		int m_length;
		// DocString: FMTSerie::m_SEPERATOR
		///string seperator
		static const std::string m_SEPERATOR;

    };

}

#endif // FMTSerie_Hm_included
