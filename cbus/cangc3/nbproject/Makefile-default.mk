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
# Adding MPLAB X bin directory to path
PATH:=/Applications/microchip/mplabx/mplab_ide.app/Contents/Resources/mplab_ide/mplab_ide/modules/../../bin/:$(PATH)
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
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

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/Boot3.o ${OBJECTDIR}/c018.o ${OBJECTDIR}/can_send.o ${OBJECTDIR}/cancmd.o ${OBJECTDIR}/cbus_common.o ${OBJECTDIR}/commands.o ${OBJECTDIR}/isr_high.o ${OBJECTDIR}/isr_low.o ${OBJECTDIR}/mode_cmds.o ${OBJECTDIR}/packet_gen.o ${OBJECTDIR}/power_cmds.o ${OBJECTDIR}/program_cmds.o

# Object Files
OBJECTFILES=${OBJECTDIR}/Boot3.o ${OBJECTDIR}/c018.o ${OBJECTDIR}/can_send.o ${OBJECTDIR}/cancmd.o ${OBJECTDIR}/cbus_common.o ${OBJECTDIR}/commands.o ${OBJECTDIR}/isr_high.o ${OBJECTDIR}/isr_low.o ${OBJECTDIR}/mode_cmds.o ${OBJECTDIR}/packet_gen.o ${OBJECTDIR}/power_cmds.o ${OBJECTDIR}/program_cmds.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH="/System/Library/Java/JavaVirtualMachines/1.6.0.jdk/Contents/Home/bin/"
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC="/Applications/microchip/mplabc18/v3.40/bin/mcc18"
# MP_BC is not defined
MP_AS="/Applications/microchip/mplabc18/v3.40/bin/../mpasm/MPASMWIN"
MP_LD="/Applications/microchip/mplabc18/v3.40/bin/mplink"
MP_AR="/Applications/microchip/mplabc18/v3.40/bin/mplib"
DEP_GEN=${MP_JAVA_PATH}java -jar "/Applications/microchip/mplabx/mplab_ide.app/Contents/Resources/mplab_ide/mplab_ide/modules/../../bin/extractobjectdependencies.jar" 
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps
MP_CC_DIR="/Applications/microchip/mplabc18/v3.40/bin"
# MP_BC_DIR is not defined
MP_AS_DIR="/Applications/microchip/mplabc18/v3.40/bin/../mpasm"
MP_LD_DIR="/Applications/microchip/mplabc18/v3.40/bin"
MP_AR_DIR="/Applications/microchip/mplabc18/v3.40/bin"
# MP_BC_DIR is not defined

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof

MP_PROCESSOR_OPTION=18F2480
MP_PROCESSOR_OPTION_LD=18f2480
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0x3dc0 -u_DEBUGCODELEN=0x240 -u_DEBUGDATASTART=0x2f4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Boot3.o: Boot3.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Boot3.o.d 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/Boot3.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PK3=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l\"${OBJECTDIR}/Boot3.lst\" -e\"${OBJECTDIR}/Boot3.err\" $(ASM_OPTIONS)  -o\"${OBJECTDIR}/Boot3.o\" Boot3.asm 
	@${DEP_GEN} -d ${OBJECTDIR}/Boot3.o 
	
else
${OBJECTDIR}/Boot3.o: Boot3.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Boot3.o.d 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/Boot3.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l\"${OBJECTDIR}/Boot3.lst\" -e\"${OBJECTDIR}/Boot3.err\" $(ASM_OPTIONS)  -o\"${OBJECTDIR}/Boot3.o\" Boot3.asm 
	@${DEP_GEN} -d ${OBJECTDIR}/Boot3.o 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/mode_cmds.o: mode_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/mode_cmds.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/mode_cmds.o   mode_cmds.c 
	@${DEP_GEN} -d ${OBJECTDIR}/mode_cmds.o 
	
${OBJECTDIR}/cancmd.o: cancmd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/cancmd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cancmd.o   cancmd.c 
	@${DEP_GEN} -d ${OBJECTDIR}/cancmd.o 
	
${OBJECTDIR}/program_cmds.o: program_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/program_cmds.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/program_cmds.o   program_cmds.c 
	@${DEP_GEN} -d ${OBJECTDIR}/program_cmds.o 
	
