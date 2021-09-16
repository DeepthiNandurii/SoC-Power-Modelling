

/*******************************************************************************
--------------------------------------------------------------------------------

		File		: 		measure_cpu_power.c

		Date		: 		15/09/2021
		
		Author		:		Deepthi Nanduri
		
		UserID		:		fc20725@bristol.ac.uk
		
		Description	:		Top file containting all essential functions to read from CPU
							PMU counters and launching threads in parallel to launch
							BM programs. 
							This file will be used in conjunction with the top file containing all essential functions to read GPU PC counters 
							(i.e, measurement_threads.c). 
							
		Functions present	:	

							1. a57_read_samples 		- Reads 3 on-board INA3221 power monitors and sample counter values during benchmark run
							2. a57_read_sample_pthread	- Creates parallel threads to launch benchmarks
							3. a57_read_sample_start	- Starts the parallel threads
							4. a57_read_sample_stop		- Stops parallel threads launched
							5. a57_clear_sample_pthread	- Destroys threads at the end of the benchmark run
							
----------------------------------------------------------------------------------
**********************************************************************************/

#define _GNU_SOURCE		// Gives a warning during compile
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "jtx1inst.h"
#include "monitoring.h"
#include "pmu_event_sel.h"
#include "read_counters.h"


typedef unsigned int uint32_t;
FILE *fp_log_cpu;
int start_cpu;
pthread_mutex_t mutex_cpu;
int stop_cpu;
pthread_t thread_ID_cpu;
void *exit_status_cpu;
extern struct a57_sample *head_cpu;

long long int sample_count_cpu = 0;

double cpu_power_total = 0;
double in_power_total = 0;

