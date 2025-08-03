
#include "error_handler.h"

namespace Oroto {

// Static member definitions
std::map<ErrorCode, std::string> ErrorHandler::errorMessages;
std::map<ErrorCode, std::function<void()>> ErrorHandler::errorHandlers;

} // namespace Oroto
