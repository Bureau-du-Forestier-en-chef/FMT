#ifndef TESTING_TESTTOOLS_HEADER
#define TESTING_TESTTOOLS_HEADER

// Shared tools of the FMT test executables (Examples/C++, FMTWrapperCore/tests).
//
// A test executable returns:
// - 0 when every check passed;
// - 1 when a check failed or an exception escaped;
// - SKIP_RETURN_CODE when a feature it needs is not compiled (OSI, GDAL, ONNX):
//   ctest then reports the test as skipped (see Examples/C++/CMakeLists.txt).
//   A missing MOSEK is not a reason to skip: the test falls back on CLP.
//
// Typical use:
//
//   int main(int argc, char* argv[])
//   {
//       return Testing::runTest([&]()
//           {
//           Testing::Checker checker;
//           checker.checkNear(model.getObjValue(), EXPECTED_OBJECTIVE, 1.0, "objective");
//           return checker.exitCode();
//           });
//   }
//
// Expected values should come from the arguments given by the CSV files of
// Examples/C++/tests: a test is then seen failing by running it with a wrong value,
// without recompiling (see Examples/C++/tests/ETAT.md).

#include <cmath>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

namespace Testing
{
	// Exit code of a test whose required feature is not compiled.
	constexpr int SKIP_RETURN_CODE = 77;

	// Prints why the test is skipped and returns SKIP_RETURN_CODE, to be returned by main.
	inline int skip(const std::string& p_reason)
	{
		std::cout << "SKIPPED: " << p_reason << std::endl;
		return SKIP_RETURN_CODE;
	}

	// Runs the checks of a test: prints one line per check and counts the failures.
	class Checker
	{
	public:
		// Returns p_condition, so that a test can stop after a failed prerequisite.
		bool check(bool p_condition, const std::string& p_description)
		{
			std::cout << (p_condition ? "  ok      " : "  FAILED  ") << p_description << std::endl;
			if (!p_condition)
			{
				++m_failures;
			}
			return p_condition;
		}

		// Checks that p_actual is within p_tolerance of p_expected.
		bool checkNear(double p_actual, double p_expected, double p_tolerance, const std::string& p_description)
		{
			const bool WITHIN_TOLERANCE = std::abs(p_actual - p_expected) <= p_tolerance;
			return check(WITHIN_TOLERANCE, p_description + ": " + _toString(p_actual)
				+ " (expected " + _toString(p_expected) + " +/- " + _toString(p_tolerance) + ")");
		}

		int getFailures() const
		{
			return m_failures;
		}

		// Returns 0 when every check passed, 1 otherwise.
		int exitCode() const
		{
			if (m_failures > 0)
			{
				std::cerr << m_failures << " check(s) failed" << std::endl;
				return 1;
			}
			return 0;
		}

	private:
		int m_failures = 0;

		static std::string _toString(double p_value)
		{
			std::ostringstream stream;
			stream.precision(12);
			stream << p_value;
			return stream.str();
		}
	};

	// Replaces the p_destination folder by a copy of the p_source folder.
	// A test that writes into its project works on such a copy, never on Examples/Models.
	inline void copyProject(const std::filesystem::path& p_source, const std::filesystem::path& p_destination)
	{
		std::filesystem::remove_all(p_destination);
		std::filesystem::create_directories(p_destination);
		std::filesystem::copy(p_source, p_destination, std::filesystem::copy_options::recursive);
	}

	// Prints an exception, then its nested exceptions with an increasing indentation.
	inline void printException(const std::exception& p_exception, std::size_t p_depth = 0)
	{
		std::cerr << std::string(p_depth * 2, ' ') << p_exception.what() << std::endl;
		try
		{
			std::rethrow_if_nested(p_exception);
		}
		catch (const std::exception& nested)
		{
			printException(nested, p_depth + 1);
		}
		catch (...)
		{
			std::cerr << std::string((p_depth + 1) * 2, ' ') << "Unknown nested exception" << std::endl;
		}
	}

	// Runs the body of a test and returns its exit code, or 1 when an exception escapes it.
	inline int runTest(const std::function<int()>& p_body)
	{
		try
		{
			return p_body();
		}
		catch (const std::exception& error)
		{
			printException(error);
		}
		catch (...)
		{
			std::cerr << "Unknown exception" << std::endl;
		}
		return 1;
	}
}

#endif
