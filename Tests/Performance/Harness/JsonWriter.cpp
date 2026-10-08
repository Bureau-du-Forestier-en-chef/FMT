/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "JsonWriter.h"

#include <cmath>
#include <cstdio>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace Performance
{
	JsonWriter::JsonWriter(std::ostream& p_stream) :
		m_stream(p_stream)
	{
	}

	void JsonWriter::beginObject()
	{
		_open('{');
	}

	void JsonWriter::endObject()
	{
		_close('}');
	}

	void JsonWriter::beginArray()
	{
		_open('[');
	}

	void JsonWriter::endArray()
	{
		_close(']');
	}

	void JsonWriter::key(const std::string& p_name)
	{
		_beginElement();
		m_stream << _escape(p_name) << ": ";
		m_afterKey = true;
	}

	void JsonWriter::value(const std::string& p_text)
	{
		_beginElement();
		m_stream << _escape(p_text);
	}

	void JsonWriter::value(const char* p_text)
	{
		value(std::string(p_text));
	}

	void JsonWriter::value(bool p_flag)
	{
		_beginElement();
		m_stream << (p_flag ? "true" : "false");
	}

	void JsonWriter::value(std::int64_t p_number)
	{
		_beginElement();
		m_stream << p_number;
	}

	void JsonWriter::value(std::uint64_t p_number)
	{
		_beginElement();
		m_stream << p_number;
	}

	void JsonWriter::value(double p_number, int p_decimals)
	{
		if (!std::isfinite(p_number))
		{
			nullValue();
			return;
		}
		std::ostringstream text;
		text.imbue(std::locale::classic());
		text << std::fixed << std::setprecision(p_decimals) << p_number;
		_beginElement();
		m_stream << text.str();
	}

	void JsonWriter::value(double p_number)
	{
		if (!std::isfinite(p_number))
		{
			nullValue();
			return;
		}
		std::ostringstream text;
		text.imbue(std::locale::classic());
		text << std::setprecision(15) << p_number;
		_beginElement();
		m_stream << text.str();
	}

	void JsonWriter::nullValue()
	{
		_beginElement();
		m_stream << "null";
	}

	void JsonWriter::_beginElement()
	{
		if (m_afterKey)
		{
			m_afterKey = false;
			return;
		}
		if (m_empty.empty())
		{
			return;
		}
		if (!m_empty.back())
		{
			m_stream << ',';
		}
		m_empty.back() = false;
		m_stream << '\n';
		_indent();
	}

	void JsonWriter::_open(char p_bracket)
	{
		_beginElement();
		m_stream << p_bracket;
		m_empty.push_back(true);
	}

	void JsonWriter::_close(char p_bracket)
	{
		if (m_empty.empty())
		{
			throw std::logic_error("JsonWriter: no object or array to close");
		}
		const bool WAS_EMPTY = m_empty.back();
		m_empty.pop_back();
		if (!WAS_EMPTY)
		{
			m_stream << '\n';
			_indent();
		}
		m_stream << p_bracket;
		if (m_empty.empty())
		{
			m_stream << '\n';
		}
	}

	void JsonWriter::_indent()
	{
		m_stream << std::string(m_empty.size() * 2, ' ');
	}

	// Bytes above 127 are written as \u00XX: the text of FMT is not UTF-8, and the file must stay
	// valid JSON whatever a name holds.
	std::string JsonWriter::_escape(const std::string& p_text)
	{
		std::string escaped = "\"";
		for (const char CHARACTER : p_text)
		{
			const unsigned char CODE = static_cast<unsigned char>(CHARACTER);
			if (CHARACTER == '"' || CHARACTER == '\\')
			{
				escaped += '\\';
				escaped += CHARACTER;
			}
			else if (CODE < 0x20 || CODE > 0x7E)
			{
				char sequence[8] = {};
				std::snprintf(sequence, sizeof(sequence), "\\u%04x", static_cast<unsigned int>(CODE));
				escaped += sequence;
			}
			else
			{
				escaped += CHARACTER;
			}
		}
		escaped += '"';
		return escaped;
	}
}
