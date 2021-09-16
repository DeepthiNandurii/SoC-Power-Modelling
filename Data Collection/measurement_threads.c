
 
 /*******************************************************************************
--------------------------------------------------------------------------------

		File		: 		measurement_threads.c

		Date		: 		15/05/2015
		
		Author		:		csxmh
		
		Description	:		Top file containting all essential functions to read from GPU
							PMU counters and launching threads in parallel to launch
							BM programs. 
							This script is adopted from work of Nunez-Yanez et al.[4]
							This file will be used in conjunction with the top file containing all essential functions to read CPU PC counters 
							(i.e, measure_cpu_pow.c). 
							
----------------------------------------------------------------------------------
**********************************************************************************/

#define _GNU_SOURCE             /* See feature_test_macros(7) */
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


/////// performance counters
#define S_OK 0x00000000
#define NVPM_INITGUID
#include "NvPmApi.Manager.h"
#include "NvPmApi.h"
#include "unistd.h"
#include "iostream"

extern int pc;
extern int bm;


// Simple singleton implementation for grabbing the NvPmApi
static NvPmApiManager S_NVPMManager;
extern NvPmApiManager *GetNvPmApiManager() {return &S_NVPMManager;}
const NvPmApi *GetNvPmApi() {return S_NVPMManager.Api();}
NVPMContext hNVPMContext;

//-------------------declaration of detection execution results of GetCounterValueByName()-----------------------------------------------------------

NVPMRESULT   inst_executed_cs_result;
NVPMRESULT   sm_inst_executed_texture_result;
NVPMRESULT   sm_executed_ipc_result;
NVPMRESULT   sm_issued_ipc_result;
NVPMRESULT   threads_launched_result;
NVPMRESULT   gpu_busy_result;
NVPMRESULT   gpu_idle_result;
NVPMRESULT   l2_write_bytes_result;
NVPMRESULT   l2_read_bytes_result;
NVPMRESULT   sm_inst_executed_global_loads_result;
NVPMRESULT   sm_inst_executed_global_stores_result;
NVPMRESULT   sm_active_cycles_result;
NVPMRESULT   sm_active_warps_result;
NVPMRESULT   sm_warps_launched_result;
///////// to here counters


double result1;
double result2;
double result3;
double result4;
double result5;
double result6;
double result7;
double result8;
double result9;
double result10;
double result11;
double result12;
double result13;
double result14;


FILE *fp_log;
int start_gpu;
pthread_mutex_t mutex_gpu;

int stop_gpu;
pthread_t  thread_ID_gpu;
void      *exit_status_gpu;
extern struct jetsontx1_sample * head_gpu;

long long int sample_count_gpu = 0;

double   gpu_power_total=0;


