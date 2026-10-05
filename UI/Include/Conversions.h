#ifndef FMTWRAPPER_CONVERSIONS_HEADER
#define FMTWRAPPER_CONVERSIONS_HEADER

#include <string>
#include <vector>

#include <msclr/marshal_cppstd.h>

namespace Wrapper
{
	// DocString: Conversions
	/**
	@brief Conversions between .NET and std types, shared by the FMTForm sources.

	Incoming conversions turn nullptr into empty values: the Core then refuses the
	operation with its own messages. Outgoing conversions keep the decoding their call
	sites relied on: toManagedList decodes in the ANSI code page, like
	gcnew System::String, while fromUtf8 decodes UTF-8.
	*/
	namespace Conversions
	{
		// DocString: Conversions::toStdString
		/**
		@brief Converts a managed string to a std::string.

		@param[in] p_value Managed string, possibly nullptr.

		@return The converted string, empty for nullptr.
		*/
		inline std::string toStdString(System::String^ p_value)
		{
			return p_value == nullptr
				? std::string()
				: msclr::interop::marshal_as<std::string>(p_value);
		}

		// DocString: Conversions::toStdVector
		/**
		@brief Converts a managed list of strings to a std::vector.

		@param[in] p_values Managed list, possibly nullptr. Its nullptr items become
		empty strings.

		@return The converted strings, none for nullptr.
		*/
		inline std::vector<std::string> toStdVector(
			System::Collections::Generic::List<System::String^>^ p_values)
		{
			std::vector<std::string> converted;

			if (p_values != nullptr)
			{
				for each (System::String^ value in p_values)
				{
					converted.push_back(toStdString(value));
				}
			}

			return converted;
		}

		// DocString: Conversions::toManagedList
		/**
		@brief Converts std::strings to a managed list.

		Each string is decoded in the ANSI code page, like gcnew System::String.

		@param[in] p_values Strings to convert.

		@return The managed list.
		*/
		inline System::Collections::Generic::List<System::String^>^ toManagedList(
			const std::vector<std::string>& p_values)
		{
			System::Collections::Generic::List<System::String^>^ converted =
				gcnew System::Collections::Generic::List<System::String^>();

			for (const std::string& VALUE : p_values)
			{
				converted->Add(gcnew System::String(VALUE.c_str()));
			}

			return converted;
		}

		// DocString: Conversions::fromUtf8
		/**
		@brief Converts a UTF-8 encoded std::string to a managed string.

		Accented and other non-ASCII characters are preserved.

		@param[in] p_value UTF-8 encoded string.

		@return The managed string.
		*/
		inline System::String^ fromUtf8(const std::string& p_value)
		{
			array<System::Byte>^ bytes = gcnew array<System::Byte>(static_cast<int>(p_value.size()));

			for (size_t i = 0; i < p_value.size(); ++i)
			{
				bytes[i] = static_cast<System::Byte>(p_value[i]);
			}

			return System::Text::Encoding::UTF8->GetString(bytes);
		}
	}
}

#endif // FMTWRAPPER_CONVERSIONS_HEADER
