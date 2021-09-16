# SoC-Power-Modelling
# SoC-Hybrid power model for NVIDIA Tegra SoC 

*******************************************************************************
# Step 1: Data Collection (Requires NVIDIA Jetson TX1)
1)  Go to Data_Collection/
2)  Run the makefile to compile the files: 
            make -f Makefile_soc soc_pow
4)  Run executable to start the benchmark run and collect data: 
            sudo ./pmon_soc
5)  Raw data file is dumped: 
            CPU-Events: power_measurement_log_a57.dat.
            GPU-Events: power_measurement_log_c0...12.dat.
            
*******************************************************************************

# Step 2: Power Prediction
1)  Use CPU unified equation to predict the power consumption of ARM Cortex-A57 CPU
2)  Use GPU unified equation (adopted from the work of Nunez-Yanez et al. 
    to predict the power consumption of GPU. 
3)  Sum of the the power predicted in Step-1 and Step-2 is the overall SoC power
    consumption. 
4)  Predicted power will be validated against the readings from on-module power monitors. 

#################################################################################

CREDITS:
1)  Dr Jose Nunez Yanez, Department of Electrical and Electronic Engineering, University of Bristol
2)  Dr Kris Nikov, Department of Computer Science, University of Bristol

This project work serves as an extension to the work carried out by Dr Jose Nunez-Yanez
and Dr Kris Nikov in their published work for the Maxwell GPU on Jetson TX1:

"Run-Time Power Modelling in Embedded GPUs with Dynamic Voltage and Frequency 
Scaling" published in the PARMA-DITAM 2020 as part of the HIPEAC conference.
Authors: Jose Nunez-Yanez, Kris Nikov, Kerstin Eder, Mohammad Hosseinabady
Department of Electrical and Electronic Engineering, University of Bristol, 
United Kingdom

*********************************************************************************

REFERENCES:
1)  GPU Power Model: https://github.com/kranik/ARMPM_BUILDMODEL/tree/GPU_tx1
2)  Original Modeling scripts adopted from Dr Kris Nikov's Doctoral Thesis
    project: https://github.com/kranik/ARMPM_BUILDMODEL/tree/master
3)  Kris Nikov. Robust Energy and Power Predictor Selection. url: 
    https://github.com/TSL-UOB/TP-REPPS
4)  Kris Nikov and Jose Nunez-Yanez. "Intra and Inter-Core Power Modelling
    for Single-ISA Heterogeneous Processors". In: International Journal
    of Embedded Systems (2020)

#################################################################################
