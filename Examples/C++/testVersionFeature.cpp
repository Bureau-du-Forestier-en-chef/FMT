/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTVersion.h"

int main()
{
    const bool hasOnnxRuntime = Version::FMTVersion::hasFeature("ONNXRUNTIME");
#ifdef FMTWITHONNXR
    return hasOnnxRuntime ? 0 : 1;
#else
    return hasOnnxRuntime ? 1 : 0;
#endif
}
