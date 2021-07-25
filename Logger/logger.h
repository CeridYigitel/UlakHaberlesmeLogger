/*
 * logger.h
 *
 *  Created on: Jul 16, 2021
 *      Author: cerid
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <string>
#include <ctime>

//Enum holds the log types.
enum LogType
{
	Info, Warning, Error
};
//Software version that we need for header.
struct SWVersion{
	unsigned int major_;
	unsigned int minor_;
	unsigned int patch_;
};


class Logger
{
public:
	static void EnableFileOutput(const char* new_filepath="log.txt");
	static void WriteHeader(SWVersion &sw);

	template<typename... Args>
	static void Log(LogType type, const char* message, Args... args){
		get_instance().LogInstance(type,message,args...);
	}

private:
	const char* filepath = nullptr;
	FILE* file = nullptr;

	//We have singleton design pattern, there will be just 1 instance, class objects should not be created outside class.
	Logger() {
	}
	//Destructor closes the file.
	~Logger()
	{
		FreeFile();
	}

	static Logger& get_instance();


	//This method writes current time in UTC format.
	//Writes logs with given parameters.
	//Checks is there any file to write log, if there is,it also writes in the file.
	template<typename... Args>
	void LogInstance(LogType type, const char* message, Args... args){
		std::string logtype;
		switch(type){
		case Info:
			logtype="Info";
			break;
		case Warning:
			logtype="Warning";
			break;
		case Error:
			logtype="Error";
			break;
		}

		std::time_t current_time = std::time(0);
		std::tm* timestamp = std::gmtime(&current_time);
		char buffer[80];
		strftime(buffer, 80, "%c", timestamp);
		printf("%s\t", buffer);
		printf("[%s]  ",logtype.c_str());
		printf(message, args...);
		printf("\n");

		if (file)
		{
			fprintf(file, "%s\t", buffer);
			fprintf(file,"[%s]\t",logtype.c_str());
			fprintf(file, message, args...);
			fprintf(file, "\n");
		}
	}

	void EnableFileOutputInstance();

	void FreeFile();

	void WriteHearderInstance(SWVersion &sw);
};





#endif /* LOGGER_H_ */
