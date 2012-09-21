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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cangc1e.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cangc1e.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/cangc1e.o ${OBJECTDIR}/commands.o ${OBJECTDIR}/io.o ${OBJECTDIR}/isr.o ${OBJECTDIR}/utils.o "${OBJECTDIR}/TCPIP Stack/ARP.o" "${OBJECTDIR}/TCPIP Stack/ENC28J60.o" "${OBJECTDIR}/TCPIP Stack/ICMP.o" "${OBJECTDIR}/TCPIP Stack/TCP.o" "${OBJECTDIR}/TCPIP Stack/Tick.o" "${OBJECTDIR}/TCPIP Stack/StackTsk.o" "${OBJECTDIR}/TCPIP Stack/Helpers.o" "${OBJECTDIR}/TCPIP Stack/IP.o" ${OBJECTDIR}/canbus.o ${OBJECTDIR}/gcaeth.o "${OBJECTDIR}/TCPIP Stack/UDP.o" ${OBJECTDIR}/eth.o "${OBJECTDIR}/TCPIP Stack/DHCP.o"
POSSIBLE_DEPFILES=${OBJECTDIR}/cangc1e.o.d ${OBJECTDIR}/commands.o.d ${OBJECTDIR}/io.o.d ${OBJECTDIR}/isr.o.d ${OBJECTDIR}/utils.o.d "${OBJECTDIR}/TCPIP Stack/ARP.o.d" "${OBJECTDIR}/TCPIP Stack/ENC28J60.o.d" "${OBJECTDIR}/TCPIP Stack/ICMP.o.d" "${OBJECTDIR}/TCPIP Stack/TCP.o.d" "${OBJECTDIR}/TCPIP Stack/Tick.o.d" "${OBJECTDIR}/TCPIP Stack/StackTsk.o.d" "${OBJECTDIR}/TCPIP Stack/Helpers.o.d" "${OBJECTDIR}/TCPIP Stack/IP.o.d" ${OBJECTDIR}/canbus.o.d ${OBJECTDIR}/gcaeth.o.d "${OBJECTDIR}/TCPIP Stack/UDP.o.d" ${OBJECTDIR}/eth.o.d "${OBJECTDIR}/TCPIP Stack/DHCP.o.d"

# Object Files
OBJECTFILES=${OBJECTDIR}/cangc1e.o ${OBJECTDIR}/commands.o ${OBJECTDIR}/io.o ${OBJECTDIR}/isr.o ${OBJECTDIR}/utils.o ${OBJECTDIR}/TCPIP\ Stack/ARP.o ${OBJECTDIR}/TCPIP\ Stack/ENC28J60.o ${OBJECTDIR}/TCPIP\ Stack/ICMP.o ${OBJECTDIR}/TCPIP\ Stack/TCP.o ${OBJECTDIR}/TCPIP\ Stack/Tick.o ${OBJECTDIR}/TCPIP\ Stack/StackTsk.o ${OBJECTDIR}/TCPIP\ Stack/Helpers.o ${OBJECTDIR}/TCPIP\ Stack/IP.o ${OBJECTDIR}/canbus.o ${OBJECTDIR}/gcaeth.o ${OBJECTDIR}/TCPIP\ Stack/UDP.o ${OBJECTDIR}/eth.o ${OBJECTDIR}/TCPIP\ Stack/DHCP.o


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/cangc1e.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F2585
MP_PROCESSOR_OPTION_LD=18f2585
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0xbd30 -u_DEBUGCODELEN=0x2d0 -u_DEBUGDATASTART=0xcf4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/cangc1e.o: cangc1e.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/cangc1e.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cangc1e.o   cangc1e.c 
	@${DEP_GEN} -d ${OBJECTDIR}/cangc1e.o 
	
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/commands.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/commands.o   commands.c 
	@${DEP_GEN} -d ${OBJECTDIR}/commands.o 
	
${OBJECTDIR}/io.o: io.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/io.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/io.o   io.c 
	@${DEP_GEN} -d ${OBJECTDIR}/io.o 
	
${OBJECTDIR}/isr.o: isr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/isr.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr.o   isr.c 
	@${DEP_GEN} -d ${OBJECTDIR}/isr.o 
	
${OBJECTDIR}/utils.o: utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/utils.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/utils.o   utils.c 
	@${DEP_GEN} -d ${OBJECTDIR}/utils.o 
	
${OBJECTDIR}/TCPIP\ Stack/ARP.o: TCPIP\ Stack/ARP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/ARP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/ARP.o"   "TCPIP Stack/ARP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/ARP.o" 
	
${OBJECTDIR}/TCPIP\ Stack/ENC28J60.o: TCPIP\ Stack/ENC28J60.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/ENC28J60.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/ENC28J60.o"   "TCPIP Stack/ENC28J60.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/ENC28J60.o" 
	
${OBJECTDIR}/TCPIP\ Stack/ICMP.o: TCPIP\ Stack/ICMP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/ICMP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/ICMP.o"   "TCPIP Stack/ICMP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/ICMP.o" 
	
${OBJECTDIR}/TCPIP\ Stack/TCP.o: TCPIP\ Stack/TCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/TCP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/TCP.o"   "TCPIP Stack/TCP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/TCP.o" 
	
${OBJECTDIR}/TCPIP\ Stack/Tick.o: TCPIP\ Stack/Tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/Tick.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/Tick.o"   "TCPIP Stack/Tick.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/Tick.o" 
	
${OBJECTDIR}/TCPIP\ Stack/StackTsk.o: TCPIP\ Stack/StackTsk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/StackTsk.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/StackTsk.o"   "TCPIP Stack/StackTsk.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/StackTsk.o" 
	
