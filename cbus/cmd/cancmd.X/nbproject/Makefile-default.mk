#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile

# Environment
MKDIR=mkdir -p
RM=rm -f 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cancmd.X.${IMAGE_TYPE}.cof
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cancmd.X.${IMAGE_TYPE}.cof
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/Boot3.o ${OBJECTDIR}/_ext/1472/c018.o ${OBJECTDIR}/_ext/1472/can_send.o ${OBJECTDIR}/_ext/1472/cancmd.o ${OBJECTDIR}/_ext/1472/cbus_common.o ${OBJECTDIR}/_ext/1472/commands.o ${OBJECTDIR}/_ext/1472/isr_high.o ${OBJECTDIR}/_ext/1472/isr_low.o ${OBJECTDIR}/_ext/1472/mode_cmds.o ${OBJECTDIR}/_ext/1472/p18F2480.o ${OBJECTDIR}/_ext/1472/packet_gen.o ${OBJECTDIR}/_ext/1472/power_cmds.o ${OBJECTDIR}/_ext/1472/program_cmds.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH=/usr/lib/jvm/java-6-openjdk/jre/bin/
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC=/opt/microchip/mplabc18/v3.40/bin/mcc18
# MP_BC is not defined
MP_AS=/opt/microchip/mplabc18/v3.40/bin/../mpasm/MPASMWIN
MP_LD=/opt/microchip/mplabc18/v3.40/bin/mplink
MP_AR=/opt/microchip/mplabc18/v3.40/bin/mplib
# MP_BC is not defined
MP_CC_DIR=/opt/microchip/mplabc18/v3.40/bin
# MP_BC_DIR is not defined
MP_AS_DIR=/opt/microchip/mplabc18/v3.40/bin/../mpasm
MP_LD_DIR=/opt/microchip/mplabc18/v3.40/bin
MP_AR_DIR=/opt/microchip/mplabc18/v3.40/bin
# MP_BC_DIR is not defined

# This makefile will use a C preprocessor to generate dependency files
MP_CPP=/opt/microchip/mplabx/mplab_ide/mplab_ide/modules/../../bin/mplab-cpp

.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/cancmd.X.${IMAGE_TYPE}.cof

MP_PROCESSOR_OPTION=18F2480
MP_PROCESSOR_OPTION_LD=18f2480
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0x3dc0 -u_DEBUGCODELEN=0x240 -u_DEBUGDATASTART=0x2f4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: createRevGrep
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
__revgrep__:   nbproject/Makefile-${CND_CONF}.mk
	@echo 'grep -q $$@' > __revgrep__
	@echo 'if [ "$$?" -ne "0" ]; then' >> __revgrep__
	@echo '  exit 0' >> __revgrep__
	@echo 'else' >> __revgrep__
	@echo '  exit 1' >> __revgrep__
	@echo 'fi' >> __revgrep__
	@chmod +x __revgrep__
else
__revgrep__:   nbproject/Makefile-${CND_CONF}.mk
	@echo 'grep -q $$@' > __revgrep__
	@echo 'if [ "$$?" -ne "0" ]; then' >> __revgrep__
	@echo '  exit 0' >> __revgrep__
	@echo 'else' >> __revgrep__
	@echo '  exit 1' >> __revgrep__
	@echo 'fi' >> __revgrep__
	@chmod +x __revgrep__
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
.PHONY: ${OBJECTDIR}/_ext/1472/Boot3.o
${OBJECTDIR}/_ext/1472/Boot3.o: ../Boot3.asm __revgrep__ nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PK3=1 -q -p$(MP_PROCESSOR_OPTION)  -l"${OBJECTDIR}/_ext/1472/Boot3.lst" -e"${OBJECTDIR}/_ext/1472/Boot3.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/_ext/1472/Boot3.o" ../Boot3.asm 
else 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PK3=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l"${OBJECTDIR}/_ext/1472/Boot3.lst" -e"${OBJECTDIR}/_ext/1472/Boot3.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/_ext/1472/Boot3.o" ../Boot3.asm 
endif 
	@cat  "${OBJECTDIR}/_ext/1472/Boot3.err" | sed -e 's/\x0D$$//' -e 's/\(^Warning\|^Error\|^Message\)\(\[[0-9]*\]\) *\(.*\) \([0-9]*\) : \(.*$$\)/\3:\4: \1\2: \5/g'
	@./__revgrep__ "^Error" ${OBJECTDIR}/_ext/1472/Boot3.err
