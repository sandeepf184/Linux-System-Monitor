#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key, value;
  float memTotal, memFree, memUtilized;
  
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kMeminfoFilename);

  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "MemTotal:"){
        memTotal = std::stof(value);
      }
      if(key == "MemFree:"){
        memFree = std::stof(value);
        break;
      }   
    }
    memUtilized = (memTotal - memFree) / memTotal;
    return memUtilized;
  }

  return 0.0; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long upTime;

  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
    return upTime;
  }

  return 0; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return (LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies());
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  long total_time, utime, stime, cutime, cstime;
  string line, value;
  vector<string> pid_data;

  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +LinuxParser::kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
      pid_data.push_back(value);
    }
  }

  //https://stackoverflow.com/questions/39066998/what-are-the-meaning-of-values-at-proc-pid-stat
  utime = std::stol(pid_data[13]);
  stime = std::stol(pid_data[14]);
  cutime = std::stol(pid_data[15]);
  cstime = std::stol(pid_data[16]);

  total_time = utime + stime + cutime + cstime;

  return total_time / sysconf(_SC_CLK_TCK); 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  return (std::stol(LinuxParser::CpuUtilization()[kUser_]) + 
          std::stol(LinuxParser::CpuUtilization()[kNice_]) +
          std::stol(LinuxParser::CpuUtilization()[kSystem_]) + 
          std::stol(LinuxParser::CpuUtilization()[kIRQ_]) +
          std::stol(LinuxParser::CpuUtilization()[kSoftIRQ_]) + 
          std::stol(LinuxParser::CpuUtilization()[kSteal_]) + 
          std::stol(LinuxParser::CpuUtilization()[kGuest_]) + 
          std::stol(LinuxParser::CpuUtilization()[kGuestNice_])); 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {   
  return (std::stol(LinuxParser::CpuUtilization()[kIdle_]) + std::stol(LinuxParser::CpuUtilization()[kIOwait_])); 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string cpu;
  vector<string> cpu_states(10);
  
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);

  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
   
    for(int i = 0; i <= 9; i++){
      linestream >> cpu_states[i];
    }

    return cpu_states;
  }
  return {}; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key, value;
  int totalProcesses;
  
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);

  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "processes"){
        totalProcesses = std::stoi(value);
        break;
      }
    }
    return totalProcesses;
  }    
  return 0; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key, value;
  int runningProcesses;
  
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);

  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "procs_running"){
        runningProcesses = std::stoi(value);
        break;
      }
    }
    return runningProcesses;
  } 
  return 0; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line; 
  
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, line); 
  }  
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  int ram;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          ram = int(std::stof(value) / float(1024));
          return std::to_string(ram);
        }
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, value;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> value;
        return value;
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line, key, value;
  std::ifstream stream(LinuxParser::kPasswordPath);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    if (line.find("x:" + LinuxParser::Uid(pid)) != line.npos) {
      return line.substr(0, line.find(":"));
    }
  }
  return string(); 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> pid_data;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      pid_data.push_back(value);
    }
  }
  return (std::stol(pid_data[21]) / sysconf(_SC_CLK_TCK)); 
}
