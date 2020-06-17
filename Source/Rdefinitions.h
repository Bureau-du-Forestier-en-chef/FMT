/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef RDEFINITIONS_H_INCLUDED
#define RDEFINITIONS_H_INCLUDED

#if defined FMTWITHR

#include <Rcpp.h>
#include <vector>
#include <map>
#include "FMTlist.h"
#include "FMTlayer.h"



#define RCPP_DEFINEVECTOR(CLASS) namespace Rcpp{\
template <> std::vector<CLASS> as(SEXP obj){\
const int RTYPE = traits::r_sexptype_traits<CLASS>::rtype;\
Vector<RTYPE> rcppvec(obj);\
std::vector<CLASS>newvec;\
newvec.reserve(rcppvec.size());\
for(int index = 0; index < rcppvec.size();++index)\
{\
newvec.push_back(rcppvec[index]);\
}\
return newvec;}\
template <> SEXP wrap(const std::vector<CLASS>& obj){\
const int RTYPE=traits::r_sexptype_traits<CLASS>::rtype;\
return Vector<RTYPE>(obj.begin(), obj.end());}}

#define RCPP_DEFINELIST(CLASS) namespace Rcpp{\
template <> std::vector<CLASS> as(SEXP obj){\
List listobj(obj);\
std::vector<CLASS>newvec;\
newvec.reserve(listobj.size());\
for(int index = 0; index < listobj.size();++index)\
{\
newvec.push_back(as<CLASS>(listobj[index]));\
}\
return newvec;}\
template <> SEXP wrap(const std::vector<CLASS>& obj){\
List listobj(obj.size());\
for(int index = 0; index < listobj.size();++index)\
{\
listobj[index]=wrap<CLASS>(obj.at(index));\
}\
return listobj;}}

#define RCPP_DEFINEPAIR(FIRST,SECOND)namespace Rcpp{\
template <> std::pair<FIRST,SECOND> as(SEXP obj){\
Rcpp::List rcpplist(obj);\
return std::pair<FIRST,SECOND>(as<FIRST>(rcpplist["first"]), as<SECOND>(rcpplist["second"]));}\
template <> SEXP wrap(const std::pair<FIRST,SECOND>& obj){\
return List::create(Named("first") = wrap<FIRST>(obj.first),Named("second") = wrap<SECOND>(obj.second));}}

#define RCPP_DEFINEMAP(KEY,OBJECT)namespace Rcpp{\
template <> std::map<KEY,OBJECT> as(SEXP obj){\
List rcpplist(obj);\
std::map<KEY,OBJECT>newmap;\
for(int index = 0; index < rcpplist.size();++index)\
{\
newmap[as<KEY>(as<List>(rcpplist[index])["first"])]=as<OBJECT>(as<List>(rcpplist[index])["second"]);\
}\
return newmap;}\
template <> SEXP wrap(const std::map<KEY,OBJECT>& obj){\
List rcpplist(obj.size());\
size_t index = 0;\
for(const auto& mapobject: obj)\
{\
rcpplist[index]=List::create(Named("first")=wrap<KEY>(mapobject.first),Named("second")=wrap<OBJECT>(mapobject.second));\
++index;\
}\
return rcpplist;}}

namespace R
{
	template <class T>
	void define_FMTlist(const char* name)
		{
		Rcpp::class_< Core::FMTlist<T> >(name,"@DocString(FMTlist)")
		.constructor("@DocString(FMTlist())");
		}

	template <class T>
	void define_FMTlayer(const char* name)
		{
		Rcpp::class_<Spatial::FMTlayer<T>>(name, "@DocString(FMTlayer)")
				.constructor("@DocString(FMTlayer())")
				.method("getXSize", &Spatial::FMTlayer<T>::GetXSize,
					"@DocString(FMTlayer::GetXSize)")
				.method("getYSize", &Spatial::FMTlayer<T>::GetYSize,
					"@DocString(FMTlayer::GetYSize)")
				.method("getgeotransform", &Spatial::FMTlayer<T>::getgeotransform,
					"@DocString(FMTlayer::getgeotransform)")
				.method("getprojection", &Spatial::FMTlayer<T>::getprojection,
					"@DocString(FMTlayer::getprojection)")
				.method("getmapping", &Spatial::FMTlayer<T>::getmapping,
					"@DocString(FMTlayer::getmapping)")
				.method("area", &Spatial::FMTlayer<T>::area,
					"@DocString(FMTlayer::area)")
				.method("getcellsize", &Spatial::FMTlayer<T>::getcellsize,
					"@DocString(FMTlayer::getcellsize)")
				.method("size", &Spatial::FMTlayer<T>::size,
					"@DocString(FMTlayer::size)");
		}
}



#endif



#endif // RDEFINITIONS_H_INCLUDED