/* read_samples method */
void *a57_read_samples(void *head_cpu) {
	unsigned int cpu_power_value;	
	unsigned int in_power_value;

	printf("executing thread\n");
	int start_flag = 0;
	int stop_flag = 0;

	printf("\n\t measure_cpu_pow.c::a57_read_samples() : Initializing counters... \n");
	disable_counters();  // Reset counters	
	init_counters();     // Initialize counters 
	printf("\n\t measure_cpu_pow.c::a57_read_samples() : Initializing counters...DONE \n");

	printf("\n\t measure_cpu_pow.c::a57_read_samples() : Opening Log File to write...\n");

	fp_log_cpu = fopen("power_measurement_log_a57.dat", "w");

	if (!fp_log_cpu) {
		printf("cannot open power_measurement_log_a57.dat file \n ");
		return 0;
	}

	int ev_group = 2;	// Select amongst the available group of PMU events - refer pmu_event_sel.h

	printf("\n\t measure_cpu_pow.c::a57_read_samples() : Choosing PMU events for Monitoring...\n");
	choose_events(ev_group);	// Selecting performance counters

	
	if (ev_group==1)
	fprintf(fp_log_cpu, "#Timestamp\tBenchmark\tRun(#)\tCPU Frequency (MHz)\tCPU Temperature(C)\tCPU Voltage(V)\tCPU Power(W)\tCPU_CYCLES\tL1_DATA_CACHE_ACCESS\tINSTRUCTIONS_RETIRED\tBUS_CYCLES\tL1I_CACHE_ACCESS\tPREDICTABLE_BRANCH_SPECULATIVELY_EXECUTED\n");	// option 1 ARM RECOMMENDED -- CPU_CYCLES - pmu0
	
	if(ev_group==2)
	fprintf(fp_log_cpu, "#Timestamp\tBenchmark\tRun(#)\tCPU Frequency (MHz)\tCPU Temperature(C)\tCPU Voltage(V)\tCPU Power(W)\tL1_DATA_CACHE_ACCESS\tCPU_CYCLES\tL1_INSTR_CACHE_ACCESS\tBUS_CYCLES\tBUS_ACCESS_PERIPHERAL\tBRANCH_SPECULATIVELY_EXECUTED_PROCEDURE_RETURN\n");	// Per-frequency model, Jose/Kris - option 2
	
	if(ev_group==3)
	fprintf(fp_log_cpu, "#Timestamp\tBenchmark\tRun(#)\tCPU Frequency (MHz)\tCPU Temperature(C)\tCPU Voltage(V)\tCPU Power(W)\tL1_INSTRUCTION_CACHE_ACCESS\tL1_DATA_CACHE_ACCESS\tEXCEPTION_TAKEN\tBRANCH_MISPREDICTION\n");	// Rance Rogrigues's model - option 3
	
	if(ev_group==4)
	fprintf(fp_log_cpu, "#Timestamp\tBenchmark\tRun(#)\tCPU Frequency (MHz)\tCPU Temperature(C)\tCPU Voltage(V)\tCPU Power(W)\tL1_INSTRUCTION_CACHE_ACCESS\tINSTRUCTION_SPECULATIVELY_EXECUTED\tL2_DATA_CACHE_ACCESS_READ\tBUS_ACCESS\tUNALIGNED_ACCESS\tCPU_CYCLES\n");	// Matt Walker's model - option 4
	fflush(fp_log_cpu);	
	
	if(feof(fp_log_cpu))
	printf("\n\t measure_cpu_pow.c::a57_read_samples() : FAILED to write first line in fp_log FILE... \n");

	char a57freq[100];
	char a57temp[100];
	char a57bench[100];
	char cpu_voltage[100];

	FILE *benchmarks = fopen("./benchmarks","r");

	struct timespec tsample = {0,0};
	float timenow, starttime;

	clock_gettime(CLOCK_MONOTONIC,&tsample);

	starttime = (tsample.tv_sec*1.0e9 + tsample.tv_nsec)/1000000;

	cpu_power_total = 0;
	in_power_total = 0;

	sample_count_cpu = 0;

	while(1){
	
		start_flag = 1;
		pthread_mutex_lock(&mutex_cpu);

		if(start_cpu==0)	start_flag = 0;
		pthread_mutex_unlock(&mutex_cpu);
		if(start_flag == 0) continue;
		
		usleep(100000);	// sleep for 100ms, one sample per 100ms
		
		printf("\t \t measure_cpu_pow.c::a57_read_samples : In WHILE loop. Reading ina3221 meters...\n\n");
		jtx1_get_ina3221(VDD_CPU, POWER, &cpu_power_value);
		printf("\t measure_cpu_power::a57_read_samples : Reading CPU power Value: %d \n",cpu_power_value);
		jtx1_get_ina3221(VDD_IN, POWER, &in_power_value);
		printf("\t measure_cpu_power::a57_read_samples : Reading Input power Value: %d \n",in_power_value);

		cpu_power_total += cpu_power_value;
		in_power_total += in_power_value;
		printf("\t \t measure_cpu_pow.c::a57_read_samples() Reading ina3221 meters...DONE \n");
		
		sample_count_cpu++;
		printf("\t measure_cpu_pow.c::a57_read_samples : In WHILE loop. Sample Count Value: %lld \n\n",sample_count_cpu);
		{
			printf("\t measure_cpu_pow.c::a57_read_samples : Reading Counter Values... \n");
			
			//Reading from counters..	
			asm volatile("msr pmcntenclr_el0,%0" :: "r" (0x8000003f));                  // disable counting	    
			read_a57_counters('a');
			disable_counters();            //This will reset the counters
			init_counters();
			printf("\t measure_cpu_pow.c::a57_read_samples : Reading Counter Values...DONE \n");

			FILE *rate = fopen("/sys/kernel/debug/clock/cpu/rate","r");		// add path to the sysfs file to read the CPU clock
			FILE *voltage = fopen("/sys/kernel/debug/clock/vdd_cpu_mv","r");		// add path to the sysfs file to read CPU voltage
			FILE *temp = fopen("/sys/devices/virtual/thermal/thermal_zone1/temp","r");		// add path to the sysfs file to read the CPU temperature

			size_t elements_freq = fread(a57freq,1,20,rate);
			strtok(a57freq,"\n");

			size_t elements_temp = fread(a57temp,1,20,temp);
			strtok(a57temp,"\n");

			rewind(benchmarks);
			size_t elements_bm = fread(a57bench,1,40,benchmarks);

			size_t elements_volt = fread(cpu_voltage,1,20,voltage);
			strtok(cpu_voltage,"\n");

			clock_gettime(CLOCK_MONOTONIC,&tsample);
			timenow = (tsample.tv_sec*1.0e9+tsample.tv_nsec)/1000000;
			printf("\t measure_cpu_pow.c::a57_read_samples: timenow is: %f \n",timenow);

			fclose(rate);
			fclose(temp);
			fclose(voltage);

			if(a57bench[0]!='\n')
			{
				strtok(a57bench,"\n");

				int cpufreqMHz = atoi(a57freq)/1000000;		// CPU freq in MHz
				printf("\t measure_cpu_pow.c::a57_read_samples: CPU freq in MHz : %d \n",cpufreqMHz);
				int cputempdeg = atoi(a57temp)/1000;		// CPU temp in degrees
				printf("\t measure_cpu_pow.c::a57_read_samples: CPU temp in degrees : %d \n",cputempdeg);
				float cpuvoltageV = atoi(cpu_voltage)/1000.0;	// CPU voltage in Volt
				printf("\t measure_cpu_pow.c::a57_read_samples: CPU Voltage in V : %f \n",cpuvoltageV);
				int timesample = (int)(timenow-starttime);	// Duration of BM run

				/*
				Activity density values calculated in read_a57_counters('a')
				*/

				//Checking for zero counter values and updating the log file with non-zero counter values..
				if((cnt0<=0.0)&&(cnt1<=0.0)&&(cnt2<=0.0)&&(cnt3<=0.0)&&(cnt4<=0.0)&&(cnt5<=0.0))
				{
				printf("\t measure_cpu_pow.c::a57_read_samples() : Counter activity Values are 0 or less...\n");
				printf("\t measure_cpu_pow.c::a57_read_samples() : UNABLE to update log file...\n");
				}
				else{
				printf("\t measure_cpu_pow.c::a57_read_samples() : Updating Log file with Counter Activity information...\n");
				fprintf(fp_log_cpu, "%d %s 1 %d %d %f %f ", timesample, a57bench, cpufreqMHz , cputempdeg,cpuvoltageV,(cpu_power_total)/1000);
				fflush(fp_log_cpu);	
				fprintf(fp_log_cpu," %f %f %f %f %f %f \n",cnt0,cnt1,cnt2,cnt3,cnt4,cnt5);		// Updating log file with Counter stats
				fflush(fp_log_cpu);	
				}
				cpu_power_total = 0;
				in_power_total = 0;
			}
		}

		stop_flag = 0;
		pthread_mutex_lock(&mutex_cpu);
		if(stop_cpu == 1) {	stop_flag = 1; }
		pthread_mutex_unlock(&mutex_cpu);

		if(stop_flag==1)
		{
			printf("Breaking sampling thread\n");
			break;
		}
	}		// end of while loop
	fclose(fp_log_cpu);
	fclose(benchmarks);	
	return 0;
}

