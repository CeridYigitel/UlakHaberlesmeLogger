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

#include<json-c/json.h>

//This enum holds the log types.
enum LogType
{
	Info, Warning, Error
};
//This enum holds Output types.
enum OutputType{

	kStdoutput=1, kFile=2, kUdp=4
};

//Software version that we need for header.
struct SWVersion{
	unsigned int major;
	unsigned int minor;
	unsigned int patch;
};


class Logger
{
public:
	void ReadJson();

	void EnableUdpOutput();

	void EnableFileOutput();

	void WriteHeader(SWVersion &sw);

	void Init(unsigned int output_type);

	//This method writes current time in UTC format.
	//Writes logs with given parameters.
	//Checks is there any file to write log, if there is,it also writes in the file.
	template<typename... Args>
	void Log(LogType type, const char* message, Args... args){
		std::string log_type;
		switch(type){
		case Info:
			log_type="Info";
			break;
		case Warning:
			log_type="Warning";
			break;
		case Error:
			log_type="Error";
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
		str.append(log_type.c_str());
		str.append("\t");
		str.append(message);
		str.append(" ");
		str.append(Stringify(args...));
		str.append("\n");
		if(kStdoutput &output_type_){
			printf("%s",str.c_str());
		}
		if((kFile& (output_type_))>>1 && pfile_){
			fprintf(pfile_,"%s",str.c_str());
		}
		if((kUdp &(output_type_))>>2){
			sendto(sockfd_, (const char *)str.c_str(), strlen(str.c_str()),
				MSG_CONFIRM, (const struct sockaddr *) &servaddr_,
				sizeof(servaddr_));
		}
	}

	static Logger& get_instance();

private:
	const char* pfilepath_ = nullptr;
	FILE* pfile_ = nullptr;
	std::mutex log_mutex_;
	int sockfd_;
	struct sockaddr_in servaddr_;
	unsigned int output_type_;
	struct json_object *pport_,*pserverip_,*pfilepath_json_;

	//We have singleton design pattern, there will be just 1 instance, class objects should not be created outside class.
	Logger() {
	}

	//Destructor closes the file.
	~Logger()
	{
		FreeFile();
	}

	void FreeFile();
	//Converts the given value to string.
	template<typename T>
	std::string toString(T value)
	{
	    std::ostringstream oss;
	    oss << value;
	    return oss.str();
	}

	std::string Merge(std::initializer_list<std::string> strList);

	//Calls Merge for all arguments.
	template< typename ... Args >
	std::string Stringify(const Args& ... args)
	{
	    return Merge({toString(args)...});
	}
};

#endif /* LOGGER_H_ */
