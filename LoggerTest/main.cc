#include "logger.h"
#include <iostream>
#include <string>
#include <thread>
#include <stdlib.h>
#include <chrono>
#include <ctime>
void LoggerTest(int thread_index,int duration_in_seconds){
	time_t start_time=time(nullptr);

	while(true){
		if(time(nullptr)-start_time > duration_in_seconds)
			break;
		Logger::Log(static_cast<LogType>(rand() % 3),"Logging from thread: %d",thread_index);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void SetSWVersion(unsigned int new_major,unsigned int new_minor,unsigned int new_patch){
	struct SWVersion sw;
	sw.major_=new_major;
	sw.minor_=new_minor;
	sw.patch_=new_patch;
	Logger::WriteHeader(&sw);
}

int main(){
	static int thread_run_duration=1;
	static int number_of_threads=3;

	Logger::EnableFileOutput("/home/cerid/eclipse-workspace/LoggerTest/log.txt");
	SetSWVersion(5,6,3);

	std::thread threads[number_of_threads];

	for(int t_indice=0;t_indice<number_of_threads;t_indice++){
		threads[t_indice]= std::thread(LoggerTest,t_indice,thread_run_duration);
	}

	for(int t_indice=0;t_indice<number_of_threads;t_indice++){
		threads[t_indice].join(); // @suppress("Method cannot be resolved")
	}

	int a=1881;
	Logger::Log(Info,"Logger Ready To Use %d",a);

	Logger::Log(Warning,"Logger Ready To Use %d",a);

	return 0;
}
