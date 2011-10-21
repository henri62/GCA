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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files
OBJECTFILES=${OBJECTDIR}/Boot3.o ${OBJECTDIR}/c018.o ${OBJECTDIR}/can_send.o ${OBJECTDIR}/cancmd.o ${OBJECTDIR}/cbus_common.o ${OBJECTDIR}/commands.o ${OBJECTDIR}/isr_high.o ${OBJECTDIR}/isr_low.o ${OBJECTDIR}/mode_cmds.o ${OBJECTDIR}/packet_gen.o ${OBJECTDIR}/power_cmds.o ${OBJECTDIR}/program_cmds.o


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
MP_CPP=/opt/microchip/mplabx702/mplab_ide/mplab_ide/modules/../../bin/mplab-cpp

.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof

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
.PHONY: ${OBJECTDIR}/Boot3.o
${OBJECTDIR}/Boot3.o: Boot3.asm __revgrep__ nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR} 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PK3=1 -q -p$(MP_PROCESSOR_OPTION)  -l"${OBJECTDIR}/Boot3.lst" -e"${OBJECTDIR}/Boot3.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/Boot3.o" Boot3.asm 
else 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PK3=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l"${OBJECTDIR}/Boot3.lst" -e"${OBJECTDIR}/Boot3.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/Boot3.o" Boot3.asm 
endif 
	@cat  "${OBJECTDIR}/Boot3.err" | sed -e 's/\x0D$$//' -e 's/\(^Warning\|^Error\|^Message\)\(\[[0-9]*\]\) *\(.*\) \([0-9]*\) : \(.*$$\)/\3:\4: \1\2: \5/g'
	@./__revgrep__ "^Error" ${OBJECTDIR}/Boot3.err
