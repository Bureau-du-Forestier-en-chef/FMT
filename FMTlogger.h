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

#ifndef FMTlogger_H_INCLUDED
#define FMTlogger_H_INCLUDED
#include <iostream>
#include <boost/python.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <CoinMessageHandler.hpp>



using namespace std;

namespace Logging
{
	enum FMTlogtype 
		{
		FMT_Debug = 0,
		FMT_Info = 1,
		FMT_Warn = 2,
		FMT_Error = 3
		};


	class FMTlogger : public CoinMessageHandler
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(_msglevel);
			}
		FMTlogtype _msglevel = FMT_Debug;
		string getlevel(FMTlogtype ltype);
		public:
			FMTlogger(FMTlogtype type);
			virtual ~FMTlogger() = default;
			FMTlogger();
			FMTlogger(const FMTlogger& rhs);
			FMTlogger& operator = (const FMTlogger& rhs);
			int print() override;
			void checkSeverity() override;
			CoinMessageHandler * clone() const override;
			template<class T>
			FMTlogger& operator<<(const T &msg)
				{
				#if defined (_MSC_VER) && defined(FMTPY)
					string value = boost::lexical_cast<std::string>(msg);
					PySys_WriteStdout(value.c_str());
				#else
					cout << msg;
				#endif 
					return *this;
				}
		};

}
#endif