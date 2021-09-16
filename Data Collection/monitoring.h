
/*******************************************************************************
--------------------------------------------------------------------------------

		File		: 		Header file - monitoring.h

		Date		: 		15/09/2021
		
		Author		:		Deepthi Nanduri (Script adopted from work of Nunez-Yanez et al.[4])
		
		UserID		:		fc20725@bristol.ac.uk
		
		Description	:		This header declares all the functions associated with 
							CPU and GPU data collection. 
							
----------------------------------------------------------------------------------
**********************************************************************************/


#ifndef __MONITORING__H__
#define __MONITORING__H__

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

struct jetsontx1_sample {

	struct timeval start_time;
	struct timeval end_time;


	double  gpu_power;
	double  cpu_power;
	double  in_power;


	struct jetsontx1_sample *next;
};


struct a57_sample {

	struct timeval start_time;
	struct timeval end_time;

	double  cpu_power;
	double  in_power;
	struct a57_sample *next;
};


void *jetsontx1_read_samples(void *head);
void *jetsontx1_read_sample_pthread();
void jetsontx1_read_sample_start();
void jetsontx1_read_sample_stop(); 
void jetsontx1_save_average_pthread(struct jetsontx1_sample *head, char *file_name);
void jetsontx1_clear_sample_pthread(struct jetsontx1_sample *head);


void *a57_read_samples(void *head);
void *a57_read_sample_pthread();
void a57_read_sample_start();
void a57_read_sample_stop(); 
void a57_save_average_pthread(struct a57_sample *head, char *file_name);
void a57_clear_sample_pthread(struct a57_sample *head);

void file_power_profile_create (char *file_name);
void power_monitoring_prologue_cpu();
void power_monitoring_epilogue_cpu();
void power_monitoring_stop_cpu();

void power_monitoring_prologue_gpu();
void power_monitoring_epilogue_gpu();
void power_monitoring_stop_gpu();



#endif //__MONITORING__H__
