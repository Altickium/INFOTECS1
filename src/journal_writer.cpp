#include "journal_writer.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>

class JournalWriterImpl : public JournalWriterInterface {
public:
    
    // constructos
    JournalWriterImpl() = default;

    JournalWriterImpl(const JournalWriterImpl& oth) = default;
    JournalWriterImpl(JournalWriterImpl&& oth) = default;

    
    JournalWriterImpl& operator=(const JournalWriterImpl& oth) = default;
    JournalWriterImpl& operator=(JournalWriterImpl&& oth) = default;

    JournalWriterImpl(const std::string& filename, 
               LoggingLevel startingLvl) 
        : JournalWriterInterface(startingLvl), 
          output(filename, std::ios::app) {}
    
    // functions
    void write(const std::string& formattedMessage) override {
        output << formattedMessage << std::endl;
    }

private:
    std::ofstream output;
};

std::unique_ptr<JournalWriterInterface> JournalWriterInterface::createJournalWriter(const std::string& filename, LoggingLevel startingLvl) {
    return std::make_unique<JournalWriterImpl>(filename, startingLvl);
}

JournalWriterInterface::JournalWriterInterface(LoggingLevel startingLvl) : _currentLvl(startingLvl) {}

void JournalWriterInterface::setLevel(LoggingLevel lvl) {
    std::lock_guard<std::mutex> lock(_mtx);
    _currentLvl = lvl;
}

void JournalWriterInterface::log(LoggingLevel lvl, const std::string& msg) {
    if (lvl < _currentLvl) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(_mtx);
    write(getJournalEntry(lvl, msg));
}

std::string JournalWriterInterface::getJournalEntry(LoggingLevel lvl, const std::string& msg) {
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << " [";
    
    switch(lvl) {
        case LoggingLevel::Error: 
            ss << "ERROR"; 
            break;
        case LoggingLevel::Warning: 
            ss << "WARNING"; 
            break;
        case LoggingLevel::Info: 
            ss << "INFO"; 
            break;
    }
    
    ss << "] " << msg;
    return ss.str();
}