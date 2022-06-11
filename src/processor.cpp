#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    totald = LinuxParser::Jiffies() - prevTotal;
    ideld = LinuxParser::IdleJiffies() - prevIdle;

    prevTotal = LinuxParser::Jiffies();
    prevIdle = LinuxParser::IdleJiffies();

    cpuPercentage = (totald - ideld) / totald;

    return cpuPercentage; 
}