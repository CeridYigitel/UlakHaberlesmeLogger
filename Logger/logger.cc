#include "logger.h"
#include <stdio.h>
#include <ctime>


void Logger::EnableFileOutput(const char* new_filepath){
	Logger& logger_instance = get_instance();
	logger_instance.filepath = new_filepath;
	logger_instance.EnableFileOutputInstance();
}

void Logger::WriteHeader(SWVersion &sw){
	get_instance().WriteHearderInstance(sw);
}

//Returns a static logger instance.
Logger& Logger::get_instance(){
	static Logger logger;
	return logger;
}

//To be able writing to a file, opens file for output at the end of a file.
void Logger::EnableFileOutputInstance(){
	if (file != 0)
	{
		fclose(file);
	}
	file = fopen(filepath, "a");

	if (file == 0)
	{
		printf("Logger: Failed to open file at %s", filepath);
	}
}

//Frees up the file pointer when program ends.
void Logger::FreeFile(){
	if (file)
		{
			fclose(file);
			file = nullptr;
		}
}

//This method writes current time in UTC format.
//Writes given software version to the header.
//Checks is there any file to write log, if there is,it also writes in the file.
void Logger::WriteHearderInstance(SWVersion &sw){

	std::time_t current_time = std::time(0);
	std::tm* timestamp = std::gmtime(&current_time);
	char buffer[80];
	strftime(buffer, 80, "%c", timestamp);

	printf("\n\n>>==== Date (UTC): %s\nVersion: %u.%u.%u ====<<\n", buffer,sw.major_,sw.minor_,sw.patch_);

	if (file !=0)
	{
		fprintf(file,"\n\n>>==== Date (UTC): %s\nVersion: %u.%u.%u ====<<\n", buffer,sw.major_,sw.minor_,sw.patch_);
	}

}




