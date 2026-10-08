/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_JSONWRITER_H_INCLUDED
#define PERFORMANCE_JSONWRITER_H_INCLUDED

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace Performance
{
	// Writes indented JSON text. Objects and arrays are opened and closed in order, and inside an
	// object every value follows its key.
	class JsonWriter
	{
	public:
		explicit JsonWriter(std::ostream& p_stream);
		void beginObject();
		void endObject();
		void beginArray();
		void endArray();
		void key(const std::string& p_name);
		void value(const std::string& p_text);
		void value(const char* p_text);
		void value(bool p_flag);
		void value(std::int64_t p_number);
		void value(std::uint64_t p_number);
		// Writes p_number with p_decimals digits after the point and never in exponent form, so that
		// CompareResults.cmake can read it. Not-a-number and infinities are written null.
		void value(double p_number, int p_decimals);
		// Writes p_number with up to 15 significant digits. Not-a-number and infinities are written
		// null.
		void value(double p_number);
		void nullValue();

	private:
		std::ostream& m_stream;
		// One flag per open object or array, true until its first element is written.
		std::vector<bool> m_empty;
		bool m_afterKey = false;

		void _beginElement();
		void _open(char p_bracket);
		void _close(char p_bracket);
		void _indent();
		static std::string _escape(const std::string& p_text);
	};
}

#endif