int initcounters(void){

printf("switch pc\n");


//---------------------------------add counters-------------------------------------------

switch(pc) {
	case 0 :
		//if (bm==0)
		{
		printf("case 0 \n ");
		if(GetNvPmApiManager()->Construct(L"/home/ubuntu/Desktop/PerfKit/lib/a64/libNvPmApi.Core.so") != S_OK)
		{  std::cout <<  "Error 1" << std::endl; 
   			return false; // This is an error condition
		}
		NVPMRESULT nvResult;
		if((nvResult = GetNvPmApi()->Init()) != NVPM_OK)
		{  std::cout <<  "Error 2" << std::endl; 
   			return false; // This is an error condition
		}

		if((nvResult = GetNvPmApi()->CreateContextFromCudaContext(0, &hNVPMContext)) != NVPM_OK)
		{  std::cout <<  "Error 3" << std::endl; 
   			return false; // This is an error condition
		}

		fp_log = fopen("power_measurement_log_c0.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c0.dat file \n ");
			return 0;
			}
		GetNvPmApi()->AddCounterByName(hNVPMContext,"inst_executed_cs");
		}		
		break;
	case 1:
		//if (bm==0)
		{
		printf("case 1 \n ");
		fp_log = fopen("power_measurement_log_c1.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c1.dat file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"inst_executed_cs");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_inst_executed_texture");
		}
		break;
	case 2:
		//if (bm==0)
		{
		printf("case 2 \n ");
		fp_log = fopen("power_measurement_log_c2.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c2.dat file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"sm_inst_executed_texture");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_inst_executed_global_loads");
		}
		break;
	case 3:
		//if (bm==0)
		{
		printf("case 3 \n ");
		fp_log = fopen("power_measurement_log_c3.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c3.dat file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"sm_inst_executed_global_loads");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_inst_executed_global_stores");
		}
		break;
	case 4:
		//if (bm==0)
		{
		printf("case 4 \n ");
		fp_log = fopen("power_measurement_log_c4.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c4.dat file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"sm_inst_executed_global_stores");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"threads_launched");
		}
		break;
	case 5:
		//if (bm==0)
		{
		printf("case 5 \n ");
		fp_log = fopen("power_measurement_log_c5.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c5.dat file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"threads_launched");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"gpu_busy");
		}
		break;

	case 6:
		//if (bm==0)
		{
		printf("case 6 \n ");
		fp_log = fopen("power_measurement_log_c6.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c5.txt file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"gpu_busy");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_executed_ipc");
		}

		break;
	case 7:
		//if (bm==0)
		{
		printf("case 7 \n ");
		fp_log = fopen("power_measurement_log_c7.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c5.txt file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"sm_executed_ipc");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_issued_ipc");
		}

		break;
	case 8:
		//if (bm==0)
		{
		printf("case 8 \n ");
		fp_log = fopen("power_measurement_log_c8.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c5.txt file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"sm_issued_ipc");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"l2_write_bytes");
		}

		break;
	case 9:
		//if (bm==0)
		{
		printf("case 9 \n ");
		fp_log = fopen("power_measurement_log_c9.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c5.txt file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"l2_write_bytes");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"l2_read_bytes");
		}
		break;
	case 10:
		//if (bm==0)
		{
		printf("case 10 \n ");
		fp_log = fopen("power_measurement_log_c10.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c5.txt file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"l2_read_bytes");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_active_cycles");
		}

		break;
	case 11:
		//if (bm==0)
		{
		printf("case 11 \n ");
		fp_log = fopen("power_measurement_log_c11.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c5.txt file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"sm_active_cycles");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_active_warps");
		}

		break;
	case 12:
		//if (bm==0)
		{
		printf("case 12 \n ");
		fp_log = fopen("power_measurement_log_c12.dat", "w");
		if (!fp_log) {
			printf("cannot open power_measurement_log_c5.txt file \n ");
			return 0;
			}
		GetNvPmApi()->RemoveCounterByName(hNVPMContext,"sm_active_warps");
		GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_warps_launched");
		}

		break;
	
}


//GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_executed_ipc");
//GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_issued_ipc");
///GetNvPmApi()->AddCounterByName(hNVPMContext,"threads_launched");
///GetNvPmApi()->AddCounterByName(hNVPMContext,"gpu_busy");
//GetNvPmApi()->AddCounterByName(hNVPMContext,"gpu_idle");
//GetNvPmApi()->AddCounterByName(hNVPMContext,"l2_write_bytes");
//GetNvPmApi()->AddCounterByName(hNVPMContext,"l2_read_bytes");
///GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_inst_executed_global_loads");
///GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_inst_executed_global_stores");
//GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_active_cycles");
//GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_active_warps");
//GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_warps_launched");


}


int getcountersvalue(void){ 


NVPMUINT nCount=0;
NVPMUINT64 value_14,value_1,value_2,value_3,value_4,value_5,value_6,value_7,value_8,value_9,value_10,value_11,value_12,value_13;
NVPMUINT64 cycle_14,cycle_1,cycle_2,cycle_3,cycle_4,cycle_5,cycle_6,cycle_7,cycle_8,cycle_9,cycle_10,cycle_11,cycle_12,cycle_13;

 

//--------------------------------------------do not edit------------------------------------------------------------------------------------------------------------
/*if(GetNvPmApiManager()->Construct(L"/home/ubuntu/Desktop/PerfKit/lib/a64/libNvPmApi.Core.so") != S_OK)
{  //std::cout <<  "Error 1" << std::endl; 
   return false; // This is an error condition
}
NVPMRESULT nvResult;
if((nvResult = GetNvPmApi()->Init()) != NVPM_OK)
{ // std::cout <<  "Error 2" << std::endl; 
   return false; // This is an error condition
}
NVPMContext hNVPMContext;
if((nvResult = GetNvPmApi()->CreateContextFromCudaContext(0, &hNVPMContext)) != NVPM_OK)
{  //std::cout <<  "Error 3" << std::endl; 
   return false; // This is an error condition
}

//-------------------declaration of detection execution results of GetCounterValueByName()-----------------------------------------------------------

NVPMRESULT   inst_executed_cs_result;
//NVPMRESULT   sm_inst_executed_texture_result;
NVPMRESULT   sm_executed_ipc_result;
NVPMRESULT   sm_issued_ipc_result;
NVPMRESULT   threads_launched_result;
NVPMRESULT   gpu_busy_result;
NVPMRESULT   l2_write_bytes_result;
NVPMRESULT   l2_read_bytes_result;
NVPMRESULT   sm_inst_executed_global_loads_result;
NVPMRESULT   sm_inst_executed_global_stores_result;
NVPMRESULT   sm_active_cycles_result;
NVPMRESULT   sm_active_warps_result;
NVPMRESULT   sm_warps_launched_result;

//---------------------------------add counters-------------------------------------------

GetNvPmApi()->AddCounterByName(hNVPMContext,"inst_executed_cs");
//GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_inst_executed_texture");

GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_executed_ipc");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_issued_ipc");
GetNvPmApi()->AddCounterByName(hNVPMContext,"l2_write_bytes");
GetNvPmApi()->AddCounterByName(hNVPMContext,"l2_read_bytes");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_active_cycles");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_active_warps");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_warps_launched");

GetNvPmApi()->AddCounterByName(hNVPMContext,"threads_launched");
GetNvPmApi()->AddCounterByName(hNVPMContext,"gpu_busy");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_inst_executed_global_loads");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_inst_executed_global_stores");

*/
//----------------------------initialzation of value and cycle(final result)--------------------------------------


value_14 = value_2 = value_1  = value_3 = value_4 = value_5 = value_6 = value_7 = value_8 = value_9 = value_10 = value_11 = value_12 = value_13 = 0;
cycle_14 = cycle_2 = cycle_1  = cycle_3 = cycle_4 = cycle_5 = cycle_6 = cycle_7 = cycle_8 = cycle_9 = cycle_10 = cycle_11 = cycle_12 = cycle_13 = 0;


//----------------------------------do sample frist time--------------------------------------------
//-----------------------Attention: some counters may need multiple sampling-------------------------
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
//printf("ncount is %d\n",nCount);
//usleep(10000000);//just for test(it not allowed to wait so long time between two samples)
//GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
//GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
//GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
//GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
//GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
//GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);

//GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
//GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);


//GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
/*GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
GetNvPmApi()->Sample(hNVPMContext,NULL,&nCount);
*/

//----------------------------------------collect execution of statues(0 is no-errror)-------------------------------------------
//---------------------------------------may have other statues (see the official statement)

inst_executed_cs_result                  = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"inst_executed_cs",0,&value_1,&cycle_1);
sm_inst_executed_texture_result          = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"sm_inst_executed_texture",0,&value_2,&cycle_2);
sm_inst_executed_global_loads_result     = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"sm_inst_executed_global_loads",0,&value_3,&cycle_3);
sm_inst_executed_global_stores_result    = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"sm_inst_executed_global_stores",0,&value_4,&cycle_4);
threads_launched_result                  = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"threads_launched",0,&value_5,&cycle_5);
gpu_busy_result                          = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"gpu_busy",0,&value_6,&cycle_6);
sm_executed_ipc_result                   = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"sm_executed_ipc",0,&value_7,&cycle_7);
sm_issued_ipc_result                     = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"sm_issued_ipc",0,&value_8,&cycle_8);
l2_write_bytes_result                    = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"l2_write_bytes",0,&value_9,&cycle_9);
l2_read_bytes_result                     = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"l2_read_bytes",0,&value_10,&cycle_10);
sm_active_cycles_result                  = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"sm_active_cycles",0,&value_11,&cycle_11);
sm_active_warps_result                   = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"sm_active_warps",0,&value_12,&cycle_12);
sm_warps_launched_result                 = GetNvPmApi()->GetCounterValueByName(hNVPMContext,"sm_warps_launched",0,&value_13,&cycle_13);

