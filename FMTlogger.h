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
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/lexical_cast.hpp>
#include <CoinMessageHandler.hpp>
#include <fstream> 



namespace Logging
{

	class FMTlogger : public CoinMessageHandler
		{
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar & BOOST_SERIALIZATION_NVP(this->logLevel());
		}
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			int coinloglevel = 0;
			ar & BOOST_SERIALIZATION_NVP(coinloglevel);
			this->setLogLevel(coinloglevel);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		protected:
			std::ofstream* filestream;
			void cout(const char* message) const;
		public:
			FMTlogger();
			virtual ~FMTlogger();
			FMTlogger(const FMTlogger& rhs);
			FMTlogger& operator = (const FMTlogger& rhs);
			int print() override;
			void checkSeverity() override;
			CoinMessageHandler * clone() const override;
			void logstamp();
			void logtime();
			void settostream(std::ofstream& stream);
			template<class T>
			FMTlogger& operator<<(const T &msg)
				{
				const std::string value = boost::lexical_cast<std::string>(msg);
				this->cout(value.c_str());
				return *this;
				}
		};

}
#endif