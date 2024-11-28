/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
	
#include "FMTScopedSeTranslator.h"

namespace Exception
    {
    #if defined _WIN32
        FMTScopedSeTranslator::FMTScopedSeTranslator(_se_translator_function p_newSETranslator) noexcept
            : m_oldSeTranslator(
            
                    _set_se_translator(p_newSETranslator)
            
                )
                        {
    
                        }
    #endif

    FMTScopedSeTranslator::~FMTScopedSeTranslator() noexcept
        { 
        #if defined _WIN32
                _set_se_translator(m_oldSeTranslator); 
        #endif	
        }

    }