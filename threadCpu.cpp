#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>

double get_cpu_usage() {
    std::ifstream stat_file("/proc/stat");
    std::string line;
    std::getline(stat_file, line);

    // Extracting total CPU usage information
    std::istringstream iss(line);
    std::string cpu_name;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    iss >> cpu_name >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    // Calculating idle and total time
    double idle_time = idle + iowait;
    double total_time = user + nice + system + idle + iowait + irq + softirq + steal;

    // Waiting for a second
    usleep(1000000);

    // Reading stat file again after 1 sec
    std::ifstream stat_file_2("/proc/stat");
    std::getline(stat_file_2, line);

    // Extracting total CPU usage information again
    std::istringstream iss_2(line);
    unsigned long long user_2, nice_2, system_2, idle_2, iowait_2, irq_2, softirq_2, steal_2, guest_2, guest_nice_2;
    iss_2 >> cpu_name >> user_2 >> nice_2 >> system_2 >> idle_2 >> iowait_2 >> irq_2 >> softirq_2 >> steal_2 >> guest_2 >> guest_nice_2;

    // Calculating idle and total time again
    double idle_time_2 = idle_2 + iowait_2;
    double total_time_2 = user_2 + nice_2 + system_2 + idle_2 + iowait_2 + irq_2 + softirq_2 + steal_2;

    // Calculating CPU usage percentage
    double usage_percentage = 100.0 * (1.0 - ((idle_time_2 - idle_time) / (total_time_2 - total_time)));

    return usage_percentage;
}

int main() {
    double cpu_usage = get_cpu_usage();
    std::cout << "CPU Usage: " << cpu_usage << "%" << std::endl;
    return 0;
}