/*
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_executed_ipc");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_issued_ipc");
GetNvPmApi()->AddCounterByName(hNVPMContext,"l2_write_bytes");
GetNvPmApi()->AddCounterByName(hNVPMContext,"l2_read_bytes");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_active_cycles");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_active_warps");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_warps_launched");
*/

result1 = 100*double(value_1)/double(cycle_1);
result2 = 100*double(value_2)/double(cycle_2);
result3 = 100*double(value_3)/double(cycle_3);
result4 = 100*double(value_4)/double(cycle_4);
result5 = 100*double(value_5)/double(cycle_5);
result6 = 100*double(value_6)/double(cycle_6);
result7 = 100*double(value_7)/double(cycle_7);
result8 = 100*double(value_8)/double(cycle_8);
result9 = 100*double(value_9)/double(cycle_9);
result10 = 100*double(value_10)/double(cycle_10);
result11 = 100*double(value_11)/double(cycle_11);
result12 = 100*double(value_12)/double(cycle_12);
result13 = 100*double(value_13)/double(cycle_13);

/*result1 = value_1;
result2 = value_2;
result3 = value_3;
result4 = value_4;
result5 = value_5;
result6 = value_6;
result7 = value_7;
result8 = value_8;
result9 = value_9;
result10 = value_10;
result11 = value_11;
result12 = value_12;
result13 = value_13;
result14 = value_14;*/

