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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files
OBJECTFILES=${OBJECTDIR}/Boot3.o ${OBJECTDIR}/c018.o ${OBJECTDIR}/cangc2.o ${OBJECTDIR}/cbus.o ${OBJECTDIR}/commands.o ${OBJECTDIR}/io.o ${OBJECTDIR}/isr.o ${OBJECTDIR}/utils.o


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof

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
${OBJECTDIR}/cangc2.o: cangc2.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cangc2.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cangc2.o   cangc2.c  > ${OBJECTDIR}/cangc2.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cangc2.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cangc2.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cangc2.o.temp cangc2.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cangc2.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cangc2.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cangc2.o.d
else
	cat ${OBJECTDIR}/cangc2.o.temp >> ${OBJECTDIR}/cangc2.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/isr.o: isr.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/isr.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr.o   isr.c  > ${OBJECTDIR}/isr.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/isr.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/isr.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/isr.o.temp isr.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/isr.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/isr.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/isr.o.d
else
	cat ${OBJECTDIR}/isr.o.temp >> ${OBJECTDIR}/isr.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/io.o: io.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/io.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/io.o   io.c  > ${OBJECTDIR}/io.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/io.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/io.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/io.o.temp io.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/io.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/io.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/io.o.d
else
	cat ${OBJECTDIR}/io.o.temp >> ${OBJECTDIR}/io.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/cbus.o: cbus.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cbus.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cbus.o   cbus.c  > ${OBJECTDIR}/cbus.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cbus.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cbus.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cbus.o.temp cbus.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cbus.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cbus.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cbus.o.d
else
	cat ${OBJECTDIR}/cbus.o.temp >> ${OBJECTDIR}/cbus.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/utils.o: utils.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/utils.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/utils.o   utils.c  > ${OBJECTDIR}/utils.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/utils.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/utils.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/utils.o.temp utils.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/utils.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/utils.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/utils.o.d
else
	cat ${OBJECTDIR}/utils.o.temp >> ${OBJECTDIR}/utils.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/commands.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/commands.o   commands.c  > ${OBJECTDIR}/commands.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/commands.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/commands.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/commands.o.temp commands.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/commands.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/commands.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/commands.o.d
else
	cat ${OBJECTDIR}/commands.o.temp >> ${OBJECTDIR}/commands.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/c018.o: c018.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/c018.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/c018.o   c018.c  > ${OBJECTDIR}/c018.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/c018.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/c018.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/c018.o.temp c018.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/c018.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/c018.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/c018.o.d
else
	cat ${OBJECTDIR}/c018.o.temp >> ${OBJECTDIR}/c018.o.d
endif
	${RM} __temp_cpp_output__
else
${OBJECTDIR}/cangc2.o: cangc2.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cangc2.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cangc2.o   cangc2.c  > ${OBJECTDIR}/cangc2.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cangc2.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cangc2.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cangc2.o.temp cangc2.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cangc2.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cangc2.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cangc2.o.d
else
	cat ${OBJECTDIR}/cangc2.o.temp >> ${OBJECTDIR}/cangc2.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/isr.o: isr.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/isr.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr.o   isr.c  > ${OBJECTDIR}/isr.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/isr.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/isr.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/isr.o.temp isr.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/isr.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/isr.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/isr.o.d
else
	cat ${OBJECTDIR}/isr.o.temp >> ${OBJECTDIR}/isr.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/io.o: io.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/io.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/io.o   io.c  > ${OBJECTDIR}/io.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/io.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/io.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/io.o.temp io.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/io.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/io.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/io.o.d
else
	cat ${OBJECTDIR}/io.o.temp >> ${OBJECTDIR}/io.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/cbus.o: cbus.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cbus.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cbus.o   cbus.c  > ${OBJECTDIR}/cbus.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cbus.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cbus.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cbus.o.temp cbus.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cbus.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cbus.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cbus.o.d
else
	cat ${OBJECTDIR}/cbus.o.temp >> ${OBJECTDIR}/cbus.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/utils.o: utils.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/utils.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/utils.o   utils.c  > ${OBJECTDIR}/utils.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/utils.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/utils.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/utils.o.temp utils.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/utils.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/utils.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/utils.o.d
else
	cat ${OBJECTDIR}/utils.o.temp >> ${OBJECTDIR}/utils.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/commands.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/commands.o   commands.c  > ${OBJECTDIR}/commands.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/commands.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/commands.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/commands.o.temp commands.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/commands.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/commands.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/commands.o.d
else
	cat ${OBJECTDIR}/commands.o.temp >> ${OBJECTDIR}/commands.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/c018.o: c018.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/c018.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -Ou- -Ot- -Ob- -Ow- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/c018.o   c018.c  > ${OBJECTDIR}/c018.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/c018.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/c018.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/c018.o.temp c018.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
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
dist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) cangc2.lkr  -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG   -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof ${OBJECTFILES}      
else
dist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) cangc2.lkr  -p$(MP_PROCESSOR_OPTION_LD)  -w    -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof ${OBJECTFILES}      
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
