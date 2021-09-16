
/*******************************************************************************
--------------------------------------------------------------------------------

		File		: 		Monitoring functions - monitoring.c

		Date		: 		15/09/2021
		
		Author		:		Deepthi Nanduri
		
		UserID		:		fc20725@bristol.ac.uk
		
		Description	:		This file contains the functions to carry out power monitoring 
							prologue, stop and epilogue. 
							
----------------------------------------------------------------------------------
**********************************************************************************/

#include "monitoring.h"

struct a57_sample *head_cpu;
struct jetsontx1_sample *head_gpu;


// CPU power monitoring prologue, epilogue and stop functions 
void power_monitoring_prologue_cpu () {

	head_cpu = (struct a57_sample *)a57_read_sample_pthread();

	a57_read_sample_start();
}


void power_monitoring_stop_cpu() {
	
	a57_read_sample_stop();
}


void power_monitoring_epilogue_cpu() {
       
	power_monitoring_stop_cpu();
	
	a57_clear_sample_pthread(head_cpu);
}


// GPU power monitoring prologue, epilogue and stop functions 
void power_monitoring_prologue_gpu () {

	head_gpu = (struct jetsontx1_sample *)jetsontx1_read_sample_pthread();

	jetsontx1_read_sample_start();
}

void power_monitoring_stop_gpu() {
	
	jetsontx1_read_sample_stop();
}

void power_monitoring_epilogue_gpu() {
 
	power_monitoring_stop_gpu();
	
	jetsontx1_clear_sample_pthread(head_gpu);
}

