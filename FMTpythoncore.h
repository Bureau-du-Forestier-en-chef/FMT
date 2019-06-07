#ifndef FMTPYCORE_H_INCLUDED
#define FMTPYCORE_H_INCLUDED

#if defined FMTPY
#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#include <boost/python.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/python/stl_iterator.hpp>
#include <regex>
#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <map>
#include <tuple>
#include <boost/dynamic_bitset.hpp>
#include <boost/unordered_map.hpp>
#include "FMTexception.h"



using namespace boost::python;
using namespace Exception;


namespace boost{

template<class T>
static size_t pyhash(const T& value)
    {
    return boost::hash<T>()(value);
    }
}

template <class K, class V>
struct MapToDict
{
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

template <class K, class V>
struct UMapToDict
{
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



template<class T>
struct VecToList
{
    static PyObject* convert(const vector<T>& vec)
    {
        boost::python::list* l = new boost::python::list();
        for(size_t i = 0; i < vec.size(); i++) {
            l->append(vec[i]);
        }

        return l->ptr();
    }
};

template<typename T>
struct VecFrList
{

    VecFrList()
    {
    boost::python::converter::registry::push_back(&VecFrList<T>::convertible,
			  &VecFrList<T>::construct,
			  type_id<std::vector<T> >());
    }

    // Determine if obj_ptr can be converted in a std::vector<T>
    static void* convertible(PyObject* obj_ptr)
    {
      if (!PyList_Check(obj_ptr)){
    PyObject* nullobj = nullptr;
	return nullobj;
      }
      return obj_ptr;
    }

    // Convert obj_ptr into a std::vector<T>
    static void construct(
    PyObject* obj_ptr,
    boost::python::converter::rvalue_from_python_stage1_data* data)
    {
      // Extract the character data from the python string
      //      const char* value = PyString_AsString(obj_ptr);
       boost::python::list l(handle<>(borrowed(obj_ptr)));

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
      int le = int(len(l));
      v.resize(le);
      for(int i = 0;i!=le;++i){
	v[i] = extract<T>(l[i]);
      }

      // Stash the memory chunk pointer for later use by boost.python
      data->convertible = storage;
    }
};



struct iterable_converter
{
  /// @note Registers converter from a python interable type to the
  ///       provided type.
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

    static void* convertible(PyObject* object)
      {
        return PyObject_GetIter(object) ? object : NULL;
      }

 template <typename Container>
  static void construct(
    PyObject* object,
    boost::python::converter::rvalue_from_python_stage1_data* data)
  {
    namespace python = boost::python;
    // Object is a borrowed reference, so create a handle indicting it is
    // borrowed for proper reference counting.
    python::handle<> handle(python::borrowed(object));

    // Obtain a handle to the memory block that the converter has allocated
    // for the C++ type.
    typedef python::converter::rvalue_from_python_storage<Container>
                                                                storage_type;
    void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

    typedef python::stl_input_iterator<typename Container::value_type>
                                                                    iterator;

    // Allocate the C++ type into the converter's memory block, and assign
    // its handle to the converter's convertible variable.  The C++
    // container is populated by passing the begin and end iterators of
    // the python object to the container's constructor.
    new (storage) Container(
      iterator(python::object(handle)), // begin
      iterator());                      // end
    data->convertible = storage;
  }
};

template<class k, class e>
struct MapFrDict
{
	/// @note Registers converter from a python interable type to the
	///       provided type.
	MapFrDict()
	{

		boost::python::converter::registry::push_back(&MapFrDict<k,e>::convertible,
			&MapFrDict<k,e>::construct,
			boost::python::type_id<std::map<k,e> >());


	}

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


	static void construct(
		PyObject* obj_ptr,
		boost::python::converter::rvalue_from_python_stage1_data* data)
	{
		boost::python::dict mapping(handle<>(borrowed(obj_ptr)));

		// // Verify that obj_ptr is a string (should be ensured by convertible())
		// assert(value);

		// Grab pointer to memory into which to construct the new std::vector<T>
		void* storage = (
			(boost::python::converter::rvalue_from_python_storage<std::map<k,e>>*)
			data)->storage.bytes;

		// in-place construct the new std::vector<T> using the character data
		// extraced from the python object
		std::map<k, e>& v = *(new (storage) std::map<k, e>());

		// populate the vector from list contains !!!
		boost::python::list keys = mapping.keys();
		for (int i = 0; i < len(keys); ++i)
			{
			boost::python::extract<k> extracted_key(keys[i]);
			k newkey = extracted_key;
			boost::python::extract<e> extracted_val(mapping[newkey]);
			e value = extracted_val;
			v[newkey] = value;
			}

		// Stash the memory chunk pointer for later use by boost.python
		data->convertible = storage;
	}
};



void FMTtranslate_warning(FMTwarning const& e)
    {

	//boost::python::object pythonExceptionInstance(e);
	//PyErr_SetObject(myCPPExceptionType, pythonExceptionInstance.ptr());
    PyErr_SetString(PyExc_UserWarning, e.what());
    }

PyObject* FMTexceptiontype = NULL;

void FMTtranslate_error(FMTerror const& error) //should be implemented more like https://stackoverflow.com/questions/9620268/boost-python-custom-exception-class
    {
	if (error.hold())
		{
		assert(FMTexceptiontype != NULL);
		boost::python::object pythonExceptionInstance(error);
		PyErr_SetObject(FMTexceptiontype, pythonExceptionInstance.ptr());
	}else {
		PyErr_SetString(PyExc_RuntimeError, error.what());
		}
	//throw_error_already_set();
    //PyErr_SetString(PyExc_RuntimeError, error.what());
    }
/*
struct FMTlpmodel_pickle_suite : boost::python::pickle_suite
{
	static
		boost::python::tuple
		getinitargs(const FMTlpmodel& lpmodel)
	{
		return boost::python::make_tuple(w.get_country());
	}
};
*/
#endif

#endif
