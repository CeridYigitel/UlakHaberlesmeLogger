#include "logger.h"
#include <stdio.h>
#include <ctime>


void Logger::EnableFileOutput(const char* new_filepath="log.txt"){
	Logger& logger_instance = get_instance();
	logger_instance.filepath = new_filepath;
	logger_instance.EnableFileOutputInstance();
}
void Logger::WriteHeader(SWVersion *sw){
	get_instance().WriteHearderInstance(sw);
}

Logger& Logger::get_instance(){
	static Logger logger;
	return logger;
}
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

void Logger::FreeFile(){
	if (file != 0)
		{
			fclose(file);
			file = 0;
		}
}

void Logger::WriteHearderInstance(SWVersion *sw){

	std::time_t current_time = std::time(0);
	std::tm* timestamp = std::gmtime(&current_time);
	char buffer[80];
	strftime(buffer, 80, "%c", timestamp);

	printf("\n\n>>==== Date (UTC): %s\nVersion: %u.%u.%u ====<<\n", buffer,sw->major_,sw->minor_,sw->patch_);

	if (file !=0)
	{
		fprintf(file,"\n\n>>==== Date (UTC): %s\nVersion: %u.%u.%u ====<<\n", buffer,sw->major_,sw->minor_,sw->patch_);
	}

}


