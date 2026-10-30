/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTObject.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTExceptionHandler.h"
#include "FMTLogger.h"
#include "FMTMask.h"
#include "FMTTheme.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

// FMTObject is the base of most classes of FMT: every object of the process shares its exception handler and its logger,
// which are static. FMT raises an error through the handler, which throws it, or logs it when it is a warning or an
// error set to warnings. The cases raise an error of FMT, FMTmissingyield, through FMTYields::get, which returns 0 when the
// error is only a warning, and read what FMT logs through a logger that keeps it. Since the handler and the logger are
// shared, a case that changes them gives FMT back a default handler and a default logger when it ends (Tests/ETAT.md,
// section 7).

namespace
{
    // A logger that keeps what FMT logs.
    class KeptLogger final : public Logging::FMTLogger
    {
    public:
        explicit KeptLogger(const std::shared_ptr<std::string>& p_text) : Logging::FMTLogger(), m_text(p_text)
        {
        }
        KeptLogger(const KeptLogger&) = default;
        KeptLogger& operator=(const KeptLogger&) = delete;
#ifdef FMTWITHOSI
        Logging::FMTLogger* clone() const override
        {
            return new KeptLogger(*this);
        }
#endif
        std::unique_ptr<Logging::FMTLogger> Clone() const override
        {
            return std::unique_ptr<Logging::FMTLogger>(new KeptLogger(*this));
        }
    protected:
        void _cout(const char* p_message) const override
        {
            *m_text += p_message;
        }
    private:
        std::shared_ptr<std::string> m_text;
    };

    // Gives every object a new default exception handler, whose counts of warnings start at zero, and a logger that keeps
    // what FMT logs; gives them back a default handler and a default logger when it ends.
    class KeptLog
    {
    public:
        KeptLog() : m_object(), m_text(std::make_shared<std::string>())
        {
            m_object.setDefaultExceptionHandler();
            m_object.passInLogger(std::unique_ptr<Logging::FMTLogger>(new KeptLogger(m_text)));
        }
        ~KeptLog()
        {
            m_object.setDefaultLogger();
            m_object.setDefaultExceptionHandler();
        }
        KeptLog(const KeptLog&) = delete;
        KeptLog& operator=(const KeptLog&) = delete;
        const std::string& getText() const
        {
            return *m_text;
        }
        Core::FMTObject& getObject()
        {
            return m_object;
        }
    private:
        Core::FMTObject m_object;
        std::shared_ptr<std::string> m_text;
    };

    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    // VOLUME of the attribute A1 only.
    Core::FMTYields getYields()
    {
        Core::FMTAgeYieldHandler handler(Core::FMTMask("A1", getThemes()));
        handler.setYieldValues("VOLUME", { 10, 20 }, { 100.0, 200.0 });
        Core::FMTYields yields;
        yields.pushBackAgeHandler(Core::FMTMask("A1", getThemes()), handler);
        yields.update();
        return yields;
    }

    // VOLUME of a development of the attribute A2, which no handler holds: FMTYields raises FMTmissingyield.
    double getMissingYield(const Core::FMTYields& p_yields)
    {
        const Core::FMTDevelopment DEVELOPMENT(Core::FMTMask("A2", getThemes()), 10, 0, 1);
        const Core::FMTYieldRequest REQUEST(DEVELOPMENT);
        return p_yields.get(REQUEST, "VOLUME");
    }

    size_t count(const std::string& p_text, const std::string& p_part)
    {
        size_t found = 0;
        size_t position = p_text.find(p_part);
        while (position != std::string::npos)
        {
            ++found;
            position = p_text.find(p_part, position + p_part.size());
        }
        return found;
    }

    const std::string MISSING_YIELD = "FMTexc(" + std::to_string(static_cast<int>(Exception::FMTexc::FMTmissingyield)) + ")";
}

TEST(FMTObject, GivenAnObjectWhenAnotherObjectAndACopyAreConstructedThenTheyShareItsExceptionHandlerAndItsLogger)
{
    // Arrange
    const Core::FMTObject first;
    const Exception::FMTExceptionHandler* handler = first.getExceptionHandler();
    const Logging::FMTLogger* logger = first.getLogger();

    // Act
    const Core::FMTObject second;
    const Core::FMTObject copy(first);

    // Assert
    EXPECT_NE(handler, nullptr);
    EXPECT_NE(logger, nullptr);
    EXPECT_EQ(second.getExceptionHandler(), handler);
    EXPECT_EQ(second.getLogger(), logger);
    EXPECT_EQ(copy.getExceptionHandler(), handler);
    EXPECT_EQ(copy.getLogger(), logger);
}

