/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTPYTHONCORE_Hm_included
#define FMTPYTHONCORE_Hm_included

#if defined FMTWITHPYTHON
#ifndef BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
	#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#endif

#include <boost/python.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/regex.h>
#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <map>
#include <tuple>
#include <set>
#include <boost/dynamic_bitset.hpp>
#include <boost/unordered_map.hpp>
#include "FMTException.h"
#include "FMTError.h"
#include "FMTWarning.h"



namespace boost{

// DocString: boost::pyHash
/**
@brief Hash a value using the boost hash of its type.
@tparam T the value type.
@param[in] value the value to hash.
@return the hash of the value.
*/
template<class T>
static size_t pyHash(const T& value)
    {
    return boost::hash<T>()(value);
    }
}

namespace Python
{
	// DocString: Python::MapToDict
	/**
	@brief Converter exposing a std::map to Python as a dict.
	@tparam K the key type.
	@tparam V the value type.
	*/
	template <class K, class V>
	struct MapToDict
	{
		// DocString: Python::MapToDict::convert
		/**
		@brief Convert a std::map into a Python dict.
		@param[in] lmap the map to convert.
		@return the Python dict.
		*/
		static PyObject* convert(const std::map<K, V>& lmap)
		{
			boost::python::dict* dictionary = new boost::python::dict();
			for (typename std::map<K, V>::const_iterator it = lmap.begin(); it != lmap.end(); ++it)
			{
				dictionary->operator[](it->first) = it->second;
			}
			return dictionary->ptr();
		}

	};

	// DocString: Python::UMapToDict
	/**
	@brief Converter exposing a boost unordered_map to Python as a dict.
	@tparam K the key type.
	@tparam V the value type.
	*/
	template <class K, class V>
	struct UMapToDict
	{
		// DocString: Python::UMapToDict::convert
		/**
		@brief Convert a boost unordered_map into a Python dict.
		@param[in] lmap the map to convert.
		@return the Python dict.
		*/
		static PyObject* convert(const boost::unordered_map<K, V>& lmap)
		{
			boost::python::dict* dictionary = new boost::python::dict();
			for (typename boost::unordered_map<K, V>::const_iterator it = lmap.begin(); it != lmap.end(); ++it)
			{
				dictionary->operator[](it->first) = it->second;
			}
			return dictionary->ptr();
		}

	};



	// DocString: Python::VecToList
	/**
	@brief Converter exposing a std::vector to Python as a list.
	@tparam T the element type.
	*/
	template<class T>
	struct VecToList
	{
		// DocString: Python::VecToList::convert
		/**
		@brief Convert a std::vector into a Python list.
		@param[in] vec the vector to convert.
		@return the Python list.
		*/
		static PyObject* convert(const std::vector<T>& vec)
		{
			boost::python::list* l = new boost::python::list();
			for (size_t i = 0; i < vec.size(); i++) {
				l->append(vec[i]);
			}

			return l->ptr();
		}
	};

	// DocString: Python::SetToList
	/**
	@brief Converter exposing a std::set to Python as a list.
	@tparam T the element type.
	*/
	template<class T>
	struct SetToList
	{
		// DocString: Python::SetToList::convert
		/**
		@brief Convert a std::set into a Python list.
		@param[in] p_set the set to convert.
		@return the Python list.
		*/
		static PyObject* convert(const std::set<T>& p_set)
		{
			boost::python::list* l = new boost::python::list();
			for (const auto& i : p_set)
			{
				l->append(i);
			}
			return l->ptr();
		}
	};

	// DocString: Python::VecFrList
	/**
	@brief Converter building a std::vector from a Python list.
	@tparam T the element type.
	*/
	template<typename T>
	struct VecFrList
	{

		// DocString: Python::VecFrList::VecFrList()
		/**
		@brief Register the Python list to std::vector converter.
		*/
		VecFrList()
		{
			boost::python::converter::registry::push_back(&VecFrList<T>::convertible,
				&VecFrList<T>::construct,
				boost::python::type_id<std::vector<T> >());
		}

		// Determine if obj_ptr can be converted in a std::vector<T>
		// DocString: Python::VecFrList::convertible
		/**
		@brief Return the object if it can be converted into a std::vector, else null.
		@param[in] obj_ptr the Python object.
		@return the object if convertible else null.
		*/
		static void* convertible(PyObject* obj_ptr)
		{
			if (!PyList_Check(obj_ptr)) {
				PyObject* nullobj = nullptr;
				return nullobj;
			}
			return obj_ptr;
		}

