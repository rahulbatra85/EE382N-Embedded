################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
lwip-1.3.2/ports/stellaris/perf.obj: ../lwip-1.3.2/ports/stellaris/perf.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M3 --code_state=16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="C:/Users/IBM_ADMIN/EE382NProjects/2_factor_auth" --include_path="{PROJECT_LOC}/lwip-1.3.2/src/include" --include_path="{PROJECT_LOC}/lwip-1.3.2/src/include/ipv4" --include_path="{PROJECT_LOC}/lwip-1.3.2/src/include/apps" --include_path="{PROJECT_LOC}/lwip-1.3.2/ports/stellaris/include" --gcc --define=ccs="ccs" --define=PART_LM3S8962 --diag_warning=225 --display_error_number --ual --preproc_with_compile --preproc_dependency="lwip-1.3.2/ports/stellaris/perf.pp" --obj_directory="lwip-1.3.2/ports/stellaris" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

lwip-1.3.2/ports/stellaris/sys_arch.obj: ../lwip-1.3.2/ports/stellaris/sys_arch.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M3 --code_state=16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="C:/Users/IBM_ADMIN/EE382NProjects/2_factor_auth" --include_path="{PROJECT_LOC}/lwip-1.3.2/src/include" --include_path="{PROJECT_LOC}/lwip-1.3.2/src/include/ipv4" --include_path="{PROJECT_LOC}/lwip-1.3.2/src/include/apps" --include_path="{PROJECT_LOC}/lwip-1.3.2/ports/stellaris/include" --gcc --define=ccs="ccs" --define=PART_LM3S8962 --diag_warning=225 --display_error_number --ual --preproc_with_compile --preproc_dependency="lwip-1.3.2/ports/stellaris/sys_arch.pp" --obj_directory="lwip-1.3.2/ports/stellaris" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


