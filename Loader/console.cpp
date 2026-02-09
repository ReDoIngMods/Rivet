#include "console.h"
#include "loggerManager.h"

Rivet::Logger* Rivet::GLogger = LoggerManager::GetInstance().GetLogger("RivetLoader");