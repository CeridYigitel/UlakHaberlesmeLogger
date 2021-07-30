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
#include <mutex>
#include <sstream>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

//Enum holds the log types.
enum LogType
{
	Info, Warning, Error
};

enum OutputType{

	Stdoutput=1, File=2, Udp=4
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
	void EnableUdpOutput();

	void EnableFileOutput();

	void WriteHeader(SWVersion &sw);

	void Init(unsigned int output_type);

	//This method writes current time in UTC format.
	//Writes logs with given parameters.
	//Checks is there any file to write log, if there is,it also writes in the file.
	template<typename... Args>
	void Log(LogType type, const char* message, Args... args){
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
		std::lock_guard<std::mutex> lock(log_mutex_);
		std::time_t current_time = std::time(0);
		std::tm* timestamp = std::gmtime(&current_time);
		char timestamp_string[80];
		strftime(timestamp_string, 80, "%c", timestamp);

		std::string str;
		str.append(timestamp_string);
		str.append("\t");
		str.append(logtype.c_str());
		str.append("\t");
		str.append(message);
		str.append(" ");
		str.append(Stringify(args...));
		str.append("\n");
		if(Stdoutput &output_type_){
			printf("%s",str.c_str());
		}
		if((File& (output_type_))>>1 && file){
			fprintf(file,"%s",str.c_str());
		}
		if((Udp &(output_type_))>>2){
			sendto(sockfd, (const char *)str.c_str(), strlen(str.c_str()),
				MSG_CONFIRM, (const struct sockaddr *) &servaddr,
				sizeof(servaddr));
		}
	}

	static Logger& get_instance();

private:
	const char* filepath = nullptr;
	FILE* file = nullptr;
	std::mutex log_mutex_;
	int sockfd;
	struct sockaddr_in servaddr;
	unsigned int output_type_;

	//We have singleton design pattern, there will be just 1 instance, class objects should not be created outside class.
	Logger() {
	}

	//Destructor closes the file.
	~Logger()
	{
		FreeFile();
	}

	void FreeFile();

	template<typename T>
	std::string toString(T value)
	{
	    std::ostringstream oss;
	    oss << value;
	    return oss.str();
	}

	std::string Merge(std::initializer_list<std::string> strList);

	template< typename ... Args >
	std::string Stringify(const Args& ... args)
	{
	    return Merge({toString(args)...});
	}
};

#endif /* LOGGER_H_ */
