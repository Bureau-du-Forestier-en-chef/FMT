/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSerie_H_INCLUDED
#define FMTSerie_H_INCLUDED

#include <string>
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
		FMTSerie() = default;
		// DocString: FMTSerie(const std::string&,int)
		/**
		 * @brief Initialize a FMTSerie using the string description and the length of the serie
		 * @param[in] p_serie the string of the serie
		 * @param[in] p_length the length of the serie in period.
		 */
		FMTSerie(const std::string& p_serie, int p_length);
		// DocString: FMTSerie::getLength
		/**
		 * @brief Return the length of the serie.
		 * @return Return the length of the serie.
		 */
		int getLength() const;
		// DocString: FMTSerie::getSerie
		/**
		 * @brief Return the string of the serie.
		 * @return Return the string of the serie.
		 */
		const std::string& getSerie() const;
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
	private:
		// DocString: FMTSerie::m_serie
		///The description of the serie in string
		std::string m_serie;
		// DocString: FMTSerie::m_length
		///The length of the serie using a int.
		int m_length;
    };

}

#endif // FMTSerie_H_INCLUDED