//printf("result2 = %f\n",result2);
/*fprintf(fp_log,"sm_executed_ipc = %f\n",result3);

fprintf(fp_log,"sm_issued_ipc = %f\n",result4);
fprintf(fp_log,"threads_launched = %f\n",result5);
fprintf(fp_log,"gpu_busy = %f\n",result6);
fprintf(fp_log,"l2_write_bytes = %f\n",result7);
fprintf(fp_log,"l2_read_bytes = %f\n",result8);
fprintf(fp_log,"sm_inst_executed_global_loads = %f\n",result9);

fprintf(fp_log,"sm_inst_executed_global_stores = %f\n",result10);
fprintf(fp_log,"sm_active_cycles = %f\n",result11);
fprintf(fp_log,"sm_active_warps = %f\n",result12);

fprintf(fp_log,"sm_warps_launched = %f\n",result13);*/



return 0;
}



void *jetsontx1_read_samples(void *head_gpu) {


	unsigned int gpu_power_value;
	unsigned int cpu_power_value;
	unsigned int in_power_value;

	printf("executing thread\n");


//	int i;
//	struct  jetsontx1_sample *t = (struct jetsontx1_sample*) head;
	int start_flag = 0;
	int stop_flag = 0;
	
	initcounters();

	//fprintf(fp_log, "#Timestamp Benchmark Run(#) GPU Frequency (MHz) GPU Temperature GPU Voltage(V) GPU Current(A) GPU Power(W) inst_executed_cs sm_executed_ipc sm_issued_ipc threads_launched gpu_busy l2_write_bytes l2_read_bytes sm_inst_executed_global_loads m_inst_executed_global_stores sm_active_cycles sm_active_warps sm_warps_launched \n");

	fprintf(fp_log, "#Timestamp\tBenchmark\tRun(#)\tGPU Frequency (MHz)\tGPU Temperature(C)\tGPU Voltage(V)\tGPU Power(W)\tinst_executed_cs\tsm_inst_executed_texture\tsm_inst_executed_global_loads\tm_inst_executed_global_stores\tthreads_launched\tgpu_busy\tsm_executed_ipc\tsm_issued_ipc\tl2_write_bytes\tl2_read_bytes\tsm_active_cycles\tsm_active_warps\tsm_warps_launched\n");

/*
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_executed_ipc");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_issued_ipc");
GetNvPmApi()->AddCounterByName(hNVPMContext,"l2_write_bytes");
GetNvPmApi()->AddCounterByName(hNVPMContext,"l2_read_bytes");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_active_cycles");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_active_warps");
GetNvPmApi()->AddCounterByName(hNVPMContext,"sm_warps_launched");
*/

        char gpufreq[100];
	char gputemp[100];
	char gpubench[100];
	char gpuvoltage[100];

	
	
 	
	FILE *benchmarks = fopen("./benchmarks","r");



        

        struct timespec tsample={0,0};
	float timenow,starttime;

        clock_gettime(CLOCK_MONOTONIC,&tsample);
        
	starttime = (tsample.tv_sec*1.0e9+tsample.tv_nsec)/1000000;

	gpu_power_total    = 0;
	
	
	sample_count_gpu = 0;

	while(1) {

		start_flag = 1;
		pthread_mutex_lock(&mutex_gpu);
		if(start_gpu == 0)
			start_flag = 0;
		pthread_mutex_unlock(&mutex_gpu);
		if(start_flag == 0)
			continue;

		usleep(100000); //sleep for 100 ms, one sample each 100 ms
		//usleep(100);
	
		jtx1_get_ina3221(VDD_IN,  POWER,  &in_power_value);
		jtx1_get_ina3221(VDD_GPU, POWER,  &gpu_power_value);
		jtx1_get_ina3221(VDD_CPU, POWER,  &cpu_power_value);

		gpu_power_total    += gpu_power_value;
		
		sample_count_gpu++;


		//printf("samplecount %d\n",sample_count);

		if(sample_count_gpu%5==0)
		{
	
	        	getcountersvalue();

			FILE *rate = fopen("/sys/kernel/debug/clock/override.gbus/rate","r");
			FILE *voltage = fopen("/sys/kernel/debug/clock/vdd_gpu_mv","r");
			FILE *temp = fopen("/sys/devices/virtual/thermal/thermal_zone2/temp","r");

			
                	size_t elements = fread(gpufreq,1,20,rate);
			strtok(gpufreq,"\n");
			//printf("read %d rate=%s\n",elements,gpufreq);
			//if (int out=fseek(rate,0, SEEK_SET) !=0){
			//	printf("rewind failed with %d\n",out);
			//	exit(0);
			//}


			fread(gputemp,1,20,temp);
			strtok(gputemp,"\n");
		
			rewind(benchmarks);			
			fread(gpubench,1,20,benchmarks);
			//printf("bench=%s\n",gpubench);
			
			fread(gpuvoltage,1,20,voltage);
			strtok(gpuvoltage,"\n");
			//get time
                	clock_gettime(CLOCK_MONOTONIC,&tsample);
			timenow = (tsample.tv_sec*1.0e9+tsample.tv_nsec)/1000000;


			fclose(rate);
			fclose(temp);
			fclose(voltage);
			

			if(gpubench[0]!='\n')
			{
				strtok(gpubench,"\n");

	        		//fprintf(fp_log, "%.0f %s 1 %s %s %f ", (timenow-starttime), gpubench, gpufreq, gputemp, (gpu_power_total/5));

				int gpufreqmh = atoi(gpufreq)/1000000;
				int gputempdeg = atoi(gputemp)/1000;
				float gpuvoltagev = atoi(gpuvoltage)/1000.0;
				int timesample = int(timenow-starttime);
				//fprintf(fp_log, "%d %s 1 %d %d %f %f ", sample_count, gpubench, gpufreqmh , gputempdeg,gpuvoltagev,(gpu_power_total/5)/1000);
				fprintf(fp_log, "%d %s 1 %d %d %f %f ", timesample, gpubench, gpufreqmh , gputempdeg,gpuvoltagev,(gpu_power_total/5)/1000);


	//		fprintf(fp_log," %f %f %f %f %f %f %f %f %f %f %f %f %f %f \n",result1,result2,result3,result4,result5,result6,result7,result8,result9,result10,result11,result12,result13,result14);

fprintf(fp_log," %f %f %f %f %f %f %f %f %f %f %f %f %f\n",result1,result2,result3,result4,result5,result6,result7,result8,result9,result10,result11,result12,result13);

			//	fprintf(fp_log,"%d %d %d %d %d %d\n",(int)result1,(int)result2,(int)result3,(int)result4,(int)result5,(int)result6);
			//read counters
			//printf("Read counters\n");
			gpu_power_total    = 0;
			
			
		
			}
		
	
		}

		
		stop_flag = 0;
		pthread_mutex_lock(&mutex_gpu);
		if (stop_gpu == 1) {
			stop_flag = 1;
		}
		pthread_mutex_unlock(&mutex_gpu);

		if(stop_flag==1) {
			printf("breaking sampling  thread\n");
			break;
		}
	}
	fclose(fp_log);
	fclose(benchmarks);
	return 0;
}

