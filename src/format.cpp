#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int secs, mins, hours;
    string result = "";

    secs = (seconds % 3600) % 60;
    mins = (seconds % 3600) / 60;
    hours = (seconds / 3600);

    if (hours < 10){
        result = "0" + std::to_string(hours);
    }
    else {
        result = std::to_string(hours);
    }
    if (mins < 10){
        result += ":0" + std::to_string(mins);
    }
    else {
        result += ":" + std::to_string(mins);
    }
    if (secs < 10){
        result += ":0" + std::to_string(secs);
    }
    else {
        result += ":" + std::to_string(secs);
    }

    return result; 
}