		// Convert obj_ptr into a std::vector<T>
		// DocString: Python::VecFrList::construct
		/**
		@brief Construct a std::vector in place from a Python list.
		@param[in] obj_ptr the Python list.
		@param[in,out] data the boost.python conversion data.
		*/
		static void construct(
			PyObject* obj_ptr,
			boost::python::converter::rvalue_from_python_stage1_data* data)
		{
			// Extract the character data from the python string
			//      const char* value = PyString_AsString(obj_ptr);
			boost::python::list l(boost::python::handle<>(boost::python::borrowed(obj_ptr)));

			// // Verify that obj_ptr is a string (should be ensured by convertible())
			// assert(value);

			// Grab pointer to memory into which to construct the new std::vector<T>
			void* storage = (
				(boost::python::converter::rvalue_from_python_storage<std::vector<T> >*)
				data)->storage.bytes;

			// in-place construct the new std::vector<T> using the character data
			// extraced from the python object
			std::vector<T>& v = *(new (storage) std::vector<T>());

			// populate the vector from list contains !!!
			int le = static_cast<int>(boost::python::len(l));
			v.resize(le);
			for (int i = 0; i != le; ++i) {
				v[i] = boost::python::extract<T>(l[i]);
			}

			// Stash the memory chunk pointer for later use by boost.python
			data->convertible = storage;
		}
	};



	// DocString: Python::iterable_converter
	/**
	@brief Converter building any iterable C++ container from a Python iterable.
	*/
	struct iterable_converter
	{
		/// @note Registers converter from a python interable type to the
		///       provided type.
		// DocString: Python::iterable_converter::from_python
		/**
		@brief Register the converter from a Python iterable to the provided container type.
		@tparam Container the container type.
		@return a reference to this converter, to support chaining.
		*/
		template <typename Container>
		iterable_converter&
			from_python()
		{
			boost::python::converter::registry::push_back(
				&iterable_converter::convertible,
				&iterable_converter::construct<Container>,
				boost::python::type_id<Container>());

			// Support chaining.
			return *this;
		}

		// DocString: Python::iterable_converter::convertible
		/**
		@brief Return the object if it is iterable, else null.
		@param[in] object the Python object.
		@return the object if convertible else null.
		*/
		static void* convertible(PyObject* object)
		{
			return PyObject_GetIter(object) ? object : NULL;
		}

		// DocString: Python::iterable_converter::construct
		/**
		@brief Construct the container in place from a Python iterable.
		@tparam Container the container type.
		@param[in] object the Python iterable.
		@param[in,out] data the boost.python conversion data.
		*/
		template <typename Container>
		static void construct(
			PyObject* object,
			boost::python::converter::rvalue_from_python_stage1_data* data)
		{

			// Object is a borrowed reference, so create a handle indicting it is
			// borrowed for proper reference counting.
			boost::python::handle<> handle(boost::python::borrowed(object));

			// Obtain a handle to the memory block that the converter has allocated
			// for the C++ type.
			typedef boost::python::converter::rvalue_from_python_storage<Container>
				storage_type;
			void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

			typedef boost::python::stl_input_iterator<typename Container::value_type>
				iterator;

			// Allocate the C++ type into the converter's memory block, and assign
			// its handle to the converter's convertible variable.  The C++
			// container is populated by passing the begin and end iterators of
			// the python object to the container's constructor.
			new (storage) Container(
				iterator(boost::python::object(handle)), // begin
				iterator());                      // end
			data->convertible = storage;
		}
	};

	// DocString: Python::MapFrDict
	/**
	@brief Converter building a std::map from a Python dict.
	@tparam k the key type.
	@tparam e the value type.
	*/
	template<class k, class e>
	struct MapFrDict
	{
		/// @note Registers converter from a python interable type to the
		///       provided type.
		// DocString: Python::MapFrDict::MapFrDict()
		/**
		@brief Register the Python dict to std::map converter.
		*/
		MapFrDict()
		{

			boost::python::converter::registry::push_back(&MapFrDict<k, e>::convertible,
				&MapFrDict<k, e>::construct,
				boost::python::type_id<std::map<k, e> >());


		}

		// DocString: Python::MapFrDict::convertible
		/**
		@brief Return the object if it can be converted into a std::map, else null.
		@param[in] obj_ptr the Python object.
		@return the object if convertible else null.
		*/
		static void* convertible(PyObject* obj_ptr)
		{
			if (PyMapping_Check(obj_ptr)) {
				return obj_ptr;
			}
			else {
				PyObject* nullobj = nullptr;
				return nullobj;
			}
		}