void *a57_read_sample_pthread() {

	pthread_mutex_init(&mutex_cpu, NULL);

	pthread_mutex_lock(&mutex_cpu);
	stop_cpu = 0;
	start_cpu = 0;
	pthread_mutex_unlock(&mutex_cpu);

	head_cpu = (struct a57_sample *)malloc(sizeof (struct a57_sample));
	head_cpu->next = NULL;

	cpu_set_t cpu_set2;
	CPU_SET(1, &cpu_set2);
	printf("create thread\n");
	pthread_create(&thread_ID_cpu,NULL,a57_read_samples,head_cpu);
	sched_setaffinity(thread_ID_cpu, sizeof(cpu_set_t), &cpu_set2);

	return head_cpu;

}

void a57_read_sample_start() 
{

	pthread_mutex_lock(&mutex_cpu);
	sample_count_cpu = 0;
	cpu_power_total = 0;
	in_power_total = 0;

	start_cpu = 1;
	stop_cpu = 0;	
	pthread_mutex_unlock(&mutex_cpu);

}

void a57_read_sample_stop() {

	pthread_mutex_lock(&mutex_cpu);
	stop_cpu = 1;
	start_cpu = 0;
	pthread_mutex_unlock(&mutex_cpu);
	
	printf("join thread\n");
	pthread_join(thread_ID_cpu, &exit_status_cpu);
	printf("join thread...DONE\n");
	printf("detach thread...\n");
	pthread_detach(thread_ID_cpu);
	printf("detach thread...DONE\n");

}

void a57_clear_sample_pthread(struct a57_sample *head_cpu) {

	struct a57_sample *sample = head_cpu;
	while (sample!= (struct a57_sample *)NULL) {
		struct a57_sample *next = sample->next;
		free(sample);
		sample = next;
	}
	pthread_mutex_destroy(&mutex_cpu);
	printf("\t measure_cpu_pow.c::a57_clear_sample_pthread() : MUTEX DESTROY, clear thread...\n");
}	
