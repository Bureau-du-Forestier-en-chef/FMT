/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/



#include "FMTObject.h"
#include "FMTMask.h"
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
	@brief Request for yield values using a development and optionally a graph vertex.
	@details The yield is calculated using the FMTYields class and the graph to locate the actual state of the development. A request can also be built without any reference to a graph.
	*/
	class FMTYieldRequest final : public FMTObject
	{
	public:
		typedef typename std::vector<std::pair<FMTMask, std::unique_ptr<FMTYieldHandler>>>::const_iterator const_iterator;
		// DocString: FMTYieldRequest(const FMTDevelopment&, const Graph::FMTGraphVertexToYield&)
		/**
		@brief Construct a yield request from a development and a graph vertex.
		@param[in] ldevelopment the development.
		@param[in] lgraphvertex the graph vertex to yield.
		*/
		FMTYieldRequest(const FMTDevelopment& ldevelopment,
			const Graph::FMTGraphVertexToYield& lgraphvertex);
		// DocString: FMTYieldRequest(const FMTDevelopment&, const FMTYieldRequest&)
		/**
		@brief Construct a yield request from a development and a previous request.
		@param[in] ldevelopment the development.
		@param[in] oldrequest the previous request.
		*/
		FMTYieldRequest(const FMTDevelopment& ldevelopment,
			const FMTYieldRequest& oldrequest);
		// DocString: FMTYieldRequest(const FMTDevelopment&)
		/**
		@brief Construct a yield request from a development, without reference to a graph.
		@param[in] ldevelopment the development.
		*/
		FMTYieldRequest(const FMTDevelopment& ldevelopment);
		// DocString: FMTYieldRequest()
		/**
		@brief Default constructor for FMTYieldRequest.
		*/
		FMTYieldRequest() = default;
		// DocString: FMTYieldRequest(const FMTYieldRequest&)
		/**
		@brief Copy constructor for FMTYieldRequest.
		@param[in] rhs the FMTYieldRequest to copy.
		*/
		FMTYieldRequest(const FMTYieldRequest& rhs) = default;
		// DocString: FMTYieldRequest::operator=
		/**
		@brief Copy assignment operator for FMTYieldRequest.
		@param[in] rhs the FMTYieldRequest to copy.
		@return a reference to this FMTYieldRequest.
		*/
		FMTYieldRequest& operator = (const FMTYieldRequest& rhs)=default;
		// DocString: FMTYieldRequest::getDevelopment
		/**
		@brief Return the development of the request.
		@return a const reference to the development.
		*/
		const FMTDevelopment& getDevelopment() const;
		// DocString: FMTYieldRequest::getDatas
		/**
		@brief Return the yield data iterators of the request.
		@return a const reference to the yield data iterators.
		*/
		const std::vector<const_iterator>&getDatas() const;
		// DocString: FMTYieldRequest::getResumeMask
		/**
		@brief Return the resume mask of the request.
		@return a const reference to the resume mask.
		*/
		const Core::FMTMask& getResumeMask() const;
		// DocString: FMTYieldRequest::getVertexGraphInfo
		/**
		@brief Return the graph vertex information of the request.
		@return a pointer to the graph vertex to yield.
		*/
		const Graph::FMTGraphVertexToYield* getVertexGraphInfo() const;
		// DocString: FMTYieldRequest::getFirstSeen
		/**
		@brief Return the first data iterator for a given yield.
		@param[in] p_yield the yield name.
		@return the first data iterator for the yield.
		*/
		const_iterator getFirstSeen(const std::string& p_yield) const;
	private:
		friend class FMTYields;
		mutable std::vector<const_iterator>datas;
		mutable FMTMask resume_mask;
		mutable FMTYields const* m_yields;
		FMTDevelopment const* development;
		Graph::FMTGraphVertexToYield const* graphvertex;
		// DocString: FMTYieldRequest::_updateData
		/**
		@brief Update the data of the request from the yields.
		@param[in] yields the yields.
		*/
		void _updateData(const FMTYields& yields) const;
	};
	


}


#endif // FMTYLD_Hm_included
