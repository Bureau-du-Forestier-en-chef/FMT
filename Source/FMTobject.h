/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTobject_H_INCLUDED
#define FMTobject_H_INCLUDED

#include "FMTexceptionhandler.h"
#include "FMTlogger.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <memory>


#if defined _MSC_VER || __MINGW64__ || __CYGWIN__

#else
    #include <boost/dll/runtime_symbol_info.hpp>
#endif



#include <vector>

namespace Core
{
class FMTtheme;
// DocString: FMTobject
/**
FMTobject is the base class of multiple class it hold a shared exception handler pointer and logger.
This way has soon as a object is create from this base class you can passin the exception handler and the logger
to the newly ceated class. FMTobject plays a big role into exception handling and for ctrl-c signals for boost python.
It also contains some usefull functions for mask validation and runtimelocation etc...
*/
class FMTobject
	{
	// DocString: FMTobject::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		//ar & BOOST_SERIALIZATION_NVP(_exhandler);
		//ar & BOOST_SERIALIZATION_NVP(_logger);
		//ar & BOOST_SERIALIZATION_NVP(_section);
	}
	protected:
		// DocString: FMTobject:: forcesave
		/**
		By Default the serialization of a FMTobject does nothing if you want to get some usefull information use this function.
		*/
		template<class Archive>
		void forcesave(Archive& ar, const unsigned int version) const
		{
			//ar & BOOST_SERIALIZATION_NVP(_exhandler);
			//ar & BOOST_SERIALIZATION_NVP(_logger);
			//ar & BOOST_SERIALIZATION_NVP(_section);
		}
		// DocString: FMTobject:: forceload
		/**
		By Default the serialization of a FMTobject does nothing if you want to get some usefull information use this function.
		*/
		template<class Archive>
		void forceload(Archive& ar, const unsigned int version)
		{
			//ar & BOOST_SERIALIZATION_NVP(_exhandler);
			//ar & BOOST_SERIALIZATION_NVP(_logger);
			setdefaultlogger();
			setdefaultexceptionhandler();
			//ar & BOOST_SERIALIZATION_NVP(_section);
		}

		// DocString: FMTobject::_exhandler
		///A shared pointer to the exception handler.
		mutable std::shared_ptr<Exception::FMTexceptionhandler> _exhandler;
		// DocString: FMTobject::_logger
		///A shared pointer to the logger.
		std::shared_ptr<Logging::FMTlogger> _logger;
		// DocString: FMTobject::checksignals
		/**
		This function only check if the user has sent a ctrl-c signal using boost::python to FMT.
		*/
		void checksignals() const;
		// DocString: FMTobject::getruntimelocation
		/**
		This function return the location of the FMT shared library location.
		*/
		std::string getruntimelocation() const;
		// DocString: FMTobject::setCPLhandler
		/**
		This function is for gdal only it pass the FMT exception handler to gdal exception handler.
		*/
		void setCPLhandler(bool onlynull=false);
		// DocString: FMTobject::validate
		/**
		The funciton validate a the construction of a valid FMTmask using the mask string based on the
		themes. If their's less themes that the number present in the string mask then the string mask is
		going to be trim for the good number of FMTthemes.
		*/
		bool validate(const std::vector<Core::FMTtheme>& themes,
			std::string& mask,std::string otherinformation = std::string()) const;
		// DocString: FMTobject::checkmask
		/**
		This function validate the mask string for a given vector of themes and throw exception if
		something is not right. It'S called by the validate function.
		*/
		bool checkmask(const std::vector<Core::FMTtheme>& themes,
			const std::vector<std::string>& values, std::string& mask,
			const std::string& otherinformation) const;
	public:
		// DocString: FMTobject()
		/**
		FMTobject default constructor.
		*/
		FMTobject();
		// DocString: ~FMTobject()
		/**
		FMTobject default virutal destructor.
		*/
		virtual ~FMTobject();
		// DocString: FMTobject(const std::shared_ptr<Exception::FMTexceptionhandler>)
		/**
		When constructing a new FMTobject it's sometime usefull to passin the exception handler of an
		other FMTobject.
		*/
		FMTobject(const std::shared_ptr<Exception::FMTexceptionhandler> exhandler);
		// DocString: FMTobject(const FMTobject&)
		/**
		FMTobject default copy constructor.
		*/
		FMTobject(const FMTobject& rhs);
		// DocString: FMTobject::operator=
		/**
		FMTobject default copy assignment.
		*/
		FMTobject& operator = (const FMTobject& rhs);
		// DocString: FMTobject::passinlogger
		/**
		It's sometime usefull to pass in the logger of an other FMTobject.
		*/
		virtual void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger);
		// DocString: FMTobject::passinexceptionhandler
		/**
		It's sometime usefull to pass in the exception handler of an other FMTobject.
		*/
		virtual void passinexceptionhandler(const std::shared_ptr<Exception::FMTexceptionhandler>& exhandler);
		// DocString: FMTobject::sharewith
		/**
		Returns true if the object share the same exception handler and logger with the other object.
		*/
		virtual bool sharewith(const FMTobject& rhs) const;
		// DocString: FMTobject::passinobject
		/**
		It's sometime usefull to pass in the exception handler and the logger  of an other FMTobject to
		a FMTobject.
		*/
		virtual void passinobject(const FMTobject& rhs);
		// DocString: FMTobject::redirectlogtofile
		/**
		redict the log to a specific file (will append to it)
		*/
		void redirectlogtofile(const std::string& location);
		// DocString: FMTobject::setdefaultlogger
		/**
		Create and set a default logger to the FMTobject.
		*/
		virtual void setdefaultlogger();
		// DocString: FMTobject::setquietlogger
		/**
		Create and set a quiet logger to the FMTobject.
		*/
		virtual void setquietlogger();
		// DocString: FMTobject::setdebuglogger
		/**
		Create and set a debug logger to the FMTobject.
		*/
		virtual void setdebuglogger();
		// DocString: FMTobject::setdefaultexceptionhandler
		/**
		Create and set a default exception handler to the FMTobject.
		*/
		virtual void setdefaultexceptionhandler();
		// DocString: FMTobject::setquietexceptionhandler
		/**
		Create and set a quiet exception handler to the FMTobject.
		*/
		virtual void setquietexceptionhandler();
		// DocString: FMTobject::setdebugexceptionhandler
		/**
		Create and set a debug exception handler to the FMTobject.
		*/
		virtual void setdebugexceptionhandler();
		// DocString: FMTobject::setfreeexceptionhandle
		/**
		Create and set a free exception handler to the FMTobject.
		*/
		virtual void setfreeexceptionhandler();
		// DocString: FMTobject::disablenestedexceptions
		/**
		Disable nested exception throw of the Exceptionhandler by default all handlers
		do nested exception throw.
		*/
		virtual void disablenestedexceptions();
		// DocString: FMTobject::enablenestedexceptions
		/**
		Enable nested exception throw of the Exceptionhandler by default all handlers
		do nested exception throw.
		*/
		virtual void enablenestedexceptions();


	};
}
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Core::FMTobject)
BOOST_CLASS_TRACKING(Core::FMTobject, boost::serialization::track_always)
BOOST_CLASS_EXPORT_KEY(Core::FMTobject)

#endif

