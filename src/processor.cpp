#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    totald_ = LinuxParser::Jiffies() - prev_total_;
    ideld_ = LinuxParser::IdleJiffies() - prev_idle_;

    prev_total_ = LinuxParser::Jiffies();
    prev_idle_ = LinuxParser::IdleJiffies();

    cpu_percentage_ = (totald_ - ideld_) / totald_;

    return cpu_percentage_; 
}