################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/headers/source/DSP2833x_GlobalVariableDefs.obj: ../ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/headers/source/DSP2833x_GlobalVariableDefs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.3.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.3.LTS/include" --include_path="D:/WorkSpace/DSP_Space/AccGyroPro_YW_28335/Driver&HAL/28335" --include_path="D:/WorkSpace/DSP_Space/AccGyroPro_YW_28335/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/common/include" --include_path="D:/WorkSpace/DSP_Space/AccGyroPro_YW_28335/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/headers/include" --include_path="D:/WorkSpace/DSP_Space/AccGyroPro_YW_28335" --advice:performance=all -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/headers/source/DSP2833x_GlobalVariableDefs.d_raw" --obj_directory="ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/headers/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


