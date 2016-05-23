#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define DISTRIBUTION_PATH "/etc/issue.net"
#define KERNEL_PATH "/proc/version_signature"
#define COMPUTER_NAME_PATH "/etc/hostname"
#define CPU_INFO_PATH "/proc/cpuinfo"

#define MESSAGE "No information"


// struct contain fields with main information about CPU
struct cpu_information{
  string model_name;
  unsigned cores;
  unsigned cache_size;
};

// this class contain main information about computer
class computer_information{
private:
  string kernel;               // contain information about system kernel
  string distribution;         // contain name of system distribution
  string computer_name;        // contain computer name
  cpu_information cpu_info;    // contain main information about CPU

public:
  computer_information();
  void output();

private:
  string get_value(string);     // get value from CPU_INFO_PATH
  unsigned to_unsigned(string); // convert string to unsigned
  void get_kernel();            // get information from KERNEL_PATH
  void get_distribution();      // get information from DISTRIBUTION_PATH
  void get_computer_name();     // get information from COMPUTER_NAME_PATH
  void get_cpu_information();   // get information from CPU_INFO_PATH
};

int main(){
  computer_information info;
  info.output();

  return 0;
}

computer_information::computer_information(){
  get_kernel();
  get_distribution();
  get_computer_name();
  get_cpu_information();
}

string computer_information::get_value(string buffer){
  unsigned long colon = buffer.find(':');
  return buffer.substr(colon + 2);
}

unsigned computer_information::to_unsigned(string buffer) {
  unsigned number;
  stringstream stream;
  stream << buffer.c_str();
  stream >> number;
  return number;
}

void computer_information::output(){
  cout << "Computer name: " << computer_name << endl
  << "Distribution: " << distribution << endl
  << "Kernel: " << kernel << endl << endl
  << "CPU information:" << endl
  << "Model: " << cpu_info.model_name << endl
  << "Cores: " << cpu_info.cores << endl
  << "Cache size: " << cpu_info.cache_size << endl;
}

void computer_information::get_kernel(){
  ifstream fin(KERNEL_PATH);
  if(!fin.is_open()){
    kernel = MESSAGE;
    perror(KERNEL_PATH);
    return;
  }

  getline(fin, kernel);
  fin.close();
}

void computer_information::get_distribution(){
  ifstream fin(DISTRIBUTION_PATH);
  if(!fin.is_open()){
    distribution = MESSAGE;
    perror(DISTRIBUTION_PATH);
    return;
  }

  getline(fin, distribution);
  fin.close();
}

void computer_information::get_computer_name(){
  ifstream fin(COMPUTER_NAME_PATH);
  if(!fin.is_open()){
    computer_name = MESSAGE;
    perror(COMPUTER_NAME_PATH);
    return;
  }

  getline(fin, computer_name);
  fin.close();
}

void computer_information::get_cpu_information(){
  ifstream fin(CPU_INFO_PATH);
  if(!fin.is_open()){
    cpu_info.model_name = MESSAGE;
    cpu_info.cache_size = 0;
    cpu_info.cores = 0;
    perror(CPU_INFO_PATH);
    return;
  }

  string buffer;
  while(!fin.eof()){
    getline(fin, buffer);
    if(fin.eof()) break;

    if(buffer.empty()) break;

    unsigned long first_space = buffer.find(' ', 0);
    if(buffer.substr(0, first_space) == "cpu"){
      if(buffer.substr(first_space + 1, buffer.find('\t', first_space)
                                        - first_space - 1) == "cores"){
        cpu_info.cores = to_unsigned(get_value(buffer));
      }
    }

    if(buffer.substr(0, first_space) == "model"){
      if(buffer.substr(first_space + 1, buffer.find('\t', first_space + 1)
                                        - first_space - 1) == "name"){
        cpu_info.model_name = get_value(buffer);
      }
    }

    if(buffer.substr(0, first_space) == "cache"){
      cpu_info.cache_size = to_unsigned(get_value(buffer));
    }
  }
}