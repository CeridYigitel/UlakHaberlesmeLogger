#include "logger.h"
#include <iostream>
#include <string>
#include <thread>
#include <stdlib.h>
#include <chrono>
#include <ctime>
void LoggerTest(int thread_index,int duration_in_seconds){
	time_t start_time=time(nullptr);
	int thread_sleep_ms=10;
	while(true){
		if(time(nullptr)-start_time > duration_in_seconds)
			break;
		Logger::get_instance().Log(static_cast<LogType>(rand() % 3),"Logging from thread: %d",thread_index);
		std::this_thread::sleep_for(std::chrono::milliseconds(thread_sleep_ms));
	}
}

void SetSWVersion(unsigned int new_major,unsigned int new_minor,unsigned int new_patch){
struct SWVersion sw;
sw.major_=new_major;
sw.minor_=new_minor;
sw.patch_=new_patch;
Logger::get_instance().WriteHeader(sw);
}

int main(){
	static int thread_run_duration=30;
	static int number_of_threads=5000;

	Logger::get_instance().EnableFileOutput();
	SetSWVersion(5,6,3);

	std::thread threads[number_of_threads];

	for(int t_indice=0;t_indice<number_of_threads;t_indice++){
		threads[t_indice]= std::thread(LoggerTest,t_indice,thread_run_duration);
	}

	for(int t_indice=0;t_indice<number_of_threads;t_indice++){
		threads[t_indice].join(); // @suppress("Method cannot be resolved")
	}

	int a=1881;
	Logger::get_instance().Log(Info,"Logger Ready To Use %d",a);

	Logger::get_instance().Log(Warning,"Logger Ready To Use %d",a);

	return 0;
}
