################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../SX/MV_FLASH_TO_RAM/F2833x_nonBIOS_flash.cmd 

ASM_SRCS += \
../SX/MV_FLASH_TO_RAM/DSP2833x_CodeStartBranch.asm \
../SX/MV_FLASH_TO_RAM/DSP2833x_SectionCopy_nonBIOS.asm \
../SX/MV_FLASH_TO_RAM/DSP2833x_usDelay.asm 

C_SRCS += \
../SX/MV_FLASH_TO_RAM/DSP2833x_SysCtrl.c 

C_DEPS += \
./SX/MV_FLASH_TO_RAM/DSP2833x_SysCtrl.d 

OBJS += \
./SX/MV_FLASH_TO_RAM/DSP2833x_CodeStartBranch.obj \
./SX/MV_FLASH_TO_RAM/DSP2833x_SectionCopy_nonBIOS.obj \
./SX/MV_FLASH_TO_RAM/DSP2833x_SysCtrl.obj \
./SX/MV_FLASH_TO_RAM/DSP2833x_usDelay.obj 

ASM_DEPS += \
./SX/MV_FLASH_TO_RAM/DSP2833x_CodeStartBranch.d \
./SX/MV_FLASH_TO_RAM/DSP2833x_SectionCopy_nonBIOS.d \
./SX/MV_FLASH_TO_RAM/DSP2833x_usDelay.d 

OBJS__QUOTED += \
"SX\MV_FLASH_TO_RAM\DSP2833x_CodeStartBranch.obj" \
"SX\MV_FLASH_TO_RAM\DSP2833x_SectionCopy_nonBIOS.obj" \
"SX\MV_FLASH_TO_RAM\DSP2833x_SysCtrl.obj" \
"SX\MV_FLASH_TO_RAM\DSP2833x_usDelay.obj" 

C_DEPS__QUOTED += \
"SX\MV_FLASH_TO_RAM\DSP2833x_SysCtrl.d" 

ASM_DEPS__QUOTED += \
"SX\MV_FLASH_TO_RAM\DSP2833x_CodeStartBranch.d" \
"SX\MV_FLASH_TO_RAM\DSP2833x_SectionCopy_nonBIOS.d" \
"SX\MV_FLASH_TO_RAM\DSP2833x_usDelay.d" 

ASM_SRCS__QUOTED += \
"../SX/MV_FLASH_TO_RAM/DSP2833x_CodeStartBranch.asm" \
"../SX/MV_FLASH_TO_RAM/DSP2833x_SectionCopy_nonBIOS.asm" \
"../SX/MV_FLASH_TO_RAM/DSP2833x_usDelay.asm" 

C_SRCS__QUOTED += \
"../SX/MV_FLASH_TO_RAM/DSP2833x_SysCtrl.c" 