		// DocString: Python::MapFrDict::construct
		/**
		@brief Construct a std::map in place from a Python dict.
		@param[in] obj_ptr the Python dict.
		@param[in,out] data the boost.python conversion data.
		*/
		static void construct(
			PyObject* obj_ptr,
			boost::python::converter::rvalue_from_python_stage1_data* data)
		{
			boost::python::dict mapping(boost::python::handle<>(boost::python::borrowed(obj_ptr)));

			// // Verify that obj_ptr is a string (should be ensured by convertible())
			// assert(value);

			// Grab pointer to memory into which to construct the new std::vector<T>
			void* storage = (
				(boost::python::converter::rvalue_from_python_storage<std::map<k, e>>*)
				data)->storage.bytes;

			// in-place construct the new std::vector<T> using the character data
			// extraced from the python object
			std::map<k, e>& v = *(new (storage) std::map<k, e>());

			// populate the vector from list contains !!!
			boost::python::list keys = mapping.keys();
			for (int i = 0; i < boost::python::len(keys); ++i)
			{
				boost::python::extract<k> extractedKey(keys[i]);
				k newkey = extractedKey;
				boost::python::extract<e> extractedVal(mapping[newkey]);
				e value = extractedVal;
				v[newkey] = value;
			}

			// Stash the memory chunk pointer for later use by boost.python
			data->convertible = storage;
		}
	};


	// DocString: Python::FMTtranslate_warning
	/**
	@brief Translate a FMTWarning into a Python UserWarning.
	@param[in] e the warning to translate.
	*/
	void FMTtranslate_warning(Exception::FMTWarning const& e)
	{
		PyErr_SetString(PyExc_UserWarning, e.what());
	}

	PyObject* FMTexceptiontype = NULL;

	// DocString: Python::FMTtranslate_error
	/**
	@brief Translate a FMTError into a Python exception.
	@param[in] error the error to translate.
	*/
	void FMTtranslate_error(Exception::FMTError const& error) //should be implemented more like https://stackoverflow.com/questions/9620268/boost-python-custom-exception-class
	{
		if (error.hold())
		{
			assert(FMTexceptiontype != NULL);
			boost::python::object pythonExceptionInstance(error);
			PyErr_SetObject(FMTexceptiontype, pythonExceptionInstance.ptr());
		}
		else {
			PyErr_SetString(PyExc_RuntimeError, error.what());
		}
	}

	// DocString: Python::PairToPythonConverter
	/**
	@brief Converter exposing a std::pair to Python as a tuple.
	@tparam T1 the first element type.
	@tparam T2 the second element type.
	*/
	template<typename T1, typename T2>
	struct PairToPythonConverter {
		// DocString: Python::PairToPythonConverter::convert
		/**
		@brief Convert a std::pair into a Python tuple.
		@param[in] pair the pair to convert.
		@return the Python tuple.
		*/
		static PyObject* convert(const std::pair<T1, T2>& pair)
		{
			return boost::python::incref(boost::python::make_tuple(pair.first, pair.second).ptr());
		}
	};

	// DocString: Python::PythonToPairConverter
	/**
	@brief Converter building a std::pair from a Python tuple.
	@tparam T1 the first element type.
	@tparam T2 the second element type.
	*/
	template<typename T1, typename T2>
	struct PythonToPairConverter {
		// DocString: Python::PythonToPairConverter::PythonToPairConverter()
		/**
		@brief Register the Python tuple to std::pair converter.
		*/
		PythonToPairConverter()
		{
			boost::python::converter::registry::push_back(&convertible, &construct, boost::python::type_id<std::pair<T1, T2> >());
		}
		// DocString: Python::PythonToPairConverter::convertible
		/**
		@brief Return the object if it is a two element tuple, else null.
		@param[in] obj the Python object.
		@return the object if convertible else null.
		*/
		static void* convertible(PyObject* obj)
		{
			if (!PyTuple_CheckExact(obj)) return 0;
			if (PyTuple_Size(obj) != 2) return 0;
			return obj;
		}
		// DocString: Python::PythonToPairConverter::construct
		/**
		@brief Construct a std::pair in place from a Python tuple.
		@param[in] obj the Python tuple.
		@param[in,out] data the boost.python conversion data.
		*/
		static void construct(PyObject* obj, boost::python::converter::rvalue_from_python_stage1_data* data)
		{
			boost::python::tuple tuple(boost::python::borrowed(obj));
			void* storage = ((boost::python::converter::rvalue_from_python_storage<std::pair<T1, T2> >*) data)->storage.bytes;
			new (storage) std::pair<T1, T2>(boost::python::extract<T1>(tuple[0]), boost::python::extract<T2>(tuple[1]));
			data->convertible = storage;
		}
	};

	// DocString: Python::py_pair
	/**
	@brief Registers both directions of the std::pair to Python tuple conversion.
	@tparam T1 the first element type.
	@tparam T2 the second element type.
	*/
	template<typename T1, typename T2>
	struct py_pair {
		boost::python::to_python_converter<std::pair<T1, T2>, PairToPythonConverter<T1, T2> > toPy;
		PythonToPairConverter<T1, T2> fromPy;
	};

}

#endif

#endif
