/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_DATASETFINGERPRINT_H_INCLUDED
#define PERFORMANCE_DATASETFINGERPRINT_H_INCLUDED

#include <string>
#include <vector>

namespace Performance
{
	// SHA-256 of the files of a Woodstock model: the files beside its primary file, then those of
	// Scenarios/<scenario> for each scenario read, each file hashed with its path relative to the
	// model. A model outside the source tree has no commit: two measurements of it compare only
	// when their fingerprints are the same.
	class DatasetFingerprint
	{
	public:
		// Returns the fingerprint in hexadecimal, or an empty text where it cannot be computed:
		// outside Windows, or when a file cannot be read.
		static std::string compute(const std::string& p_primaryFile, const std::vector<std::string>& p_scenarios);
	};
}

#endif
