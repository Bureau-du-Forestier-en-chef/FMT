/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef FMTobject_H_INCLUDED
#define FMTobject_H_INCLUDED

#include "FMTexceptionhandler.h"
#include "FMTlogger.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <memory>


#if defined _MSC_VER || __CYGWIN__

#else
    #include <boost/dll/runtime_symbol_info.hpp>
#endif




namespace Core
{
class FMTobject
	{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(_exhandler);
		ar & BOOST_SERIALIZATION_NVP(_logger);
	}
	protected:
		std::shared_ptr<Exception::FMTexceptionhandler> _exhandler;
		std::shared_ptr<Logging::FMTlogger> _logger;
		void checksignals();
		string getruntimelocation();
		void setCPLhandler();
	public:
		FMTobject();
		virtual ~FMTobject();
		FMTobject(const std::shared_ptr<Exception::FMTexceptionhandler> exhandler);
		FMTobject(const FMTobject& rhs);
		FMTobject& operator = (const FMTobject& rhs);
		void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger);
		void passinexceptionhandler(const std::shared_ptr<Exception::FMTexceptionhandler>& exhandler);
		void setdefaultlogger();
		void setdebuglogger();
		void setdefaultexceptionhandler();
		void setquietexceptionhandler();
		void setdebugexceptionhandler();
		void setfreeexceptionhandler();
	};
}

#endif

