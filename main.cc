#include "logger.h"
#include <iostream>
#include <string>
void SetSWVersion(unsigned int new_major,unsigned int new_minor,unsigned int new_patch){
	struct SWVersion sw;
	sw.major_=new_major;
	sw.minor_=new_minor;
	sw.patch_=new_patch;
	Logger::WriteHeader(&sw);
}
int main(){
	Logger::EnableFileOutput("/home/cerid/eclipse-workspace/test_googletest/file.txt");
	SetSWVersion(5,6,3);
	int a=1881;
	double gravity=9.8;
	const char *deneme="hrhrhhrhrhrh";
	Logger::Info("Logger Ready To Use%d %s",a,deneme);
	Logger::Warning("Warning expected value not in range %d %s",a,deneme);
	Logger::Error("Divided by 0 program can not continue %lf %d",gravity,a);
	return 0;
}

