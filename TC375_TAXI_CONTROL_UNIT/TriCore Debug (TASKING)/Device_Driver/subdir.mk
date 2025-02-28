################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../Device_Driver/Asclin0.c" \
"../Device_Driver/UartPacket.c" \
"../Device_Driver/can.c" 

COMPILED_SRCS += \
"Device_Driver/Asclin0.src" \
"Device_Driver/UartPacket.src" \
"Device_Driver/can.src" 

C_DEPS += \
"./Device_Driver/Asclin0.d" \
"./Device_Driver/UartPacket.d" \
"./Device_Driver/can.d" 

OBJS += \
"Device_Driver/Asclin0.o" \
"Device_Driver/UartPacket.o" \
"Device_Driver/can.o" 


# Each subdirectory must supply rules for building sources it contributes
"Device_Driver/Asclin0.src":"../Device_Driver/Asclin0.c" "Device_Driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc37x "-fC:/Users/A/aurix_final/TC375_TAXI_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"Device_Driver/Asclin0.o":"Device_Driver/Asclin0.src" "Device_Driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"Device_Driver/UartPacket.src":"../Device_Driver/UartPacket.c" "Device_Driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc37x "-fC:/Users/A/aurix_final/TC375_TAXI_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"Device_Driver/UartPacket.o":"Device_Driver/UartPacket.src" "Device_Driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"Device_Driver/can.src":"../Device_Driver/can.c" "Device_Driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc37x "-fC:/Users/A/aurix_final/TC375_TAXI_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"Device_Driver/can.o":"Device_Driver/can.src" "Device_Driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-Device_Driver

clean-Device_Driver:
	-$(RM) ./Device_Driver/Asclin0.d ./Device_Driver/Asclin0.o ./Device_Driver/Asclin0.src ./Device_Driver/UartPacket.d ./Device_Driver/UartPacket.o ./Device_Driver/UartPacket.src ./Device_Driver/can.d ./Device_Driver/can.o ./Device_Driver/can.src

.PHONY: clean-Device_Driver

