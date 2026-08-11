#include "FMTEmptyTheme.h"

namespace Exception
	{
	FMTEmptyTheme::FMTEmptyTheme(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTempty_theme,
				FMTlev::FMT_range,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un thème a été déclaré sans contenir de valeurs utilisables.",
				"A theme was declared without containing usable values.")
		{
		}
	}