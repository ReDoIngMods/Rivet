#include "console.h"
#include "loggerManager.h"

Rivet::Logger* Rivet::GLogger = Rivet::LoggerManager::GetInstance().getLogger("RivetLoader");