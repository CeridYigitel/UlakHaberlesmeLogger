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

void Logger::Init(unsigned int output_type){
	output_type_=output_type;
	if(Stdoutput &output_type_){
		printf("konsol ciktisi aktif\n");
	}

	if((File& (output_type_))>>1){
		printf("file ciktisi aktif\n");
		EnableFileOutput();
	}

	if((Udp &(output_type_))>>2){
		printf("udp ciktisi aktif\n");
		EnableUdpOutput();
	}
}


void Logger::EnableUdpOutput(){

	FILE *fp;
	char buffer[1024];
	struct json_object *parsed_json;
	struct json_object *Port,*ServerIp;

	fp = fopen("/home/cerid/eclipse-workspace/Logger/Settings.json","r");
		fread(buffer, 1024, 1, fp);
		fclose(fp);
	parsed_json = json_tokener_parse(buffer);

	json_object_object_get_ex(parsed_json, "Port", &Port);
	json_object_object_get_ex(parsed_json, "ServerIp", &ServerIp);

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	int opt = 1;
	if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt)) < 0 )
	{
	    perror("setsockopt");
	    exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(json_object_get_int(Port));
	servaddr.sin_addr.s_addr = inet_addr(json_object_get_string(ServerIp));
}

//To be able writing to a file, opens file for output at the end of a file.
void Logger::EnableFileOutput(){
	FILE *fp;
	char buffer[1024];
	struct json_object *parsed_json;
	struct json_object *Filepath;

	fp = fopen("Settings.json","r");
		fread(buffer, 1024, 1, fp);
		fclose(fp);
	parsed_json = json_tokener_parse(buffer);

	json_object_object_get_ex(parsed_json, "Filepath", &Filepath);

	get_instance().filepath = json_object_get_string(Filepath);

	if (get_instance().file != 0)
	{
		fclose(get_instance().file);
	}
	get_instance().file = fopen(get_instance().filepath, "a");

	if (get_instance().file == 0)
	{
		printf("Logger: Failed to open file at %s", get_instance().filepath);
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

	if(Stdoutput &output_type_){
		printf("\n\n>>==== Date (UTC): %s\nVersion: %u.%u.%u ====<<\n", timestamp_string,sw.major_,sw.minor_,sw.patch_);
	}
	if((File& (output_type_))>>1 && file){
		fprintf(file,"\n\n>>==== Date (UTC): %s\nVersion: %u.%u.%u ====<<\n", timestamp_string,sw.major_,sw.minor_,sw.patch_);
	}
}

//Returns a static logger instance.
Logger& Logger::get_instance(){
	static Logger logger;
	return logger;
}

//Frees up the file pointer when program ends.
void Logger::FreeFile(){
	if (file)
		{
			fclose(file);
			file = nullptr;
		}
}


std::string Logger::Merge(std::initializer_list<std::string> strList)
	{
	std::string ret = "";
	for (std::string s : strList) {
		ret += s;
	}
	return ret;
	}