.PHONY: ${OBJECTDIR}/_ext/1472/p18F2480.o
${OBJECTDIR}/_ext/1472/p18F2480.o: ../p18F2480.asm __revgrep__ nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PK3=1 -q -p$(MP_PROCESSOR_OPTION)  -l"${OBJECTDIR}/_ext/1472/p18F2480.lst" -e"${OBJECTDIR}/_ext/1472/p18F2480.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/_ext/1472/p18F2480.o" ../p18F2480.asm 
else 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PK3=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l"${OBJECTDIR}/_ext/1472/p18F2480.lst" -e"${OBJECTDIR}/_ext/1472/p18F2480.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/_ext/1472/p18F2480.o" ../p18F2480.asm 
endif 
	@cat  "${OBJECTDIR}/_ext/1472/p18F2480.err" | sed -e 's/\x0D$$//' -e 's/\(^Warning\|^Error\|^Message\)\(\[[0-9]*\]\) *\(.*\) \([0-9]*\) : \(.*$$\)/\3:\4: \1\2: \5/g'
	@./__revgrep__ "^Error" ${OBJECTDIR}/_ext/1472/p18F2480.err
else
.PHONY: ${OBJECTDIR}/_ext/1472/Boot3.o
${OBJECTDIR}/_ext/1472/Boot3.o: ../Boot3.asm __revgrep__ nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION)  -l"${OBJECTDIR}/_ext/1472/Boot3.lst" -e"${OBJECTDIR}/_ext/1472/Boot3.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/_ext/1472/Boot3.o" ../Boot3.asm 
else 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l"${OBJECTDIR}/_ext/1472/Boot3.lst" -e"${OBJECTDIR}/_ext/1472/Boot3.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/_ext/1472/Boot3.o" ../Boot3.asm 
endif 
	@cat  "${OBJECTDIR}/_ext/1472/Boot3.err" | sed -e 's/\x0D$$//' -e 's/\(^Warning\|^Error\|^Message\)\(\[[0-9]*\]\) *\(.*\) \([0-9]*\) : \(.*$$\)/\3:\4: \1\2: \5/g'
	@./__revgrep__ "^Error" ${OBJECTDIR}/_ext/1472/Boot3.err
.PHONY: ${OBJECTDIR}/_ext/1472/p18F2480.o
${OBJECTDIR}/_ext/1472/p18F2480.o: ../p18F2480.asm __revgrep__ nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION)  -l"${OBJECTDIR}/_ext/1472/p18F2480.lst" -e"${OBJECTDIR}/_ext/1472/p18F2480.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/_ext/1472/p18F2480.o" ../p18F2480.asm 
else 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l"${OBJECTDIR}/_ext/1472/p18F2480.lst" -e"${OBJECTDIR}/_ext/1472/p18F2480.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/_ext/1472/p18F2480.o" ../p18F2480.asm 
endif 
	@cat  "${OBJECTDIR}/_ext/1472/p18F2480.err" | sed -e 's/\x0D$$//' -e 's/\(^Warning\|^Error\|^Message\)\(\[[0-9]*\]\) *\(.*\) \([0-9]*\) : \(.*$$\)/\3:\4: \1\2: \5/g'
	@./__revgrep__ "^Error" ${OBJECTDIR}/_ext/1472/p18F2480.err
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/packet_gen.o: ../packet_gen.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/packet_gen.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/packet_gen.o   ../packet_gen.c  > ${OBJECTDIR}/_ext/1472/packet_gen.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/packet_gen.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/packet_gen.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/packet_gen.o.temp ../packet_gen.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/packet_gen.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/packet_gen.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/packet_gen.o.d
else
	cat ${OBJECTDIR}/_ext/1472/packet_gen.o.temp >> ${OBJECTDIR}/_ext/1472/packet_gen.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/can_send.o: ../can_send.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/can_send.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/can_send.o   ../can_send.c  > ${OBJECTDIR}/_ext/1472/can_send.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/can_send.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/can_send.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/can_send.o.temp ../can_send.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/can_send.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/can_send.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/can_send.o.d
else
	cat ${OBJECTDIR}/_ext/1472/can_send.o.temp >> ${OBJECTDIR}/_ext/1472/can_send.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/power_cmds.o: ../power_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/power_cmds.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/power_cmds.o   ../power_cmds.c  > ${OBJECTDIR}/_ext/1472/power_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/power_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/power_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/power_cmds.o.temp ../power_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/power_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/power_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/power_cmds.o.d
else
	cat ${OBJECTDIR}/_ext/1472/power_cmds.o.temp >> ${OBJECTDIR}/_ext/1472/power_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/commands.o: ../commands.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/commands.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/commands.o   ../commands.c  > ${OBJECTDIR}/_ext/1472/commands.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/commands.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/commands.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/commands.o.temp ../commands.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/commands.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/commands.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/commands.o.d
