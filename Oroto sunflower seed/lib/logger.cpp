
#include "logger.h"

namespace Oroto {

// Static member definitions
std::mutex Logger::logMutex;
LogLevel Logger::currentLevel = LogLevel::INFO;
std::ofstream Logger::logFile;
bool Logger::initialized = false;

} // namespace Oroto
