
#ifndef SRC_LOGGER_LOGGER_H_
#define SRC_LOGGER_LOGGER_H_

#include <stdio.h>

#include <ctime>
enum LogPriority
{
	InfoPriority, WarningPriority, ErrorPriority
};
struct SWVersion{
	unsigned int major_;
	unsigned int minor_;
	unsigned int patch_;
};

class Logger
{
public:
	static void EnableFileOutput(const char* new_filepath);
	static void WriteHeader(SWVersion *sw);

	template<typename... Args>
	static void Info(const char* message, Args... args){
		get_instance().Log("[Info]\t", message, args...);
	}

	template<typename... Args>
	static void Warning(const char* message, Args... args){
		get_instance().Log("[Warning]\t", message, args...);
	}

	template<typename... Args>
	static void Error(const char* message, Args... args){
		get_instance().Log("[Error]\t", message, args...);
	}

private:
	LogPriority priority = InfoPriority;
	const char* filepath = 0;
	FILE* file = 0;

	Logger() {
	}
	~Logger()
	{
		FreeFile();
	}

	static Logger& get_instance();

	template<typename... Args>
	void Log(const char* message_priority_str, const char* message, Args... args){

		std::time_t current_time = std::time(0);
		std::tm* timestamp = std::gmtime(&current_time);
		char buffer[80];
		strftime(buffer, 80, "%c", timestamp);
		printf("%s\t", buffer);
		printf("%s",message_priority_str);
		printf(message, args...);
		printf("\n");

		if (file)
		{
			fprintf(file, "%s\t", buffer);
			fprintf(file,"%s", message_priority_str);
			fprintf(file, message, args...);
			fprintf(file, "\n");
		}
	}

	void EnableFileOutputInstance();

	void FreeFile();

	void WriteHearderInstance(SWVersion *sw);
};
#endif
