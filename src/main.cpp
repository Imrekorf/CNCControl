#include <wiringPi.h>
#include <iostream>

#include "Manager.h"
#include "JobExecuter.h"

// used for debugging
int main(int argc, char** argv);

int main(int argc, char** argv)
{
	
	wiringPiSetup();

	JobExecuter JE;

	Manager manager(&JE, {0, 0, 0});

	while(JE.JobCount());

	return 0;
}