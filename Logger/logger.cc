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

	json_object_object_get_ex(parsed_json, "UdpPort", &pudp_port_);
	json_object_object_get_ex(parsed_json, "UdpServerIp", &pudp_serverip_);
	json_object_object_get_ex(parsed_json, "Filepath", &pfilepath_json_);
	json_object_object_get_ex(parsed_json, "TcpPort", &ptcp_port_);
	json_object_object_get_ex(parsed_json, "TcpServerIp", &ptcp_serverip_);
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
	if((kTcp &(output_type_))>>3){
		EnableTcpOutput();
	}
}

//To be able writing to UDP socket.
void Logger::EnableUdpOutput(){
	// Creating socket file descriptor
	if ( (udp_sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	int opt = 1;
	if( setsockopt(udp_sockfd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt)) < 0 )
	{
	    perror("setsockopt");
	    exit(EXIT_FAILURE);
	}
	memset(&udp_servaddr_, 0, sizeof(udp_servaddr_));

	// Filling server information
	udp_servaddr_.sin_family = AF_INET;
	udp_servaddr_.sin_port = htons(json_object_get_int(pudp_port_));
	udp_servaddr_.sin_addr.s_addr = inet_addr(json_object_get_string(pudp_serverip_));
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

void Logger::EnableTcpOutput(){
    unsigned int len;
    struct sockaddr_in servaddr, cli;
    // socket create and verification
    tcp_sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sockfd_ == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(json_object_get_string(ptcp_serverip_));
    servaddr.sin_port = htons(json_object_get_int(ptcp_port_));

    // Binding newly created socket to given IP and verification
    if ((bind(tcp_sockfd_, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(tcp_sockfd_, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    tcp_connfd_ = accept(tcp_sockfd_, (struct sockaddr *)&cli, &len);
    if (tcp_connfd_ < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");

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
