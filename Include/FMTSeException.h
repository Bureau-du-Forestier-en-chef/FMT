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
    // DocString: FMTSeException
    /**
    @brief Exception representing a structured exception (SE) caught on Windows.
    */
    class FMTSeException : public std::logic_error
        {
        public:
            // DocString: FMTSeException()
            /**
            @brief Default constructor for FMTSeException.
            */
            FMTSeException() noexcept;
            // DocString: FMTSeException(unsigned int)
            /**
            @brief Construct a FMTSeException from a structured exception number.
            @param[in] p_n the structured exception number.
            */
            FMTSeException(unsigned int p_n) noexcept;
            // DocString: FMTSeException::getSeNumber
            /**
            @brief Return the structured exception number.
            @return the structured exception number.
            */
            unsigned int getSeNumber() const noexcept;
            // DocString: FMTSeException(const FMTSeException&)
            /**
            @brief Default copy constructor for FMTSeException.
            @param[in] p_rhs the FMTSeException to copy.
            */
            FMTSeException(const FMTSeException& p_rhs) = default;
            // DocString: FMTSeException::operator=
            /**
            @brief Default copy assignment operator for FMTSeException.
            @param[in] p_rhs the FMTSeException to copy.
            @return a reference to this FMTSeException.
            */
            FMTSeException& operator = (const FMTSeException& p_rhs) = default;
        private:
            unsigned int m_nSE;

        };
    }


#endif // FMTEXCEPTION_Hm_included
