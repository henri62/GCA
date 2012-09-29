#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cangc8.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cangc8.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/canbus.o ${OBJECTDIR}/cangc8.o ${OBJECTDIR}/commands.o ${OBJECTDIR}/display.o ${OBJECTDIR}/io.o ${OBJECTDIR}/isr.o ${OBJECTDIR}/utils.o ${OBJECTDIR}/c018i.o ${OBJECTDIR}/Boot3.o
POSSIBLE_DEPFILES=${OBJECTDIR}/canbus.o.d ${OBJECTDIR}/cangc8.o.d ${OBJECTDIR}/commands.o.d ${OBJECTDIR}/display.o.d ${OBJECTDIR}/io.o.d ${OBJECTDIR}/isr.o.d ${OBJECTDIR}/utils.o.d ${OBJECTDIR}/c018i.o.d ${OBJECTDIR}/Boot3.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/canbus.o ${OBJECTDIR}/cangc8.o ${OBJECTDIR}/commands.o ${OBJECTDIR}/display.o ${OBJECTDIR}/io.o ${OBJECTDIR}/isr.o ${OBJECTDIR}/utils.o ${OBJECTDIR}/c018i.o ${OBJECTDIR}/Boot3.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/cangc8.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

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
	@${FIXDEPS} "${OBJECTDIR}/Boot3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/Boot3.o: Boot3.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Boot3.o.d 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/Boot3.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l\"${OBJECTDIR}/Boot3.lst\" -e\"${OBJECTDIR}/Boot3.err\" $(ASM_OPTIONS)  -o\"${OBJECTDIR}/Boot3.o\" Boot3.asm 
	@${DEP_GEN} -d ${OBJECTDIR}/Boot3.o 
	@${FIXDEPS} "${OBJECTDIR}/Boot3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/canbus.o: canbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/canbus.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/canbus.o   canbus.c 
	@${DEP_GEN} -d ${OBJECTDIR}/canbus.o 
	@${FIXDEPS} "${OBJECTDIR}/canbus.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/cangc8.o: cangc8.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/cangc8.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cangc8.o   cangc8.c 
	@${DEP_GEN} -d ${OBJECTDIR}/cangc8.o 
	@${FIXDEPS} "${OBJECTDIR}/cangc8.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/commands.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/commands.o   commands.c 
	@${DEP_GEN} -d ${OBJECTDIR}/commands.o 
	@${FIXDEPS} "${OBJECTDIR}/commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/display.o: display.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/display.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/display.o   display.c 
	@${DEP_GEN} -d ${OBJECTDIR}/display.o 
	@${FIXDEPS} "${OBJECTDIR}/display.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/io.o: io.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/io.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/io.o   io.c 
	@${DEP_GEN} -d ${OBJECTDIR}/io.o 
	@${FIXDEPS} "${OBJECTDIR}/io.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/isr.o: isr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/isr.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr.o   isr.c 
	@${DEP_GEN} -d ${OBJECTDIR}/isr.o 
	@${FIXDEPS} "${OBJECTDIR}/isr.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/utils.o: utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/utils.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/utils.o   utils.c 
	@${DEP_GEN} -d ${OBJECTDIR}/utils.o 
	@${FIXDEPS} "${OBJECTDIR}/utils.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/c018i.o: c018i.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/c018i.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/c018i.o   c018i.c 
	@${DEP_GEN} -d ${OBJECTDIR}/c018i.o 
	@${FIXDEPS} "${OBJECTDIR}/c018i.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/canbus.o: canbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/canbus.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/canbus.o   canbus.c 
	@${DEP_GEN} -d ${OBJECTDIR}/canbus.o 
	@${FIXDEPS} "${OBJECTDIR}/canbus.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/cangc8.o: cangc8.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/cangc8.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cangc8.o   cangc8.c 
	@${DEP_GEN} -d ${OBJECTDIR}/cangc8.o 
	@${FIXDEPS} "${OBJECTDIR}/cangc8.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/commands.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/commands.o   commands.c 
	@${DEP_GEN} -d ${OBJECTDIR}/commands.o 
	@${FIXDEPS} "${OBJECTDIR}/commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/display.o: display.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/display.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/display.o   display.c 
	@${DEP_GEN} -d ${OBJECTDIR}/display.o 
	@${FIXDEPS} "${OBJECTDIR}/display.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/io.o: io.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/io.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/io.o   io.c 
	@${DEP_GEN} -d ${OBJECTDIR}/io.o 
	@${FIXDEPS} "${OBJECTDIR}/io.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/isr.o: isr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/isr.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr.o   isr.c 
	@${DEP_GEN} -d ${OBJECTDIR}/isr.o 
	@${FIXDEPS} "${OBJECTDIR}/isr.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/utils.o: utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/utils.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/utils.o   utils.c 
	@${DEP_GEN} -d ${OBJECTDIR}/utils.o 
	@${FIXDEPS} "${OBJECTDIR}/utils.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/c018i.o: c018i.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/c018i.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/c018i.o   c018i.c 
	@${DEP_GEN} -d ${OBJECTDIR}/c018i.o 
	@${FIXDEPS} "${OBJECTDIR}/c018i.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/cangc8.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "cangc8.lkr"  -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG   -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/cangc8.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/cangc8.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "cangc8.lkr"  -p$(MP_PROCESSOR_OPTION_LD)  -w    -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}/../lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/cangc8.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
