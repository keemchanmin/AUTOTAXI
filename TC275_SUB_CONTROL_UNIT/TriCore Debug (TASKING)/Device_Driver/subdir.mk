################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../Device_Driver/Asclin3.c" \
"../Device_Driver/Can.c" \
"../Device_Driver/Door.c" \
"../Device_Driver/Tof.c" 

COMPILED_SRCS += \
"Device_Driver/Asclin3.src" \
"Device_Driver/Can.src" \
"Device_Driver/Door.src" \
"Device_Driver/Tof.src" 

C_DEPS += \
"./Device_Driver/Asclin3.d" \
"./Device_Driver/Can.d" \
"./Device_Driver/Door.d" \
"./Device_Driver/Tof.d" 

OBJS += \
"Device_Driver/Asclin3.o" \
"Device_Driver/Can.o" \
"Device_Driver/Door.o" \
"Device_Driver/Tof.o" 


# Each subdirectory must supply rules for building sources it contributes
"Device_Driver/Asclin3.src":"../Device_Driver/Asclin3.c" "Device_Driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/A/aurix_final/TC275_SUB_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"Device_Driver/Asclin3.o":"Device_Driver/Asclin3.src" "Device_Driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"Device_Driver/Can.src":"../Device_Driver/Can.c" "Device_Driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/A/aurix_final/TC275_SUB_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"Device_Driver/Can.o":"Device_Driver/Can.src" "Device_Driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"Device_Driver/Door.src":"../Device_Driver/Door.c" "Device_Driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/A/aurix_final/TC275_SUB_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"Device_Driver/Door.o":"Device_Driver/Door.src" "Device_Driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"Device_Driver/Tof.src":"../Device_Driver/Tof.c" "Device_Driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/A/aurix_final/TC275_SUB_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"Device_Driver/Tof.o":"Device_Driver/Tof.src" "Device_Driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-Device_Driver

clean-Device_Driver:
	-$(RM) ./Device_Driver/Asclin3.d ./Device_Driver/Asclin3.o ./Device_Driver/Asclin3.src ./Device_Driver/Can.d ./Device_Driver/Can.o ./Device_Driver/Can.src ./Device_Driver/Door.d ./Device_Driver/Door.o ./Device_Driver/Door.src ./Device_Driver/Tof.d ./Device_Driver/Tof.o ./Device_Driver/Tof.src

.PHONY: clean-Device_Driver

