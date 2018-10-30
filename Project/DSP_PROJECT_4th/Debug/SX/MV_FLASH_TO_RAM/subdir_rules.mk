################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
SX/MV_FLASH_TO_RAM/DSP2833x_CodeStartBranch.obj: ../SX/MV_FLASH_TO_RAM/DSP2833x_CodeStartBranch.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/pt-1.4" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/libraries/math/FPUfastRTS/c28/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/libraries/math/IQmath/c28/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/common/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/headers/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th" --advice:performance=all -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="SX/MV_FLASH_TO_RAM/DSP2833x_CodeStartBranch.d_raw" --obj_directory="SX/MV_FLASH_TO_RAM" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SX/MV_FLASH_TO_RAM/DSP2833x_SectionCopy_nonBIOS.obj: ../SX/MV_FLASH_TO_RAM/DSP2833x_SectionCopy_nonBIOS.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/pt-1.4" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/libraries/math/FPUfastRTS/c28/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/libraries/math/IQmath/c28/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/common/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/headers/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th" --advice:performance=all -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="SX/MV_FLASH_TO_RAM/DSP2833x_SectionCopy_nonBIOS.d_raw" --obj_directory="SX/MV_FLASH_TO_RAM" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SX/MV_FLASH_TO_RAM/DSP2833x_SysCtrl.obj: ../SX/MV_FLASH_TO_RAM/DSP2833x_SysCtrl.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/pt-1.4" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/libraries/math/FPUfastRTS/c28/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/libraries/math/IQmath/c28/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/common/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/headers/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th" --advice:performance=all -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="SX/MV_FLASH_TO_RAM/DSP2833x_SysCtrl.d_raw" --obj_directory="SX/MV_FLASH_TO_RAM" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SX/MV_FLASH_TO_RAM/DSP2833x_usDelay.obj: ../SX/MV_FLASH_TO_RAM/DSP2833x_usDelay.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/pt-1.4" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/libraries/math/FPUfastRTS/c28/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/libraries/math/IQmath/c28/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/common/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th/ti/c2000/C2000Ware_1_00_03_00/device_support/f2833x/headers/include" --include_path="D:/WorkSpace/DSP_Space/DSP_PROJECT_4th" --advice:performance=all -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="SX/MV_FLASH_TO_RAM/DSP2833x_usDelay.d_raw" --obj_directory="SX/MV_FLASH_TO_RAM" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


