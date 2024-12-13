/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
	


#ifndef FMTSEEXCEPTION_H_INCLUDED
#define FMTSEEXCEPTION_H_INCLUDED

#include <stdexcept>





/// Namespace for exceptions and exceptions handling (warnings and errors) thrown by FMT and all exceptions handlers available.
namespace Exception
    {
    class FMTseException : public std::logic_error
        {
        public:
            FMTseException() noexcept;
            FMTseException(unsigned int p_n) noexcept;
            unsigned int getSeNumber() const noexcept;
            FMTseException(const FMTseException& p_rhs) = default;
            FMTseException& operator = (const FMTseException& p_rhs) = default;
        private:
            unsigned int m_nSE;

        };
    }


#endif // FMTEXCEPTION_H_INCLUDED
