#include "journal_writer.h"

#include <iostream>
#include <queue>
#include <thread>
#include <condition_variable>

// queue and respective mutex and conditional variable (in case of many writers)
std::queue<std::pair<LoggingLevel, std::string>> msgQueue;
std::mutex queueMtx;
std::condition_variable journalFinished;

// exit flag
bool EXIT = false;

void journalWorker(std::unique_ptr<JournalWriterInterface> journaler) {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMtx);
        journalFinished.wait(lock, []{ return !msgQueue.empty() || EXIT; });
        
        if (EXIT && msgQueue.empty()) {
            return;
        }
        
        auto task = msgQueue.front();
        msgQueue.pop();
        lock.unlock();
        
        // in case of heavy work to not freeze
        journaler->log(task.first, task.second);
    }
}

int main(int argc, char* argv[]) {

    /*
        Argument parsing and validating
    */
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <logfile_name> <logging_level>" << std::endl;
        return 1;
    }
    LoggingLevel defaultLvl;
    std::string levelStr = argv[2];
    if (levelStr == "ERROR") {
        defaultLvl = LoggingLevel::Error;
    } else if (levelStr == "WARNING") {
        defaultLvl = LoggingLevel::Warning;
    } else if (levelStr == "INFO") {
        defaultLvl = LoggingLevel::Info;
    } else {
        std::cerr << "Invalid log level" << std::endl;
        return 1;
    }
    
    auto writer = JournalWriterInterface::createJournalWriter(argv[1], defaultLvl);
    std::thread worker(journalWorker, std::move(writer));
    
    /*
        Business logic start
    */
    std::string input;
    while (std::getline(std::cin, input) && input.size() > 0) {    // checking for 0 size to detect program end
        LoggingLevel lvl = defaultLvl;
        std::string msg = input;
        
        if (input.substr(0, 6) == "ERROR:") {
            lvl = LoggingLevel::Error;
            msg = msg.substr(6);
        } else if (input.substr(0, 9) == "WARNING:") {
            lvl = LoggingLevel::Warning;
            msg = msg.substr(9);
        } else if (input.substr(0, 5) == "INFO:") {
            lvl = LoggingLevel::Info;
            msg = msg.substr(5);
        }
        
        {
            std::lock_guard<std::mutex> lock(queueMtx);
            msgQueue.push({lvl, msg});
        }
        journalFinished.notify_one();
    }
    
    {
        std::lock_guard<std::mutex> lock(queueMtx);
        EXIT = true;
    }
    journalFinished.notify_one();
    worker.join();
    
    return 0;
}