${OBJECTDIR}/isr_high.o: isr_high.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/isr_high.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr_high.o   isr_high.c 
	@${DEP_GEN} -d ${OBJECTDIR}/isr_high.o 
	
${OBJECTDIR}/packet_gen.o: packet_gen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/packet_gen.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/packet_gen.o   packet_gen.c 
	@${DEP_GEN} -d ${OBJECTDIR}/packet_gen.o 
	
${OBJECTDIR}/isr_low.o: isr_low.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/isr_low.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr_low.o   isr_low.c 
	@${DEP_GEN} -d ${OBJECTDIR}/isr_low.o 
	
${OBJECTDIR}/cbus_common.o: cbus_common.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/cbus_common.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cbus_common.o   cbus_common.c 
	@${DEP_GEN} -d ${OBJECTDIR}/cbus_common.o 
	
${OBJECTDIR}/can_send.o: can_send.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/can_send.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/can_send.o   can_send.c 
	@${DEP_GEN} -d ${OBJECTDIR}/can_send.o 
	
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/commands.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/commands.o   commands.c 
	@${DEP_GEN} -d ${OBJECTDIR}/commands.o 
	
${OBJECTDIR}/power_cmds.o: power_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/power_cmds.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/power_cmds.o   power_cmds.c 
	@${DEP_GEN} -d ${OBJECTDIR}/power_cmds.o 
	
${OBJECTDIR}/c018.o: c018.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/c018.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/c018.o   c018.c 
	@${DEP_GEN} -d ${OBJECTDIR}/c018.o 
	
else
${OBJECTDIR}/mode_cmds.o: mode_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/mode_cmds.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/mode_cmds.o   mode_cmds.c 
	@${DEP_GEN} -d ${OBJECTDIR}/mode_cmds.o 
	
${OBJECTDIR}/cancmd.o: cancmd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/cancmd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cancmd.o   cancmd.c 
	@${DEP_GEN} -d ${OBJECTDIR}/cancmd.o 
	
${OBJECTDIR}/program_cmds.o: program_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/program_cmds.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/program_cmds.o   program_cmds.c 
	@${DEP_GEN} -d ${OBJECTDIR}/program_cmds.o 
	
${OBJECTDIR}/isr_high.o: isr_high.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/isr_high.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr_high.o   isr_high.c 
	@${DEP_GEN} -d ${OBJECTDIR}/isr_high.o 
	
${OBJECTDIR}/packet_gen.o: packet_gen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/packet_gen.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/packet_gen.o   packet_gen.c 
	@${DEP_GEN} -d ${OBJECTDIR}/packet_gen.o 
	
${OBJECTDIR}/isr_low.o: isr_low.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/isr_low.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr_low.o   isr_low.c 
	@${DEP_GEN} -d ${OBJECTDIR}/isr_low.o 
	
${OBJECTDIR}/cbus_common.o: cbus_common.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/cbus_common.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cbus_common.o   cbus_common.c 
	@${DEP_GEN} -d ${OBJECTDIR}/cbus_common.o 
	
${OBJECTDIR}/can_send.o: can_send.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/can_send.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/can_send.o   can_send.c 
	@${DEP_GEN} -d ${OBJECTDIR}/can_send.o 
	
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/commands.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/commands.o   commands.c 
	@${DEP_GEN} -d ${OBJECTDIR}/commands.o 
	
${OBJECTDIR}/power_cmds.o: power_cmds.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/power_cmds.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/power_cmds.o   power_cmds.c 
	@${DEP_GEN} -d ${OBJECTDIR}/power_cmds.o 
	
${OBJECTDIR}/c018.o: c018.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/c018.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -I"." -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/c018.o   c018.c 
	@${DEP_GEN} -d ${OBJECTDIR}/c018.o 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) cancmd.lkr  -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG -l"C:/Program Files/Microchip/mplabc18/v3.37/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) cancmd.lkr  -p$(MP_PROCESSOR_OPTION_LD)  -w  -l"C:/Program Files/Microchip/mplabc18/v3.37/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/cangc3.${IMAGE_TYPE}.cof  ${OBJECTFILES_QUOTED_IF_SPACED}   
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard $(addsuffix .d, ${OBJECTFILES}))
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
