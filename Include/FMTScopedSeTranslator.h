/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTSCOPEDSETRANSLATOR_H_INCLUDED
#define FMTSCOPEDSETRANSLATOR_H_INCLUDED

#if defined _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <windows.h>
#include <eh.h>
#endif	

/// Namespace for exceptions and exceptions handling (warnings and errors) thrown by FMT and all exceptions handlers available.
namespace Exception
    {
    class FMTScopedSeTranslator
        {
        public:
            #if defined _MSC_VER
                FMTScopedSeTranslator(_se_translator_function p_newSETranslator) noexcept;
            #endif	
            ~FMTScopedSeTranslator() noexcept;
            FMTScopedSeTranslator(const FMTScopedSeTranslator& p_rhs) = default;
            FMTScopedSeTranslator& operator = (const FMTScopedSeTranslator& p_rhs) = default;
        private:
        #if defined _MSC_VER
            _se_translator_function m_oldSeTranslator;
        #endif	
        };
    }


#endif // FMTEXCEPTION_H_INCLUDED
