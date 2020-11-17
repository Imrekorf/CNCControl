#pragma once

#include <stdlib.h>
#include <string>

#include <signal.h>
#include <unistd.h>

#include <future>
#include <vector>
#include <mutex>

class JobBase {
public:
	enum JobState {
		Uncalled,
		Delayed,
		Exit,
		Running,
		Error
	};

private:
	static unsigned int GlobalJobCounter;
	unsigned int JobID;

	unsigned int CounterAtFirstCall;
	unsigned int CounterAtEndDelay;

	unsigned int delay;
	unsigned int interval;

	std::mutex JSmutex;
	JobState JS = Uncalled;

	std::string JSToString(){
		switch(JS){
			case Uncalled: 	return "Uncalled"; break;
			case Delayed: 	return "Delayed"; break;
			case Exit: 		return "Exit"; break;
			case Running: 	return "Running"; break;
			case Error: 	return "Error"; break;
			default: 		return "-1"; break;
		}
	}

protected:
	bool CheckRunJob(unsigned int Counter);

public:
	JobBase(unsigned int delay, unsigned int interval);
	virtual ~JobBase();
	virtual void ExecuteJob(unsigned int Counter) = 0;
	void KillJob();
	JobState GetJobState();	
	unsigned int GetJobID() { return JobID; }
	void SetInterval(unsigned int interval) { this->interval = interval; }
};

class Job : public JobBase
{
private:
	void (*jobfunction)(JobBase* Job);

public:
	Job(unsigned int delay, unsigned int interval, void (*jobfunction)(JobBase* Job));
	~Job();
	void ExecuteJob(unsigned int Counter);

};


class JobExecuter
{
private:
	static std::vector<Job*> jobs;
	static unsigned int counter;
	static void OnAlarm(int sig_num);

public:
	JobExecuter();
	~JobExecuter();
	void AddJob(Job* J);
	unsigned int JobCount(){return jobs.size();}
};