################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../../../src/freeglut_callbacks.c \
../../../src/freeglut_cursor.c \
../../../src/freeglut_display.c \
../../../src/freeglut_ext.c \
../../../src/freeglut_font.c \
../../../src/freeglut_font_data.c \
../../../src/freeglut_gamemode.c \
../../../src/freeglut_geometry.c \
../../../src/freeglut_glutfont_definitions.c \
../../../src/freeglut_init.c \
../../../src/freeglut_input_devices.c \
../../../src/freeglut_joystick.c \
../../../src/freeglut_main.c \
../../../src/freeglut_menu.c \
../../../src/freeglut_misc.c \
../../../src/freeglut_overlay.c \
../../../src/freeglut_spaceball.c \
../../../src/freeglut_state.c \
../../../src/freeglut_stroke_mono_roman.c \
../../../src/freeglut_stroke_roman.c \
../../../src/freeglut_structure.c \
../../../src/freeglut_teapot.c \
../../../src/freeglut_videoresize.c \
../../../src/freeglut_window.c 

OBJS += \
./freeglut_callbacks.o \
./freeglut_cursor.o \
./freeglut_display.o \
./freeglut_ext.o \
./freeglut_font.o \
./freeglut_font_data.o \
./freeglut_gamemode.o \
./freeglut_geometry.o \
./freeglut_glutfont_definitions.o \
./freeglut_init.o \
./freeglut_input_devices.o \
./freeglut_joystick.o \
./freeglut_main.o \
./freeglut_menu.o \
./freeglut_misc.o \
./freeglut_overlay.o \
./freeglut_spaceball.o \
./freeglut_state.o \
./freeglut_stroke_mono_roman.o \
./freeglut_stroke_roman.o \
./freeglut_structure.o \
./freeglut_teapot.o \
./freeglut_videoresize.o \
./freeglut_window.o 

C_DEPS += \
./freeglut_callbacks.d \
./freeglut_cursor.d \
./freeglut_display.d \
./freeglut_ext.d \
./freeglut_font.d \
./freeglut_font_data.d \
./freeglut_gamemode.d \
./freeglut_geometry.d \
./freeglut_glutfont_definitions.d \
./freeglut_init.d \
./freeglut_input_devices.d \
./freeglut_joystick.d \
./freeglut_main.d \
./freeglut_menu.d \
./freeglut_misc.d \
./freeglut_overlay.d \
./freeglut_spaceball.d \
./freeglut_state.d \
./freeglut_stroke_mono_roman.d \
./freeglut_stroke_roman.d \
./freeglut_structure.d \
./freeglut_teapot.d \
./freeglut_videoresize.d \
./freeglut_window.d 


# Each subdirectory must supply rules for building sources it contributes
freeglut_callbacks.o: ../../../src/freeglut_callbacks.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_cursor.o: ../../../src/freeglut_cursor.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_display.o: ../../../src/freeglut_display.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_ext.o: ../../../src/freeglut_ext.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_font.o: ../../../src/freeglut_font.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_font_data.o: ../../../src/freeglut_font_data.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_gamemode.o: ../../../src/freeglut_gamemode.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_geometry.o: ../../../src/freeglut_geometry.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_glutfont_definitions.o: ../../../src/freeglut_glutfont_definitions.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_init.o: ../../../src/freeglut_init.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_input_devices.o: ../../../src/freeglut_input_devices.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_joystick.o: ../../../src/freeglut_joystick.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_main.o: ../../../src/freeglut_main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_menu.o: ../../../src/freeglut_menu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_misc.o: ../../../src/freeglut_misc.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_overlay.o: ../../../src/freeglut_overlay.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_spaceball.o: ../../../src/freeglut_spaceball.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_state.o: ../../../src/freeglut_state.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_stroke_mono_roman.o: ../../../src/freeglut_stroke_mono_roman.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_stroke_roman.o: ../../../src/freeglut_stroke_roman.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_structure.o: ../../../src/freeglut_structure.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_teapot.o: ../../../src/freeglut_teapot.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_videoresize.o: ../../../src/freeglut_videoresize.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeglut_window.o: ../../../src/freeglut_window.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFREEGLUT_EXPORTS -DHAVE_DLFCN_H -DHAVE_FCNTL_H -I/usr/include/ -I../../../../../src/sixense_simple3d/include -O3 -Wall -c -fmessage-length=0 -m32 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


