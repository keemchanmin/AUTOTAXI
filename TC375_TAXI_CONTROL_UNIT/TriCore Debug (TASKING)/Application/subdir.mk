################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../Application/AppScheduler.c" \
"../Application/CCU.c" \
"../Application/Vehicle_State_Controller.c" 

COMPILED_SRCS += \
"Application/AppScheduler.src" \
"Application/CCU.src" \
"Application/Vehicle_State_Controller.src" 

C_DEPS += \
"./Application/AppScheduler.d" \
"./Application/CCU.d" \
"./Application/Vehicle_State_Controller.d" 

OBJS += \
"Application/AppScheduler.o" \
"Application/CCU.o" \
"Application/Vehicle_State_Controller.o" 


# Each subdirectory must supply rules for building sources it contributes
"Application/AppScheduler.src":"../Application/AppScheduler.c" "Application/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc37x "-fC:/Users/A/aurix_final/TC375_TAXI_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"Application/AppScheduler.o":"Application/AppScheduler.src" "Application/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"Application/CCU.src":"../Application/CCU.c" "Application/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc37x "-fC:/Users/A/aurix_final/TC375_TAXI_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"Application/CCU.o":"Application/CCU.src" "Application/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"Application/Vehicle_State_Controller.src":"../Application/Vehicle_State_Controller.c" "Application/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc37x "-fC:/Users/A/aurix_final/TC375_TAXI_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"Application/Vehicle_State_Controller.o":"Application/Vehicle_State_Controller.src" "Application/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-Application

clean-Application:
	-$(RM) ./Application/AppScheduler.d ./Application/AppScheduler.o ./Application/AppScheduler.src ./Application/CCU.d ./Application/CCU.o ./Application/CCU.src ./Application/Vehicle_State_Controller.d ./Application/Vehicle_State_Controller.o ./Application/Vehicle_State_Controller.src

.PHONY: clean-Application

