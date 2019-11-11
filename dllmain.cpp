
#include "Utils/GvsLog.h"
#include "metric/m4dMetricDatabase.h"

#ifdef _WIN32
m4d::MetricDatabase* m4d::MetricDatabase::m_instance = nullptr;
#endif

GvsLog& LOG = GvsLog::instance();