${OBJECTDIR}/TCPIP\ Stack/Helpers.o: TCPIP\ Stack/Helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/Helpers.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/Helpers.o"   "TCPIP Stack/Helpers.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/Helpers.o" 
	
${OBJECTDIR}/TCPIP\ Stack/IP.o: TCPIP\ Stack/IP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/IP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/IP.o"   "TCPIP Stack/IP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/IP.o" 
	
${OBJECTDIR}/canbus.o: canbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/canbus.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/canbus.o   canbus.c 
	@${DEP_GEN} -d ${OBJECTDIR}/canbus.o 
	
${OBJECTDIR}/gcaeth.o: gcaeth.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/gcaeth.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/gcaeth.o   gcaeth.c 
	@${DEP_GEN} -d ${OBJECTDIR}/gcaeth.o 
	
${OBJECTDIR}/TCPIP\ Stack/UDP.o: TCPIP\ Stack/UDP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/UDP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/UDP.o"   "TCPIP Stack/UDP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/UDP.o" 
	
${OBJECTDIR}/eth.o: eth.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/eth.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/eth.o   eth.c 
	@${DEP_GEN} -d ${OBJECTDIR}/eth.o 
	
${OBJECTDIR}/TCPIP\ Stack/DHCP.o: TCPIP\ Stack/DHCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/DHCP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/DHCP.o"   "TCPIP Stack/DHCP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/DHCP.o" 
	
else
${OBJECTDIR}/cangc1e.o: cangc1e.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/cangc1e.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cangc1e.o   cangc1e.c 
	@${DEP_GEN} -d ${OBJECTDIR}/cangc1e.o 
	
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/commands.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/commands.o   commands.c 
	@${DEP_GEN} -d ${OBJECTDIR}/commands.o 
	
${OBJECTDIR}/io.o: io.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/io.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/io.o   io.c 
	@${DEP_GEN} -d ${OBJECTDIR}/io.o 
	
${OBJECTDIR}/isr.o: isr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/isr.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/isr.o   isr.c 
	@${DEP_GEN} -d ${OBJECTDIR}/isr.o 
	
${OBJECTDIR}/utils.o: utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/utils.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/utils.o   utils.c 
	@${DEP_GEN} -d ${OBJECTDIR}/utils.o 
	
${OBJECTDIR}/TCPIP\ Stack/ARP.o: TCPIP\ Stack/ARP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/ARP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/ARP.o"   "TCPIP Stack/ARP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/ARP.o" 
	
${OBJECTDIR}/TCPIP\ Stack/ENC28J60.o: TCPIP\ Stack/ENC28J60.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/ENC28J60.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/ENC28J60.o"   "TCPIP Stack/ENC28J60.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/ENC28J60.o" 
	
${OBJECTDIR}/TCPIP\ Stack/ICMP.o: TCPIP\ Stack/ICMP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/ICMP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/ICMP.o"   "TCPIP Stack/ICMP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/ICMP.o" 
	
${OBJECTDIR}/TCPIP\ Stack/TCP.o: TCPIP\ Stack/TCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/TCP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/TCP.o"   "TCPIP Stack/TCP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/TCP.o" 
	
${OBJECTDIR}/TCPIP\ Stack/Tick.o: TCPIP\ Stack/Tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/Tick.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/Tick.o"   "TCPIP Stack/Tick.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/Tick.o" 
	
${OBJECTDIR}/TCPIP\ Stack/StackTsk.o: TCPIP\ Stack/StackTsk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/StackTsk.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/StackTsk.o"   "TCPIP Stack/StackTsk.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/StackTsk.o" 
	
${OBJECTDIR}/TCPIP\ Stack/Helpers.o: TCPIP\ Stack/Helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/Helpers.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/Helpers.o"   "TCPIP Stack/Helpers.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/Helpers.o" 
	
${OBJECTDIR}/TCPIP\ Stack/IP.o: TCPIP\ Stack/IP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/IP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/IP.o"   "TCPIP Stack/IP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/IP.o" 
	
${OBJECTDIR}/canbus.o: canbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/canbus.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/canbus.o   canbus.c 
	@${DEP_GEN} -d ${OBJECTDIR}/canbus.o 
	
${OBJECTDIR}/gcaeth.o: gcaeth.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/gcaeth.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/gcaeth.o   gcaeth.c 
	@${DEP_GEN} -d ${OBJECTDIR}/gcaeth.o 
	
${OBJECTDIR}/TCPIP\ Stack/UDP.o: TCPIP\ Stack/UDP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/UDP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/UDP.o"   "TCPIP Stack/UDP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/UDP.o" 
	
${OBJECTDIR}/eth.o: eth.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/eth.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/eth.o   eth.c 
	@${DEP_GEN} -d ${OBJECTDIR}/eth.o 
	
${OBJECTDIR}/TCPIP\ Stack/DHCP.o: TCPIP\ Stack/DHCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/TCPIP\ Stack 
	@${RM} ${OBJECTDIR}/TCPIP\ Stack/DHCP.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"./" -I"./include" -pa=9  -I ${MP_CC_DIR}/../h  -fo "${OBJECTDIR}/TCPIP Stack/DHCP.o"   "TCPIP Stack/DHCP.c" 
	@${DEP_GEN} -d "${OBJECTDIR}/TCPIP Stack/DHCP.o" 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/cangc1e.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG   -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/cangc1e.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/cangc1e.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w    -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}/../lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/cangc1e.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
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
