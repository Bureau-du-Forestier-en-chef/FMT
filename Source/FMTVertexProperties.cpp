/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTVertexProperties.h"
#include "FMTFuturDevelopment.h"
#include "FMTActualDevelopment.h"

namespace Graph
{
	FMTVertexProperties::FMTVertexProperties(const FMTVertexProperties& rhs):
		FMTBaseVertexProperties(rhs), constraintID(rhs.constraintID)
	{

	}

	FMTVertexProperties::FMTVertexProperties(const Core::FMTFuturDevelopment& ldevelopment,
		const int& lconstraintID):FMTBaseVertexProperties(ldevelopment),constraintID(lconstraintID)
		{
		}
	FMTVertexProperties::FMTVertexProperties(const Core::FMTActualDevelopment& ldevelopment,
		const int& lconstraintID): FMTBaseVertexProperties(ldevelopment), constraintID(lconstraintID)
		{

		}
    FMTVertexProperties::FMTVertexProperties(const Core::FMTDevelopment& ldevelopment, const int& lconstraintID):
		FMTBaseVertexProperties(ldevelopment), constraintID(lconstraintID)
		{

		}

	

	void FMTVertexProperties::setConstraintId(const int& ID)
		{
		constraintID = ID;
		}


	std::string FMTVertexProperties::constraintName() const
		{
		return "D" + std::to_string(constraintID);
		}
}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTVertexProperties)