#pragma once

#include <string>
#include <memory>
#include <mutex>

/*
    Enum for log levels of our app
    DEFAULT: INFO
*/
enum class LoggingLevel {
    Error,
    Warning,
    Info
};

/*
    Interface for the journal writer
*/
class JournalWriterInterface {
public:
    static std::unique_ptr<JournalWriterInterface> createJournalWriter(const std::string& filename, LoggingLevel startingLvl);
    
    void setLevel(LoggingLevel lvl);

    // virtual in case of implementation for socket
    virtual void log(LoggingLevel lvl, const std::string& msg);
    
    virtual ~JournalWriterInterface() = default;

protected:
    JournalWriterInterface(LoggingLevel startingLvl);

    // virtual in case of implementation for socket
    virtual void write(const std::string& formattedMsg) = 0;

    // function to add current time and logging lvl to message
    std::string getJournalEntry(LoggingLevel lvl, const std::string& msg);

private:
    LoggingLevel _currentLvl;
    std::mutex _mtx;
};