else
	cat ${OBJECTDIR}/_ext/1472/commands.o.temp >> ${OBJECTDIR}/_ext/1472/commands.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/mode_cmds.o: ../mode_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/mode_cmds.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/mode_cmds.o   ../mode_cmds.c  > ${OBJECTDIR}/_ext/1472/mode_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/mode_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/mode_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/mode_cmds.o.temp ../mode_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/mode_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/mode_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/mode_cmds.o.d
else
	cat ${OBJECTDIR}/_ext/1472/mode_cmds.o.temp >> ${OBJECTDIR}/_ext/1472/mode_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/cancmd.o: ../cancmd.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/cancmd.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/cancmd.o   ../cancmd.c  > ${OBJECTDIR}/_ext/1472/cancmd.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/cancmd.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/cancmd.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/cancmd.o.temp ../cancmd.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/cancmd.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/cancmd.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/cancmd.o.d
else
	cat ${OBJECTDIR}/_ext/1472/cancmd.o.temp >> ${OBJECTDIR}/_ext/1472/cancmd.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/isr_low.o: ../isr_low.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/isr_low.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/isr_low.o   ../isr_low.c  > ${OBJECTDIR}/_ext/1472/isr_low.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/isr_low.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/isr_low.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/isr_low.o.temp ../isr_low.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/isr_low.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/isr_low.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/isr_low.o.d
else
	cat ${OBJECTDIR}/_ext/1472/isr_low.o.temp >> ${OBJECTDIR}/_ext/1472/isr_low.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/program_cmds.o: ../program_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/program_cmds.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/program_cmds.o   ../program_cmds.c  > ${OBJECTDIR}/_ext/1472/program_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/program_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/program_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/program_cmds.o.temp ../program_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/program_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/program_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/program_cmds.o.d
else
	cat ${OBJECTDIR}/_ext/1472/program_cmds.o.temp >> ${OBJECTDIR}/_ext/1472/program_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/isr_high.o: ../isr_high.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/isr_high.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/isr_high.o   ../isr_high.c  > ${OBJECTDIR}/_ext/1472/isr_high.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/isr_high.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/isr_high.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/isr_high.o.temp ../isr_high.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/isr_high.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/isr_high.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/isr_high.o.d
else
	cat ${OBJECTDIR}/_ext/1472/isr_high.o.temp >> ${OBJECTDIR}/_ext/1472/isr_high.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/c018.o: ../c018.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/c018.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/c018.o   ../c018.c  > ${OBJECTDIR}/_ext/1472/c018.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/c018.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/c018.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/c018.o.temp ../c018.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/c018.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/c018.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/c018.o.d
else
	cat ${OBJECTDIR}/_ext/1472/c018.o.temp >> ${OBJECTDIR}/_ext/1472/c018.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/cbus_common.o: ../cbus_common.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/cbus_common.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/cbus_common.o   ../cbus_common.c  > ${OBJECTDIR}/_ext/1472/cbus_common.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/cbus_common.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/cbus_common.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/cbus_common.o.temp ../cbus_common.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/cbus_common.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/cbus_common.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/cbus_common.o.d
else
	cat ${OBJECTDIR}/_ext/1472/cbus_common.o.temp >> ${OBJECTDIR}/_ext/1472/cbus_common.o.d
endif
	${RM} __temp_cpp_output__
else
${OBJECTDIR}/_ext/1472/packet_gen.o: ../packet_gen.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/packet_gen.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/packet_gen.o   ../packet_gen.c  > ${OBJECTDIR}/_ext/1472/packet_gen.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/packet_gen.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/packet_gen.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/packet_gen.o.temp ../packet_gen.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/packet_gen.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/packet_gen.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/packet_gen.o.d
else
	cat ${OBJECTDIR}/_ext/1472/packet_gen.o.temp >> ${OBJECTDIR}/_ext/1472/packet_gen.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/can_send.o: ../can_send.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/can_send.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/can_send.o   ../can_send.c  > ${OBJECTDIR}/_ext/1472/can_send.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/can_send.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/can_send.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/can_send.o.temp ../can_send.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/can_send.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/can_send.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/can_send.o.d
else
	cat ${OBJECTDIR}/_ext/1472/can_send.o.temp >> ${OBJECTDIR}/_ext/1472/can_send.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/power_cmds.o: ../power_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/power_cmds.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/power_cmds.o   ../power_cmds.c  > ${OBJECTDIR}/_ext/1472/power_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/power_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/power_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/power_cmds.o.temp ../power_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/power_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/power_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/power_cmds.o.d