TEST(FMTObject, GivenAnErrorThatIsNotSetToWarningsWhenFMTRaisesItThenItIsThrownWithoutBeingLogged)
{
    // Arrange
    const KeptLog log;
    const Core::FMTYields yields = getYields();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        getMissingYield(yields);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTmissingyield));
    EXPECT_EQ(log.getText().find(MISSING_YIELD), std::string::npos);
}

// FMT sets to warnings the errors of FMTExceptionHandler::getErrorsToIgnore when a user asks it: the error is logged,
// marked "Ignoring", and the method goes on.
TEST(FMTObject, GivenAnErrorSetToWarningsWhenFMTRaisesItThenLogsItAsIgnoredWithoutThrowing)
{
    // Arrange
    KeptLog log;
    const Core::FMTYields yields = getYields();
    log.getObject().setErrorsToWarnings({ Exception::FMTexc::FMTmissingyield });

    // Act
    double value = -1.0;
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        value = getMissingYield(yields);
        });

    // Assert
    EXPECT_TRUE(raised.empty());
    EXPECT_DOUBLE_EQ(value, 0.0);
    EXPECT_NE(log.getText().find(MISSING_YIELD), std::string::npos);
    EXPECT_NE(log.getText().find("Ignoring: "), std::string::npos);
}

TEST(FMTObject, GivenTheErrorsSetToWarningsEmptiedWhenFMTRaisesTheErrorThenItIsThrownAgain)
{
    // Arrange
    KeptLog log;
    const Core::FMTYields yields = getYields();
    log.getObject().setErrorsToWarnings({ Exception::FMTexc::FMTmissingyield });

    // Act
    log.getObject().setErrorsToWarnings({});
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        getMissingYield(yields);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTmissingyield));
}

// A warning is logged until it has been logged once more than the maximum, then a line says it reached its maximum,
// then it is no longer logged: with a maximum of 2, three times out of six. The documentation of
// setMaxWarningsBeforeSilenced speaks of the number of warnings raised before they are silenced.
TEST(FMTObject, GivenAMaximumOfWarningsWhenFMTRaisesTheSameWarningMoreOftenThenLogsItOnceMoreThanTheMaximumThenSilencesIt)
{
    // Arrange
    KeptLog log;
    const Core::FMTYields yields = getYields();
    log.getObject().setErrorsToWarnings({ Exception::FMTexc::FMTmissingyield });
    log.getObject().setMaxWarningsBeforeSilenced(2);

    // Act
    for (int raise = 0; raise < 6; ++raise)
    {
        getMissingYield(yields);
    }

    // Assert
    EXPECT_EQ(count(log.getText(), "Ignoring: "), 3u);
    EXPECT_EQ(count(log.getText(), MISSING_YIELD + " has reached the maximum number of times it can be raised."), 1u);
}

// FMT wraps an error in an FMTfunctionfailed in each method it goes through (raiseFromCatch); without nested exceptions,
// the caller receives the error itself.
TEST(FMTObject, GivenNestedExceptionsDisabledWhenAMethodOfFMTFailsThenTheCallerReceivesTheErrorItself)
{
    // Arrange
    KeptLog log;
    const Core::FMTYields yields = getYields();
    const std::vector<Exception::FMTexc> nested = Testing::getRaisedTypes([&]()
        {
        getMissingYield(yields);
        });

    // Act
    log.getObject().disableNestedExceptions();
    const std::vector<Exception::FMTexc> notNested = Testing::getRaisedTypes([&]()
        {
        getMissingYield(yields);
        });
    log.getObject().enableNestedExceptions();

    // Assert
    ASSERT_FALSE(nested.empty());
    EXPECT_EQ(nested.front(), Exception::FMTexc::FMTfunctionfailed);
    ASSERT_FALSE(notNested.empty());
    EXPECT_EQ(notNested.front(), Exception::FMTexc::FMTmissingyield);
}

TEST(FMTObject, GivenALoggerPassedInWhenFMTLogsThenEveryObjectLogsThroughIt)
{
    // Arrange
    KeptLog log;
    const Core::FMTObject other;

    // Act
    *other.getLogger() << "kept line" << "\n";

    // Assert
    EXPECT_NE(log.getText().find("kept line"), std::string::npos);
}

TEST(FMTObject, GivenTheProcessWhenGetAvailableMemoryThenReturnsAPositiveAmount)
{
    // Arrange
    const Core::FMTObject object;

    // Act
    const unsigned long long available = object.getAvailableMemory();

    // Assert
    EXPECT_GT(available, 0u);
}

// The location of FMTlib, the folder of the shared library that the executable loaded.
TEST(FMTObject, GivenTheLibraryWhenGetRuntimeLocationThenReturnsAFolderThatExists)
{
    // Arrange
    const Core::FMTObject object;

    // Act
    const std::string location = object.getRuntimeLocation();

    // Assert
    EXPECT_FALSE(location.empty());
    EXPECT_TRUE(std::filesystem::is_directory(location));
}
