/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTSCOPEDSETRANSLATOR_Hm_included
#define FMTSCOPEDSETRANSLATOR_Hm_included

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
    // DocString: FMTScopedSeTranslator
    /**
    @brief RAII helper that installs a structured exception translator for its scope on MSVC.
    */
    class FMTScopedSeTranslator
        {
        public:
            #if defined _MSC_VER
                // DocString: FMTScopedSeTranslator(_se_translator_function)
                /**
                @brief Install the given structured exception translator for the lifetime of this object.
                @param[in] p_newSETranslator the structured exception translator to install.
                */
                FMTScopedSeTranslator(_se_translator_function p_newSETranslator) noexcept;
            #endif	
            // DocString: ~FMTScopedSeTranslator()
            /**
            @brief Restore the previous structured exception translator.
            */
            ~FMTScopedSeTranslator() noexcept;
            // DocString: FMTScopedSeTranslator(const FMTScopedSeTranslator&)
            /**
            @brief Default copy constructor for FMTScopedSeTranslator.
            @param[in] p_rhs the FMTScopedSeTranslator to copy.
            */
            FMTScopedSeTranslator(const FMTScopedSeTranslator& p_rhs) = default;
            // DocString: FMTScopedSeTranslator::operator=
            /**
            @brief Default copy assignment operator for FMTScopedSeTranslator.
            @param[in] p_rhs the FMTScopedSeTranslator to copy.
            @return a reference to this FMTScopedSeTranslator.
            */
            FMTScopedSeTranslator& operator = (const FMTScopedSeTranslator& p_rhs) = default;
        private:
        #if defined _MSC_VER
            _se_translator_function m_oldSeTranslator;
        #endif	
        };
    }


#endif // FMTEXCEPTION_Hm_included
