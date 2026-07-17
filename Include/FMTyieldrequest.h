/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/



#include "FMTobject.h"
#include "FMTmask.h"
#include <vector>
#include <unordered_map>


namespace Graph
{
	class FMTGraphVertexToYield;
}



#ifndef FMTYIELDREQUEST_Hm_included
#define FMTYIELDREQUEST_Hm_included

namespace Core
{
	class FMTDevelopment;
	class FMTYieldHandler;
	class FMTYields;
	// DocString: FMTYieldRequest
	/**
	Using a pointer to a given graph and a FMTDevelopment you can create thise class to request yield values.
	The yield will be calculated using the FMTYields class but also using the FMTGraph so the location of the 
	actual state of the FMTDevelopment.You can also build a yield request without anyreference to a graph for calculatin yield values.
	*/
	class FMTYieldRequest final : public FMTObject
	{
	public:
		typedef typename std::vector<std::pair<FMTMask, std::unique_ptr<FMTYieldHandler>>>::const_iterator const_iterator;
		FMTYieldRequest(const FMTDevelopment& ldevelopment,
			const Graph::FMTGraphVertexToYield& lgraphvertex);
		FMTYieldRequest(const FMTDevelopment& ldevelopment,
			const FMTYieldRequest& oldrequest);
		FMTYieldRequest(const FMTDevelopment& ldevelopment);
		FMTYieldRequest() = default;
		FMTYieldRequest(const FMTYieldRequest& rhs) = default;
		FMTYieldRequest& operator = (const FMTYieldRequest& rhs)=default;
		const FMTDevelopment& getDevelopment() const;
		const std::vector<const_iterator>&getDatas() const;
		const Core::FMTMask& getResumeMask() const;
		const Graph::FMTGraphVertexToYield* getVertexGraphInfo() const;
		const_iterator getFirstSeen(const std::string& p_yield) const;
	private:
		friend class FMTYields;
		mutable std::vector<const_iterator>datas;
		mutable FMTMask resume_mask;
		mutable FMTYields const* m_yields;
		FMTDevelopment const* development;
		Graph::FMTGraphVertexToYield const* graphvertex;
		void _updateData(const FMTYields& yields) const;
	};
	


}


#endif // FMTYLD_Hm_included
