################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
main_4th_GGyro.obj: ../main_4th_GGyro.cpp $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_17.9.0.STS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -O0 --opt_for_speed=5 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_17.9.0.STS/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/libraries/math/FPUfastRTS/c28/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/libraries/math/IQmath/c28/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/common/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/headers/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th" --advice:performance=all -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="main_4th_GGyro.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


