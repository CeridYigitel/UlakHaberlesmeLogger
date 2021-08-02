#include "logger.h"
#include <stdio.h>
#include <ctime>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <chrono>
#include <string>
#include<json-c/json.h>
//Port-ServerIp-Filepath (for now) stands inside Settings.json file, this function reads and parses the json file.
void Logger::ReadJson(){
	FILE *fp;
	char buffer[1024];
	struct json_object *parsed_json;
	//This file path must be your "Settings.json" file path.
	fp = fopen("/home/cerid/eclipse-workspace/Logger/Settings.json","r");
		fread(buffer, 1024, 1, fp);
		fclose(fp);
	parsed_json = json_tokener_parse(buffer);

	json_object_object_get_ex(parsed_json, "Port", &pport_);
	json_object_object_get_ex(parsed_json, "ServerIp", &pserverip_);
	json_object_object_get_ex(parsed_json, "Filepath", &pfilepath_json_);
}

void Logger::Init(unsigned int output_type){
	output_type_=output_type;
	ReadJson();

	if((kFile& (output_type_))>>1){
		EnableFileOutput();
	}

	if((kUdp &(output_type_))>>2){
		EnableUdpOutput();
	}
}

//To be able writing to UDP socket.
void Logger::EnableUdpOutput(){
	// Creating socket file descriptor
	if ( (sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	int opt = 1;
	if( setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt)) < 0 )
	{
	    perror("setsockopt");
	    exit(EXIT_FAILURE);
	}
	memset(&servaddr_, 0, sizeof(servaddr_));

	// Filling server information
	servaddr_.sin_family = AF_INET;
	servaddr_.sin_port = htons(json_object_get_int(pport_));
	servaddr_.sin_addr.s_addr = inet_addr(json_object_get_string(pserverip_));
}

//To be able writing to a file, opens file for output at the end of a file.
void Logger::EnableFileOutput(){
	get_instance().pfilepath_ = json_object_get_string(pfilepath_json_);

	if (get_instance().pfile_ != 0)
	{
		fclose(get_instance().pfile_);
	}
	get_instance().pfile_ = fopen(get_instance().pfilepath_, "a");

	if (get_instance().pfile_ == 0)
	{
		printf("Logger: Failed to open file at %s", get_instance().pfilepath_);
	}
}

//This method writes current time in UTC format.
//Writes given software version to the header.
//Checks is there any file to write log, if there is,it also writes in the file.
void Logger::WriteHeader(SWVersion &sw){
	std::time_t current_time = std::time(0);
	std::tm* timestamp = std::gmtime(&current_time);
	char timestamp_string[80];
	strftime(timestamp_string, 80, "%c", timestamp);

	if(kStdoutput &output_type_){
		printf("\n\n>>==== Date (UTC): %s\nVersion: %u.%u.%u ====<<\n", timestamp_string,sw.major,sw.minor,sw.patch);
	}
	if((kFile& (output_type_))>>1 && pfile_){
		fprintf(pfile_,"\n\n>>==== Date (UTC): %s\nVersion: %u.%u.%u ====<<\n", timestamp_string,sw.major,sw.minor,sw.patch);
	}
}

//Returns a static logger instance.
Logger& Logger::get_instance(){
	static Logger logger;
	return logger;
}

//Frees up the file pointer when program ends.
void Logger::FreeFile(){
	if (pfile_)
		{
			fclose(pfile_);
			pfile_ = nullptr;
		}
}

//Merge given string list.
std::string Logger::Merge(std::initializer_list<std::string> strList)
	{
	std::string ret = "";
	for (std::string s : strList) {
		ret += s;
	}
	return ret;
	}




