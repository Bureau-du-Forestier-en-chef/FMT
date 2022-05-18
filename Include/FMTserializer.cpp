#include "FMTserializer.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/hash_collections_save_imp.hpp>
#include <boost/serialization/hash_collections_load_imp.hpp>

#include "FMTfork.hpp"

namespace Core
{


	template void FMTserializer::serialize<FMTfork,boost::archive::binary_oarchive>(FMTfork& element, boost::archive::binary_oarchive& ar, const unsigned int version)
	{
		try {
			ar& boost::serialization::make_nvp("specifications", boost::serialization::base_object<FMTspec>(element));
			ar& element.fork
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTserializer::serialize", __LINE__, __FILE__);
		}
	}

	template void FMTserializer::serialize<FMTfork, boost::archive::binary_iarchive>(FMTfork& element, boost::archive::binary_iarchive& ar, const unsigned int version)
	{
		try {
			ar& boost::serialization::make_nvp("specifications", boost::serialization::base_object<FMTspec>(element));
			ar& element.fork
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTserializer::serialize", __LINE__, __FILE__);
		}
	}



	template<class Archive, typename T>
	void FMTserializer::serialize(T& element, Archive& ar, const unsigned int version)
	{
		try {
			ar& BOOST_SERIALIZATION_NVP(element);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTserializer::serialize", __LINE__, __FILE__);
		}
	}

	template<class Archive, typename T>
	void FMTserializer::save(T& element, Archive& ar, const unsigned int version)
	{
		try {
			serialize<T>(element, ar, version);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTserializer::save", __LINE__, __FILE__);
		}
	}

	template<class Archive, typename T>
	T FMTserializer::load(Archive& ar, const unsigned int version)
	{
		T newelement;
		try {
			serialize<T>(newelement, ar, version);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTserializer::load", __LINE__, __FILE__);
		}
		return newelement;
	}

	template<class Archive, typename T>
	void serializenvp(T& element, Archive& ar, const unsigned int version)
	{
		try {
			ar& boost::serialization::make_nvp(BOOST_PP_STRINGIZE(T), boost::serialization::base_object<T>(element));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTserializer::serializenvp", __LINE__, __FILE__);
		}
	}
	template<class Archive, typename T>
	void savenvp(T& element, Archive& ar, const unsigned int version)
	{
		try {
			serializenvp<T>(element, ar, version);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTserializer::savenvp", __LINE__, __FILE__);
		}
	}
	template<class Archive, typename T>
	T loadnvp(Archive& ar, const unsigned int version)
	{
		T newelement;
		try {
			serializenvp<T>(newelement, ar, version);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTserializer::loadnvp", __LINE__, __FILE__);
		}
		return newelement;
	}


}