void *jetsontx1_read_sample_pthread() {

	pthread_mutex_init(&mutex_gpu, NULL);

	pthread_mutex_lock(&mutex_gpu);
	stop_gpu = 0;
	start_gpu = 0;
	pthread_mutex_unlock(&mutex_gpu);

	head_gpu = (struct jetsontx1_sample *)malloc(sizeof (struct jetsontx1_sample));
	head_gpu->next = NULL;


	cpu_set_t cpu_set2;
	CPU_SET(1, &cpu_set2);
	printf("create thread\n");
	pthread_create(&thread_ID_gpu, NULL, jetsontx1_read_samples, head_gpu);
	sched_setaffinity(thread_ID_gpu, sizeof(cpu_set_t), &cpu_set2);

//	pthread_join(thread_ID, &exit_status);


	return head_gpu;
}


void jetsontx1_read_sample_start( ) {

	pthread_mutex_lock(&mutex_gpu);

	//printf("mutex is locked\n");

	sample_count_gpu = 0;
	gpu_power_total    = 0;
	


	start_gpu = 1;
	stop_gpu  = 0;
	pthread_mutex_unlock(&mutex_gpu);
}


void jetsontx1_read_sample_stop( ) {

	pthread_mutex_lock(&mutex_gpu);
	stop_gpu = 1;
	start_gpu = 0;
	pthread_mutex_unlock(&mutex_gpu);
	printf("join thread\n");
	pthread_join(thread_ID_gpu, &exit_status_gpu);
	pthread_detach(thread_ID_gpu);
}


void jetsontx1_clear_sample_pthread(struct jetsontx1_sample *head_gpu) {

	struct jetsontx1_sample *sample = head_gpu;
	while (sample != (struct jetsontx1_sample *)NULL) {
		struct jetsontx1_sample *next = sample->next;
		free (sample);
		sample = next;
	}
	pthread_mutex_destroy(&mutex_gpu);
	printf("clear thread\n");
}
