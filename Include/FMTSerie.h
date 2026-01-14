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
@class FMTSerie
@brief The class keep information of a serie of actions in a given order.
*/
class FMTEXPORT FMTSerie
    {
	public:
		FMTSerie();
		// DocString: FMTSerie()
		/**
		 * @brief Initialize a FMTSerie using the string description of the serie
		 * @param[in] p_serie the vector string of the serie
		 * @param[in] p_length the length of the serie in period.
		 */
		FMTSerie(const std::vector<std::string>& p_serie, int p_length);
		// DocString: FMTSerie()
		/**
		 * @brief Initialize a FMTSerie using the string description of the serie
		 * @param[in] p_asap
		 * @param[in] p_alap
		 */
		FMTSerie(const std::vector<std::string>& p_serie, 
							bool p_asap, bool p_alap);
		// DocString: FMTSerie::getLength
		/**
		 * @brief Return the length of the serie.
		 * @return Return the length of the serie.
		 */
		int getLength() const;
		// DocString: FMTSerie::getActions
		/**
		 * @brief Return the actions of the serie
		 * @return the actions
		 */
		const std::vector<std::string>& getActions() const;
		// DocString: FMTSerie::getSerie
		/**
		 * @brief Return the string of the serie.
		 * @return Return the string of the serie.
		 */
		const std::string getSerie() const;
		// DocString:FMTSerie::operator std::string
		/**
		@brief return in string format
		@return string formated with _ASAP _ALAP
		*/
		operator std::string() const;
		// DocString: FMTSerie::operator==
		/**
		 * @brief Check if two series are equal.
		 * @return true if 2 series are equal.
		 */
		bool operator == (const FMTSerie& p_RHS) const;
		// DocString: FMTSerie::operator!=
		/**
		 * @brief Check if two series are not equal.
		 * @return true if 2 series are nor equal.
		 */
		bool operator != (const FMTSerie& p_RHS) const;
		// DocString: FMTSerie::operator<
		/**
		 * @brief Check if this serie is less then the p_RHS serie.
		 * @return true this serie is less then the p_RHS serie.
		 */
		bool operator < (const FMTSerie & p_RHS) const;
		// DocString: FMTSerie::operator>
		/**
		 * @brief Check if this serie is greater then the p_RHS serie.
		 * @return true this serie is less then the p_RHS serie.
		 */
		bool operator > (const FMTSerie& p_RHS) const;
		// DocString: FMTSerie::isASAP
		/**
		 * @brief return true if asap
		 * @return return true if asap
		 */
		bool isASAP() const;
		// DocString: FMTSerie::isALAP
		/**
		 * @brief return true if alap
		 * @return return true if alap
		 */
		bool isALAP() const;
		// DocString: FMTSerie::getLength
		/**
		 * @brief test if you allow those actions in series
		 * @return true if in serie
		 */
		bool isAllowedInSerie(const std::vector<std::string>& p_SerieMask) const;
		// DocString: FMTSerie::isEmpty
		/**
		 * @brief return true if empty
		 * @return return true if empty
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
