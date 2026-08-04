/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMODELYIELDHANDLER_Hm_included
#define FMTMODELYIELDHANDLER_Hm_included

#include "FMTYieldHandler.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <memory>

namespace Models
{
	class FMTModel;
};

namespace Core
{
	class FMTData;
	class FMTYieldModel;
	class FMTTimeYieldHandler;
	// DocString: FMTModelYieldHandler
	/**
	@brief Yield handler wrapping machine learning yield models.
	*/
	class FMTEXPORT FMTModelYieldHandler final : public FMTYieldHandler
	{
	public:
		// DocString: FMTModelYieldHandler::get
		/**
		@brief Get the value of a yield for a request.
		@param[in] yld the yield name.
		@param[in] request the yield request.
		@return the yield value.
		*/
		virtual double get(const std::string& yld, const FMTYieldRequest& request) const;
		// DocString: FMTModelYieldHandler::operator std::string
		/**
		@brief Return the string representation of the model yield handler.
		@return the string representation.
		*/
		virtual  operator std::string() const;
		// DocString: ~FMTModelYieldHandler()
		/**
		@brief Default destructor for FMTModelYieldHandler.
		*/
		~FMTModelYieldHandler() = default;
		// DocString: FMTModelYieldHandler()
		/**
		@brief Default constructor for FMTModelYieldHandler.
		*/
		FMTModelYieldHandler();
		// DocString: FMTModelYieldHandler(const FMTModelYieldHandler&)
		/**
		@brief Copy constructor for FMTModelYieldHandler.
		@param[in] rhs the FMTModelYieldHandler to copy.
		*/
		FMTModelYieldHandler(const FMTModelYieldHandler& rhs);
		// DocString: FMTModelYieldHandler::operator=
		/**
		@brief Copy assignment operator for FMTModelYieldHandler.
		@param[in] rhs the FMTModelYieldHandler to copy.
		@return a reference to this FMTModelYieldHandler.
		*/
		FMTModelYieldHandler& operator = (const FMTModelYieldHandler& rhs);
		// DocString: FMTModelYieldHandler(const FMTMask&)
		/**
		@brief Construct a model yield handler from a mask.
		@param[in] p_mask the mask.
		*/
		FMTModelYieldHandler(const FMTMask& p_mask);
		// DocString: FMTModelYieldHandler::getModelsNameByIndex
		/**
		@brief Return the model names by their index.
		@return a map of model name to index.
		*/
		std::map<std::string, size_t>getModelsNameByIndex() const;
		// DocString: FMTModelYieldHandler::pushBackModel
		/**
		@brief Push back a yield model.
		@param[in] model the yield model.
		*/
		void pushBackModel(const std::unique_ptr<FMTYieldModel>& model);
		// DocString: FMTModelYieldHandler::setYield
		/**
		@brief Set the yield name for a given model and yield id.
		@param[in] modelid the model id.
		@param[in] yieldid the yield id.
		@param[in] yldname the yield name.
		*/
		void setYield(const size_t& modelid,const size_t& yieldid,const std::string& yldname);
		// DocString: FMTModelYieldHandler::clone
		/**
		@brief Clone the model yield handler.
		@return a unique pointer to the cloned yield handler.
		*/
		virtual std::unique_ptr<FMTYieldHandler>clone() const;
		// DocString: FMTModelYieldHandler::operator==
		/**
		@brief Equality comparison operator of FMTModelYieldHandler.
		@param[in] rhs the handler to compare with.
		@return true if both handlers are equal else false.
		*/
		virtual bool operator == (const FMTModelYieldHandler& rhs) const;
		// DocString: FMTModelYieldHandler::empty
		/**
		@brief Return true if the handler is empty.
		@return true if the handler is empty else false.
		*/
		virtual bool empty() const;
		// DocString: FMTModelYieldHandler::size
		/**
		@brief Return the number of yields in the handler.
		@return the number of yields.
		*/
		virtual size_t size() const;
		// DocString: FMTModelYieldHandler::getType
		/**
		@brief Return the type of the yield handler.
		@return the yield type.
		*/
		virtual FMTyldtype getType() const;
		// DocString: FMTModelYieldHandler::containsYield
		/**
		@brief Return true if the handler contains a given yield.
		@param[in] yldname the yield name.
		@return true if the handler contains the yield else false.
		*/
		virtual bool containsYield(const std::string& yldname) const;
		// DocString: FMTModelYieldHandler::isNullYield
		/**
		@brief Return true if a yield is a null yield.
		@param[in] yldname the yield name.
		@return true if the yield is null else false.
		*/
		virtual bool isNullYield(const std::string& yldname) const;
		// DocString: FMTModelYieldHandler::getYieldNames
		/**
		@brief Return the yield names of the handler.
		@return the yield names.
		*/
		virtual std::vector<std::string>getYieldNames() const;
		// DocString: FMTModelYieldHandler::clearCache
		/**
		@brief Clear the cache of the handler.
		*/
		virtual void clearCache();
		// DocString: FMTModelYieldHandler::preSolve
		/**
		@brief Return a presolved copy of the handler.
		@param[in] filter the mask filter.
		@param[in] newthemes the presolved themes.
		@return the presolved yield handler.
		*/
		virtual std::unique_ptr<FMTYieldHandler> preSolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>& newthemes) const;
		// DocString: FMTModelYieldHandler::postSolve
		/**
		@brief Return a postsolved copy of the handler.
		@param[in] filter the mask filter.
		@param[in] basethemes the original themes.
		@return the postsolved yield handler.
		*/
		virtual std::unique_ptr<FMTYieldHandler> postSolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>& basethemes) const;
		// DocString: FMTModelYieldHandler::clearRandomYieldsCache
		/**
		@brief Clear the cache of all random yield models.
		*/
		void clearRandomYieldsCache();
		// DocString: FMTModelYieldHandler::toTimeHandler
		/**
		@brief Convert the yield models to a time yield handler if possible.
		@return the time yield handler.
		*/
		FMTTimeYieldHandler toTimeHandler() const;
		// DocString: FMTModelYieldHandler::setModel
		/**
		@brief Set the model linked to the yield models.
		@param[in] p_modelPtr the pointer to the model.
		*/
		void setModel(Models::FMTModel* p_modelPtr);
	private:
		friend class boost::serialization::access;
		// DocString: FMTModelYieldHandler::serialize
		/**
		@brief Serialize the FMTModelYieldHandler through its base FMTYieldHandler for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTyieldhandler", boost::serialization::base_object<FMTYieldHandler>(*this));
			ar& m_yldnames;
		}
		std::vector<std::unique_ptr<FMTYieldModel>>m_models;
		std::map<std::string, std::pair<size_t, size_t>, cmpYieldString>m_yldnames;
	};

}

#endif 
