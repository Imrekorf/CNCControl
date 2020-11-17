#include "JobExecuter.h"
#include <iostream>

JobExecuter::JobExecuter()
{
	std::cout << "job executer initialized" << std::endl;
	signal(SIGALRM, JobExecuter::OnAlarm);
	ualarm(1, 1000);
}

JobExecuter::~JobExecuter(){
	signal(SIGALRM, SIG_DFL);
	for(auto job : jobs)
	{
		delete job;	
	}
}

std::vector<Job*> JobExecuter::jobs = {};
unsigned int JobExecuter::counter = 1;

void JobExecuter::OnAlarm(int sig_num){
	if(sig_num == SIGALRM){
		//std::cout << "counter: " << counter << std::endl;
		for(unsigned int i = 0; i < jobs.size(); i++)
		{
			// remove jobs that exited last time ( we do it now because of async )
			//! could result in problems due to async function taking longer than re-entering the job
			if(jobs[i]->GetJobState() == JobBase::Exit){
				delete jobs[i];
				jobs.erase(jobs.begin() + i);
				continue;
			}
			jobs[i]->ExecuteJob(counter);
		}
		counter++;	// rolls over eventually
	}
}

void JobExecuter::AddJob(Job* J){
	jobs.push_back(J);
}


unsigned int JobBase::GlobalJobCounter = 0;

JobBase::JobBase(unsigned int delay, unsigned int interval)
	: delay(delay), interval(interval) {
		JobID = GlobalJobCounter;
		GlobalJobCounter++;
		std::cout << "Starting Job: " << JobID << " in " << delay << "ms every " << interval << "ms" << std::endl;
		if(!delay)
			JS = Running;
	}
JobBase::~JobBase(){
	std::cout << "Exited Job " << JobID << " On State: " << JSToString() << std::endl;
}

bool JobBase::CheckRunJob(unsigned int Counter){
	JSmutex.lock();
	if(JS == Uncalled){
		CounterAtFirstCall = Counter;
		CounterAtEndDelay = Counter + delay;	// roll over baby!
		JS = Delayed;
	}
	if(JS == Delayed && Counter == CounterAtEndDelay){
		JS = Running;
		JSmutex.unlock();
		return true;
	}
	if(JS == Running && !((Counter - CounterAtFirstCall) % this->interval)){
		JSmutex.unlock();
		return true;
	}
	JSmutex.unlock();
	return false;
}

void JobBase::KillJob(){
	JSmutex.lock();
	JS = Exit;
	JSmutex.unlock();
}

JobBase::JobState JobBase::GetJobState(){
	return JS;
}

Job::Job(unsigned int delay, unsigned int interval, void (*jobfunction)(JobBase* Job))
	: JobBase(delay, interval), jobfunction(jobfunction) {}

void Job::ExecuteJob(unsigned int Counter){
	if(CheckRunJob(Counter)){
		std::async(jobfunction, this);
	}
}

Job::~Job(){}