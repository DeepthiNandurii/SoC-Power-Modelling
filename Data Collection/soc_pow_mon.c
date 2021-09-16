
/*******************************************************************************
--------------------------------------------------------------------------------

		File		: 		Wrapper Script - soc_pow_mon.c

		Date		: 		15/09/2021
		
		Author		:		Deepthi Nanduri
		
		UserID		:		fc20725@bristol.ac.uk
		
		Description	:		This file is the wrapper script to control the CPU & GPU frequencies to be set
							and launches the scripts to start Rodinia benchmark run on Tegra SoC
							
----------------------------------------------------------------------------------
**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include "monitoring.h"
#include "jtx1inst.h"
#include "unistd.h"
#include <cuda_runtime.h>
#include "cusparse.h"

int pc,bm;

int main(void) {

	printf("\n\t\t CPU POW MON:: Starting power monitor...\n");

	int freqs_cpu[17] = {102000, 204000, 307200, 403200, 518400, 614400, 710400, 825600, 921600, 1036800, 1132800, 1224000, 1326000, 1428000, 1555500, 1632000, 1734000};	//CPU frequency in kHz
	int freqs_gpu[13] = {76800000,153600000,230400000,307200000,384000000,460800000,537600000,614400000,691200000,768000000,844800000,921600000,998400000};    //GPU frequencies in MHz 
	
	char cmdbuf[256];
		
		   for(pc=0;pc<13;pc++)         //For all the 13 GPU counters 
		   {
			   snprintf(cmdbuf,sizeof(cmdbuf),"./jetson_clocks_cool.sh --set 76800000"); 
			   system(cmdbuf);
			   usleep(10000000); //wait for same time to setttle temp
			   power_monitoring_prologue_gpu ();
			   
               for(int gfreq=0;gfreq<1;gfreq++)
			   { 
		               printf("Setting GPU freqs to %d\n",freqs_gpu[gfreq]);
					   snprintf(cmdbuf,sizeof(cmdbuf),"./jetson_clocks.sh --set %d",freqs_gpu[gfreq]);
					   system(cmdbuf);
					   

					   snprintf(cmdbuf,sizeof(cmdbuf),"./cpu_clocks_cool.sh --set 102000");
						int ret = system(cmdbuf);
	
						if (ret == -1)
						{
							printf("\tThis system method failed\n");
						}
	
						usleep(100000000); //wait for some time to setttle temp	

						printf("\tStarting Monitoring prologue...\n");
						power_monitoring_prologue_cpu ();
						printf("\tMonitoring prologue Done...\n");

						for(int cfreq=6;cfreq<7;cfreq++)
						{
							printf("\t cpu_pow_mon.c::main() : Setting CPU frequency (kHz) to %d\n",freqs_cpu[cfreq]);

							snprintf(cmdbuf,sizeof(cmdbuf),"./cpu_clocks.sh --set %d",freqs_cpu[cfreq]);
							system(cmdbuf);

					   		//launch benchmarks
					   		printf("\tLaunching Rodinia BM...\n");
					  		system("./rodinia/run_edited.sh");
					   		printf("\tFinished Rodinia BM...\n");
						} 

							printf("\tStarting Monitoring epilogue...\n");
							power_monitoring_epilogue_cpu();
							printf("\tMonitoring epilogue Done...\n");  
		       }       
		      power_monitoring_epilogue_gpu();
		   }
		   
		   printf("Finishing GPU power monitor\n");
		   printf("Read GPU freqs\n");
		   system("./jetson_clocks.sh --read");
		   


	printf("\t Finishing CPU power monitor...\n");

	printf("\tRead CPU freqs...\n");
	system("./cpu_clocks.sh --read");


	return 0;	
}
