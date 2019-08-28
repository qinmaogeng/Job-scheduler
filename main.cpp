#include <fstream>  
#include <string>  
#include <iostream>  
#include "test.h"
#include <stdlib.h>
#include <ostream>  

using namespace std;

struct input{
	string time;
	string command; /*the name of the command*/
	string para;
	string v1;
	string v2;
	int Time; /*time stamp*/
	int p1; 
	int p2; /*values of the command*/
	int isRange; /*the flag indicating that whether the printjob command is a findrange command or just print a job with given id*/
}; /*the struct containing the information reading from every line of the input file*/

input ReadData(input in, string line) /*this function interprete the sring line read from the file to some value in the input struct*/
{
	int pos1;
	int pos2;
	int cpos;
	int length;
	in.isRange = 0;
	pos1 = line.find(":");
	in.time = line.substr(0, pos1);
	in.Time = stoi(in.time); /*convert the time sting to long int Time,which is the time stamp of the command*/ 
	pos2 = line.find("(");
	length = pos2 - pos1;
	in.command = line.substr(pos1 + 2, length - 2);
	pos1 = pos2 + 1;
	pos2 = line.find(")");
	length = pos2 - pos1;
	in.para = line.substr(pos1, length); /*all the codes above get the position of the symbols of the line and then cut the line according to the symbols' position,
                                                         then store the cutted line into some string of the input struct for later disposal*/
	if (in.command.compare("Insert") == 0) /*if the command name is Insert, then we have two values of the command,we cut the para string and then convert in to int type*/
	{
		cpos = in.para.find(",");
		in.v1 = in.para.substr(0, cpos);
		in.v2 = in.para.substr(cpos + 1, in.para.length());
		in.p1 = stoi(in.v1);
		in.p2 = stoi(in.v2);
	}

	else if (in.command.compare("PrintJob") == 0)
	{
		if (in.para.find(",") == -1) /*if the command is PrintJob, if it have no commma in the para, it will be a function that print the job with given id,just convert
                                                   all of the para to int */
		{
			in.p1 = stoi(in.para);
		}
		else /*if the para string have comma,it have two values, so it is a command to print job between range,we should cut it to two string and then convert them to 
                             int*/
		{
			in.isRange = 1;
			cpos = in.para.find(",");
			in.v1 = in.para.substr(0, cpos);
			in.v2 = in.para.substr(cpos + 1, in.para.length());
			in.p1 = stoi(in.v1);
			in.p2 = stoi(in.v2);
		}
	}

	else if (in.command.compare("NextJob") == 0) 
	{
		in.p1 = stoi(in.para);
	}

	else if (in.command.compare("PreviousJob") == 0) /*"nextjob and previous only have 1 value, so we just convert the para to int*/
	{
		in.p1 = stoi(in.para);
	}
	return in;
}

int main(int argc ,char* argv[]) /*main function*/
{
        string filename = argv[1]; /* get the file name from the command line*/
	input in;
	ifstream IN(filename);
        ofstream fout("output_file.txt");
        streambuf *coutbackup;
        coutbackup= cout.rdbuf(fout.rdbuf()); /*redirect the stdout to the file named output_file.txt*/
	string line;
	job *jobs = new job[100];
	RBT *tree = new RBT();
	PHeap *heap = NULL;
	int GlobalTime = 0;
	int JobCount = 0;
	int JobNum = 0;
	int exe_c = 1; /*a flag indicating that whether the previous command is executed*/
	int assign = 1; /*a flag indicating that whether we should assign the next job to the scheduler*/
	int onetime = 0; 
	int runtime = 5;
	int more = 1;


	while (1){
		RBTNode *temp = new RBTNode();
		if (exe_c && getline(IN, line)) /*if the previous command is executed and we can get line from the file, read the data in line*/
		{
			in = ReadData(in, line);
			exe_c = 0;
		}

		if (GlobalTime == in.Time) /*if the time stamp of the current match the global time, execute it*/
		{
			exe_c = 1;
			if (in.command.compare("Insert") == 0) /*insert the jobID and job pointer of the new job into the RBT and insert its execute time and pointer
                                                                  into the min heap*/
			{
				jobs[JobNum] = job(in.p1, 0, in.p2, in.p2); /* construct a new job*/
				tree->insert(&jobs[JobNum]);
				heap = heap->insert(heap, jobs[JobNum].exe_t, &jobs[JobNum]);
				JobNum++;
				JobCount++;
			}
			else if (in.command.compare("PrintJob") == 0)
			{
				if (in.isRange == 0) /*search the job with the given job id and print it*/
				{
					temp = tree->search(tree->Root, in.p1);
					if (temp == NULL)
						cout << "(0,0,0)" << endl;
					else
						cout << "(" << temp->jobpointer->ID << "," << temp->jobpointer->exe_t << "," << temp->jobpointer->total_t << ")" << endl;
				}
				else if (in.isRange == 1) /*search the job between the given value low and high and then print it*/
				{
					tree->findRange(in.p1, in.p2);
                                        cout << "\n";
				}
			}
			else if (in.command.compare("NextJob") == 0) /*finding the node with the least ID bigger than the given ID and then print it*/
			{
				temp = tree->next(in.p1);
				if (temp == NULL)
				cout << "(0,0,0)" << endl;
				else
					cout << "(" << temp->jobpointer->ID << "," << temp->jobpointer->exe_t << "," << temp->jobpointer->total_t << ")" << endl;
			}
			else if (in.command.compare("PreviousJob") == 0) /*finding the node with the largest ID less than the given ID and then print it*/
			{
				temp = tree->previous(in.p1);
				if (temp == NULL)
					cout << "(0,0,0)" << endl;
				else
					cout << "(" << temp->jobpointer->ID << "," << temp->jobpointer->exe_t << "," << temp->jobpointer->total_t << ")" << endl;
			}
		}

		if (assign && heap != NULL) /*assign a new job to the jobscheduler*/
		{
			if (heap->jobpointer->remain_t < 5) /*if the job's remaining time is less than 5, set its run time to its remaining time*/
				runtime = heap->jobpointer->remain_t;
			else
				runtime = 5;
			onetime = 0;
			assign = 0;
		}
		
		if (heap != NULL) /*if there are any job in the system, execute it and caculate its executed time, remaining time, and the time it have worked for one time*/ 
		{

			heap->jobpointer->exe_t++;
			heap->jobpointer->remain_t = heap->jobpointer->total_t - heap->jobpointer->exe_t;

			onetime++;
		}


		if (onetime == runtime&& heap!=NULL) /*if the scheduled work have worked for 5ms in one time*/
		{
			if (heap->jobpointer->remain_t == 0) /*if the remaining time is 0,the job is done,remove it from both the RBT and min heap*/
			{
				tree->remove(heap->jobpointer->ID);
				heap = heap->deleteMin(heap);
				JobCount--;
			}
			else
				heap = heap->increaseKey(5, heap); /*else,increase the key of the job in the min heap,update the job with minimum executed time*/
			assign = 1;
		}

		GlobalTime++; /*increase the global time every loop*/

		if (JobCount == 0 && IN.eof()) /*if there are no job in the scheduler and no command come from the file, terminate the scheduler*/
			break;

	}

        cout.rdbuf(coutbackup); 

	return 0;
}