else
.PHONY: ${OBJECTDIR}/Boot3.o
${OBJECTDIR}/Boot3.o: Boot3.asm __revgrep__ nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR} 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION)  -l"${OBJECTDIR}/Boot3.lst" -e"${OBJECTDIR}/Boot3.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/Boot3.o" Boot3.asm 
else 
	-${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l"${OBJECTDIR}/Boot3.lst" -e"${OBJECTDIR}/Boot3.err" $(ASM_OPTIONS)  -o"${OBJECTDIR}/Boot3.o" Boot3.asm 
endif 
	@cat  "${OBJECTDIR}/Boot3.err" | sed -e 's/\x0D$$//' -e 's/\(^Warning\|^Error\|^Message\)\(\[[0-9]*\]\) *\(.*\) \([0-9]*\) : \(.*$$\)/\3:\4: \1\2: \5/g'
	@./__revgrep__ "^Error" ${OBJECTDIR}/Boot3.err
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/mode_cmds.o: mode_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/mode_cmds.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/mode_cmds.o   mode_cmds.c  > ${OBJECTDIR}/mode_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/mode_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/mode_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/mode_cmds.o.temp mode_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/mode_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/mode_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/mode_cmds.o.d
else
	cat ${OBJECTDIR}/mode_cmds.o.temp >> ${OBJECTDIR}/mode_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/cancmd.o: cancmd.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cancmd.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cancmd.o   cancmd.c  > ${OBJECTDIR}/cancmd.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cancmd.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cancmd.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cancmd.o.temp cancmd.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cancmd.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cancmd.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cancmd.o.d
else
	cat ${OBJECTDIR}/cancmd.o.temp >> ${OBJECTDIR}/cancmd.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/program_cmds.o: program_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/program_cmds.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/program_cmds.o   program_cmds.c  > ${OBJECTDIR}/program_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/program_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/program_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/program_cmds.o.temp program_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/program_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/program_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/program_cmds.o.d
else
	cat ${OBJECTDIR}/program_cmds.o.temp >> ${OBJECTDIR}/program_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/isr_high.o: isr_high.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/isr_high.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr_high.o   isr_high.c  > ${OBJECTDIR}/isr_high.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/isr_high.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/isr_high.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/isr_high.o.temp isr_high.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/isr_high.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/isr_high.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/isr_high.o.d
else
	cat ${OBJECTDIR}/isr_high.o.temp >> ${OBJECTDIR}/isr_high.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/packet_gen.o: packet_gen.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/packet_gen.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/packet_gen.o   packet_gen.c  > ${OBJECTDIR}/packet_gen.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/packet_gen.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/packet_gen.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/packet_gen.o.temp packet_gen.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/packet_gen.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/packet_gen.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/packet_gen.o.d
else
	cat ${OBJECTDIR}/packet_gen.o.temp >> ${OBJECTDIR}/packet_gen.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/isr_low.o: isr_low.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/isr_low.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr_low.o   isr_low.c  > ${OBJECTDIR}/isr_low.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/isr_low.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/isr_low.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/isr_low.o.temp isr_low.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/isr_low.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/isr_low.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/isr_low.o.d
else
	cat ${OBJECTDIR}/isr_low.o.temp >> ${OBJECTDIR}/isr_low.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/cbus_common.o: cbus_common.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cbus_common.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cbus_common.o   cbus_common.c  > ${OBJECTDIR}/cbus_common.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cbus_common.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cbus_common.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cbus_common.o.temp cbus_common.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cbus_common.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cbus_common.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cbus_common.o.d
else
	cat ${OBJECTDIR}/cbus_common.o.temp >> ${OBJECTDIR}/cbus_common.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/can_send.o: can_send.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/can_send.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/can_send.o   can_send.c  > ${OBJECTDIR}/can_send.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/can_send.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/can_send.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/can_send.o.temp can_send.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/can_send.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/can_send.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/can_send.o.d
else
	cat ${OBJECTDIR}/can_send.o.temp >> ${OBJECTDIR}/can_send.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/commands.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/commands.o   commands.c  > ${OBJECTDIR}/commands.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/commands.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/commands.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/commands.o.temp commands.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/commands.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/commands.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/commands.o.d
else
	cat ${OBJECTDIR}/commands.o.temp >> ${OBJECTDIR}/commands.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/power_cmds.o: power_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/power_cmds.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/power_cmds.o   power_cmds.c  > ${OBJECTDIR}/power_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/power_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/power_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/power_cmds.o.temp power_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/power_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/power_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/power_cmds.o.d
else
	cat ${OBJECTDIR}/power_cmds.o.temp >> ${OBJECTDIR}/power_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/c018.o: c018.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/c018.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/c018.o   c018.c  > ${OBJECTDIR}/c018.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/c018.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/c018.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/c018.o.temp c018.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/c018.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/c018.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/c018.o.d
else
	cat ${OBJECTDIR}/c018.o.temp >> ${OBJECTDIR}/c018.o.d
endif
	${RM} __temp_cpp_output__
else
${OBJECTDIR}/mode_cmds.o: mode_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/mode_cmds.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/mode_cmds.o   mode_cmds.c  > ${OBJECTDIR}/mode_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/mode_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/mode_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/mode_cmds.o.temp mode_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/mode_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/mode_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/mode_cmds.o.d
else
	cat ${OBJECTDIR}/mode_cmds.o.temp >> ${OBJECTDIR}/mode_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/cancmd.o: cancmd.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cancmd.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cancmd.o   cancmd.c  > ${OBJECTDIR}/cancmd.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cancmd.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cancmd.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cancmd.o.temp cancmd.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cancmd.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cancmd.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cancmd.o.d
else
	cat ${OBJECTDIR}/cancmd.o.temp >> ${OBJECTDIR}/cancmd.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/program_cmds.o: program_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/program_cmds.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/program_cmds.o   program_cmds.c  > ${OBJECTDIR}/program_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/program_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/program_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/program_cmds.o.temp program_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/program_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/program_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/program_cmds.o.d
else
	cat ${OBJECTDIR}/program_cmds.o.temp >> ${OBJECTDIR}/program_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/isr_high.o: isr_high.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/isr_high.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr_high.o   isr_high.c  > ${OBJECTDIR}/isr_high.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/isr_high.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/isr_high.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/isr_high.o.temp isr_high.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/isr_high.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/isr_high.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/isr_high.o.d
else
	cat ${OBJECTDIR}/isr_high.o.temp >> ${OBJECTDIR}/isr_high.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/packet_gen.o: packet_gen.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/packet_gen.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/packet_gen.o   packet_gen.c  > ${OBJECTDIR}/packet_gen.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/packet_gen.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/packet_gen.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/packet_gen.o.temp packet_gen.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/packet_gen.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/packet_gen.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/packet_gen.o.d
else
	cat ${OBJECTDIR}/packet_gen.o.temp >> ${OBJECTDIR}/packet_gen.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/isr_low.o: isr_low.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/isr_low.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr_low.o   isr_low.c  > ${OBJECTDIR}/isr_low.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/isr_low.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/isr_low.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/isr_low.o.temp isr_low.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/isr_low.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/isr_low.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/isr_low.o.d
else
	cat ${OBJECTDIR}/isr_low.o.temp >> ${OBJECTDIR}/isr_low.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/cbus_common.o: cbus_common.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cbus_common.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cbus_common.o   cbus_common.c  > ${OBJECTDIR}/cbus_common.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cbus_common.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cbus_common.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cbus_common.o.temp cbus_common.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cbus_common.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cbus_common.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cbus_common.o.d
else
	cat ${OBJECTDIR}/cbus_common.o.temp >> ${OBJECTDIR}/cbus_common.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/can_send.o: can_send.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/can_send.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/can_send.o   can_send.c  > ${OBJECTDIR}/can_send.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/can_send.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/can_send.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/can_send.o.temp can_send.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/can_send.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/can_send.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/can_send.o.d
else
	cat ${OBJECTDIR}/can_send.o.temp >> ${OBJECTDIR}/can_send.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/commands.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/commands.o   commands.c  > ${OBJECTDIR}/commands.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/commands.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/commands.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/commands.o.temp commands.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/commands.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/commands.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/commands.o.d
else
	cat ${OBJECTDIR}/commands.o.temp >> ${OBJECTDIR}/commands.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/power_cmds.o: power_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/power_cmds.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/power_cmds.o   power_cmds.c  > ${OBJECTDIR}/power_cmds.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/power_cmds.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/power_cmds.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/power_cmds.o.temp power_cmds.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/power_cmds.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/power_cmds.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/power_cmds.o.d
else
	cat ${OBJECTDIR}/power_cmds.o.temp >> ${OBJECTDIR}/power_cmds.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/c018.o: c018.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/c018.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/c018.o   c018.c  > ${OBJECTDIR}/c018.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/c018.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/c018.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/c018.o.temp c018.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /home/rob/lp/Hardware/cbus/cangc3 -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/c018.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/c018.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/c018.o.d
else
	cat ${OBJECTDIR}/c018.o.temp >> ${OBJECTDIR}/c018.o.d
endif
	${RM} __temp_cpp_output__
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) cancmd.lkr  -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG -l"C:/Program Files/Microchip/mplabc18/v3.37/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof ${OBJECTFILES}      
else
dist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) cancmd.lkr  -p$(MP_PROCESSOR_OPTION_LD)  -w  -l"C:/Program Files/Microchip/mplabc18/v3.37/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof ${OBJECTFILES}      
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
