/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
	


#ifndef FMTSEEXCEPTION_Hm_included
#define FMTSEEXCEPTION_Hm_included

#include <stdexcept>





/// Namespace for exceptions and exceptions handling (warnings and errors) thrown by FMT and all exceptions handlers available.
namespace Exception
    {
    class FMTSeException : public std::logic_error
        {
        public:
            FMTSeException() noexcept;
            FMTSeException(unsigned int p_n) noexcept;
            unsigned int getSeNumber() const noexcept;
            FMTSeException(const FMTSeException& p_rhs) = default;
            FMTSeException& operator = (const FMTSeException& p_rhs) = default;
        private:
            unsigned int m_nSE;

        };
    }


#endif // FMTEXCEPTION_Hm_included