else
	cat ${OBJECTDIR}/_ext/1472/power_cmds.o.temp >> ${OBJECTDIR}/_ext/1472/power_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/commands.o: ../commands.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/commands.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/commands.o   ../commands.c  > ${OBJECTDIR}/_ext/1472/commands.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/commands.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/commands.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/commands.o.temp ../commands.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/commands.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/commands.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/commands.o.d
else
	cat ${OBJECTDIR}/_ext/1472/commands.o.temp >> ${OBJECTDIR}/_ext/1472/commands.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/mode_cmds.o: ../mode_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/mode_cmds.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/mode_cmds.o   ../mode_cmds.c  > ${OBJECTDIR}/_ext/1472/mode_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/mode_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/mode_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/mode_cmds.o.temp ../mode_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/mode_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/mode_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/mode_cmds.o.d
else
	cat ${OBJECTDIR}/_ext/1472/mode_cmds.o.temp >> ${OBJECTDIR}/_ext/1472/mode_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/cancmd.o: ../cancmd.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/cancmd.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/cancmd.o   ../cancmd.c  > ${OBJECTDIR}/_ext/1472/cancmd.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/cancmd.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/cancmd.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/cancmd.o.temp ../cancmd.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/cancmd.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/cancmd.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/cancmd.o.d
else
	cat ${OBJECTDIR}/_ext/1472/cancmd.o.temp >> ${OBJECTDIR}/_ext/1472/cancmd.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/isr_low.o: ../isr_low.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/isr_low.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/isr_low.o   ../isr_low.c  > ${OBJECTDIR}/_ext/1472/isr_low.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/isr_low.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/isr_low.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/isr_low.o.temp ../isr_low.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/isr_low.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/isr_low.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/isr_low.o.d
else
	cat ${OBJECTDIR}/_ext/1472/isr_low.o.temp >> ${OBJECTDIR}/_ext/1472/isr_low.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/program_cmds.o: ../program_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/program_cmds.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/program_cmds.o   ../program_cmds.c  > ${OBJECTDIR}/_ext/1472/program_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/program_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/program_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/program_cmds.o.temp ../program_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/program_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/program_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/program_cmds.o.d
else
	cat ${OBJECTDIR}/_ext/1472/program_cmds.o.temp >> ${OBJECTDIR}/_ext/1472/program_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/isr_high.o: ../isr_high.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/isr_high.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/isr_high.o   ../isr_high.c  > ${OBJECTDIR}/_ext/1472/isr_high.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/isr_high.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/isr_high.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/isr_high.o.temp ../isr_high.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/isr_high.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/isr_high.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/isr_high.o.d
else
	cat ${OBJECTDIR}/_ext/1472/isr_high.o.temp >> ${OBJECTDIR}/_ext/1472/isr_high.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/c018.o: ../c018.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/c018.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/c018.o   ../c018.c  > ${OBJECTDIR}/_ext/1472/c018.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/c018.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/c018.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/c018.o.temp ../c018.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/c018.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/c018.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/c018.o.d
else
	cat ${OBJECTDIR}/_ext/1472/c018.o.temp >> ${OBJECTDIR}/_ext/1472/c018.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/_ext/1472/cbus_common.o: ../cbus_common.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/_ext/1472/cbus_common.o.d 
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -ou- -ot- -ob- -op- -or- -od- -opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/cbus_common.o   ../cbus_common.c  > ${OBJECTDIR}/_ext/1472/cbus_common.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/_ext/1472/cbus_common.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/_ext/1472/cbus_common.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/_ext/1472/cbus_common.o.temp ../cbus_common.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cmd -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR}/_ext/1472 > ${OBJECTDIR}/_ext/1472/cbus_common.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/_ext/1472/cbus_common.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/_ext/1472/cbus_common.o.d
else
	cat ${OBJECTDIR}/_ext/1472/cbus_common.o.temp >> ${OBJECTDIR}/_ext/1472/cbus_common.o.d
endif
	${RM} __temp_cpp_output__
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/cancmd.X.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) ../cancmd.lkr  -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG -l"/opt/microchip/mplabc18/v3.40/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/cancmd.X.${IMAGE_TYPE}.cof ${OBJECTFILES}      
else
dist/${CND_CONF}/${IMAGE_TYPE}/cancmd.X.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) ../cancmd.lkr  -p$(MP_PROCESSOR_OPTION_LD)  -w  -l"/opt/microchip/mplabc18/v3.40/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/cancmd.X.${IMAGE_TYPE}.cof ${OBJECTFILES}      
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
