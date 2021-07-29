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

void Logger::Init(OutputType stdoutput,OutputType file,OutputType udp , const char* new_filepath , unsigned short sin_port,unsigned long s_addr){

	if(Stdoutput &(stdoutput|file|udp)){
		printf("konsol ciktisi aktif\n");
	}

	if((File&(stdoutput|file|udp))>>1){
		printf("file ciktisi aktif\n");
		EnableFileOutput();
	}

	if((Udp &(stdoutput|file|udp))>>2){
		printf("udp ciktisi aktif\n");
		EnableUdpOutput();
	}
}


void Logger::EnableUdpOutput(unsigned short sin_port,unsigned long s_addr){

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
	servaddr.sin_port = htons(sin_port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	udp=true;


}

//To be able writing to a file, opens file for output at the end of a file.
void Logger::EnableFileOutput(const char* new_filepath){
	get_instance().filepath=new_filepath;
	if (file)
	{
		fclose(file);
	}
	file = fopen(filepath, "a");

	if (!file)
	{
		printf("Logger: Failed to open file at %s", filepath);
	}
}

//This method writes current time in UTC format.
//Writes given software version to the header.
//Checks is there any file to write log, if there is,it also writes in the file.
void Logger::WriteHeader(SWVersion &sw){
	std::time_t current_time = std::time(0);
	std::tm* timestamp = std::gmtime(&current_time);
	char buffer[80];
	strftime(buffer, 80, "%c", timestamp);

	printf("\n\n>>==== Date (UTC): %s\nVersion: %u.%u.%u ====<<\n", buffer,sw.major_,sw.minor_,sw.patch_);

	if (file)
	{
		fprintf(file,"\n\n>>==== Date (UTC): %s\nVersion: %u.%u.%u ====<<\n", buffer,sw.major_,sw.minor_,sw.patch_);
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

std::string Logger::merge(std::initializer_list<std::string> strList)
	{
	    std::string ret = "";
	    for (std::string s : strList) {
	        ret += s;
	    }
	    return ret;
	}




