################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../../progs/demos/sixense_simple3d/sixense_simple3d.cpp 

OBJS += \
./sixense_simple3d.o 

CPP_DEPS += \
./sixense_simple3d.d 


# Each subdirectory must supply rules for building sources it contributes
sixense_simple3d.o: ../../../progs/demos/sixense_simple3d/sixense_simple3d.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../../../../../src/sixense_simple3d/include -I../../../../../include -O0 -Wall -c -fmessage-length=0 -m64 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


