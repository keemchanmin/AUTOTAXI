################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../Application/AppScheduler.c" \
"../Application/SCU.c" 

COMPILED_SRCS += \
"Application/AppScheduler.src" \
"Application/SCU.src" 

C_DEPS += \
"./Application/AppScheduler.d" \
"./Application/SCU.d" 

OBJS += \
"Application/AppScheduler.o" \
"Application/SCU.o" 


# Each subdirectory must supply rules for building sources it contributes
"Application/AppScheduler.src":"../Application/AppScheduler.c" "Application/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/A/aurix_final/TC275_SUB_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"Application/AppScheduler.o":"Application/AppScheduler.src" "Application/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"Application/SCU.src":"../Application/SCU.c" "Application/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/A/aurix_final/TC275_SUB_CONTROL_UNIT/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"Application/SCU.o":"Application/SCU.src" "Application/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-Application

clean-Application:
	-$(RM) ./Application/AppScheduler.d ./Application/AppScheduler.o ./Application/AppScheduler.src ./Application/SCU.d ./Application/SCU.o ./Application/SCU.src

.PHONY: clean-Application

