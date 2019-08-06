/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : FTM0_Config.h
**     Project     : zies
**     Processor   : MK22FN512VLH12
**     Component   : Init_FTM
**     Version     : Component 01.010, Driver 01.10, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-06-20, 16:05, # CodeGen: 1
**     Abstract    :
**          This file implements the FTM (FTM0) module initialization
**          according to the Peripheral Initialization settings, and
**          defines interrupt service routines prototypes.
**     Settings    :
**          Component name                                 : M1_FTM0_PWM1
**          Device                                         : FTM0
**          Settings                                       : 
**            Clock gate                                   : Enabled
**            Clock settings                               : 
**              Clock source                               : No clock
**              Prescaler                                  : divide by 1
**              Counter frequency                          : STOP
**              Counter init                               : 0
**              Modulo counter                             : 0xFFFF
**              Period                                     : Counter stopped
**              Deadtime                                   : 
**                Deadtime prescaler                       : divide by 1
**                Deadtime value                           : 0
**                Deadtime delay                           : DISABLED
**              Num of timer overflows                     : 0
**            FTM mode                                     : Enabled
**            BDM mode                                     : FTM counter stopped; channels outputs functional
**            Global time base                             : Disabled
**            Global time base output                      : Disabled
**            Write protection                             : Disabled
**            Combined channels                            : 
**              Channels 0 and 1                           : 
**                Complementary mode                       : Disabled
**                Fault control                            : Disabled
**                Synchronization                          : Disabled
**                Deadtime                                 : Disabled
**                Inverting                                : Disabled
**              Channels 2 and 3                           : 
**                Complementary mode                       : Disabled
**                Fault control                            : Disabled
**                Synchronization                          : Disabled
**                Deadtime                                 : Disabled
**                Inverting                                : Disabled
**              Channels 4 and 5                           : 
**                Complementary mode                       : Disabled
**                Fault control                            : Disabled
**                Synchronization                          : Disabled
**                Deadtime                                 : Disabled
**                Inverting                                : Disabled
**              Channels 6 and 7                           : 
**                Complementary mode                       : Disabled
**                Fault control                            : Disabled
**                Synchronization                          : Disabled
**                Deadtime                                 : Disabled
**                Inverting                                : Disabled
**            Synchronization                              : 
**              Synchronization mode                       : Disabled
**                Counter reinitialization                 : Disabled
**              HW trigger mode                            : Enabled
**              CNTIN synchronization                      : Disabled
**              INVCTRL synchronization                    : Disabled
**              SWOCTRL synchronization                    : Disabled
**              Software Trigger                           : Disabled
**              OUTMASK synchronization                    : Disabled
**              Minimum loading point                      : Disabled
**              Maximum loading point                      : Disabled
**            External triggers                            : 
**              Counter init trigger                       : Disabled
**              Channel 0 trigger                          : Disabled
**              Channel 1 trigger                          : Disabled
**              Channel 2 trigger                          : Disabled
**              Channel 3 trigger                          : Disabled
**              Channel 4 trigger                          : Disabled
**              Channel 5 trigger                          : Disabled
**            Fault control settings                       : 
**              Fault control mode                         : Disabled
**              Fault input filter value                   : Disabled
**              Fault input 0 filter                       : Disabled
**              Fault input 0 polarity                     : High
**              Fault input 1 filter                       : Disabled
**              Fault input 1 polarity                     : High
**              Fault input 2 filter                       : Disabled
**              Fault input 2 polarity                     : High
**              Fault input 3 filter                       : Disabled
**              Fault input 3 polarity                     : High
**          Channels                                       : 
**            Channel 0                                    : Disabled
**            Channel 1                                    : Enabled
**              Channel mode                               : Center-aligned PWM
**                PWM polarity                             : High-true
**                Channel value register                   : 0
**              Input capture filter                       : Disabled
**              Software output control                    : Enabled
**                Output control value                     : 1
**              Pin                                        : Enabled
**                Pin                                      : <Automatic>
**              Interrupt/DMA                              : 
**                Interrupt                                : INT_FTM0
**                Channel interrupt                        : Enabled
**                DMA request                              : Disabled
**            Channel 2                                    : Disabled
**            Channel 3                                    : Disabled
**            Channel 4                                    : Disabled
**            Channel 5                                    : Disabled
**            Channel 6                                    : Disabled
**            Channel 7                                    : Disabled
**          Pins                                           : 
**            Fault pins                                   : 
**              Fault pin 0                                : Disabled
**              Fault pin 1                                : Disabled
**              Fault pin 2                                : Disabled
**              Fault pin 3                                : Disabled
**            External clock pin                           : Disabled
**            HW Synchronization triggers                  : 
**              Hardware Trigger 0                         : Disabled
**              Hardware Trigger 1                         : Disabled
**              Hardware Trigger 2                         : Disabled
**          Interrupts                                     : 
**            Channels / Timer overflow / Fault            : 
**              Interrupt                                  : INT_FTM0
**              Interrupt request                          : <Automatic>
**              Interrupt priority                         : <Automatic>
**              ISR Name                                   : FTM0_ISR
**              Channels interrupt/DMA                     : See the respective Channel settings
**              Timer overflow interrupt                   : Disabled
**              Fault interrupt                            : Disabled
**          Initialization                                 : 
**            Initialize the Channels Output               : no
**            Load enable                                  : no
**            Channel 0 load select                        : no
**            Channel 1 load select                        : no
**            Channel 2 load select                        : no
**            Channel 3 load select                        : no
**            Channel 4 load select                        : no
**            Channel 5 load select                        : no
**            Channel 6 load select                        : no
**            Channel 7 load select                        : no
**            Call Init method                             : yes
**            Utilize after reset values                   : default
**     Contents    :
**         Init - void M1_FTM0_PWM1_Init(void);
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file FTM0_Config.h                                                  
** @version 01.10
** @brief
**          This file implements the FTM (FTM0) module initialization
**          according to the Peripheral Initialization settings, and
**          defines interrupt service routines prototypes.
*/         
/*!
**  @addtogroup FTM0_Config_module FTM0_Config module documentation
**  @{
*/         

#ifndef FTM0_Config_H_
#define FTM0_Config_H_

/* MODULE M1_FTM0_PWM1. */



#define FTM0_MODE_VALUE_1 4
/* FTM0_SC: TOF=0,CPWMS=1 */
#define FTM0_SC_VALUE_2      0x20U
#define FTM0_SC_MASK_2       0xA0U
/* FTM0_MODE: FTMEN=1 */
#define FTM0_MODE_VALUE_2    0x01U
#define FTM0_MODE_MASK_2     0x01U
/* FTM0_COMBINE: FAULTEN3=0,SYNCEN3=0,DTEN3=0,COMP3=0,FAULTEN2=0,SYNCEN2=0,DTEN2=0,COMP2=0,FAULTEN1=0,SYNCEN1=0,DTEN1=0,COMP1=0,FAULTEN0=0,SYNCEN0=0,DTEN0=0,DECAPEN0=0,COMP0=0,COMBINE0=0 */
#define FTM0_COMBINE_VALUE   0x00U
#define FTM0_COMBINE_MASK    0x72727277U
/* FTM0_INVCTRL: INV3EN=0,INV2EN=0,INV1EN=0,INV0EN=0 */
#define FTM0_INVCTRL_VALUE   0x00U
#define FTM0_INVCTRL_MASK    0x0FU
/* FTM0_C0SC: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CHF=0,MSB=0,MSA=0,ELSB=0,ELSA=0,ICRST=0 */
#define FTM0_C0SC_VALUE      0x00U
#define FTM0_C0SC_MASK       0xFFFFFFBEU
/* FTM0_C1SC: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CHF=0,CHIE=1,ELSB=1,ELSA=0,ICRST=0,DMA=0 */
#define FTM0_C1SC_VALUE      0x48U
#define FTM0_C1SC_MASK       0xFFFFFFCFU
/* FTM0_C2SC: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CHF=0,MSB=0,MSA=0,ELSB=0,ELSA=0,ICRST=0 */
#define FTM0_C2SC_VALUE      0x00U
#define FTM0_C2SC_MASK       0xFFFFFFBEU
/* FTM0_C3SC: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CHF=0,MSB=0,MSA=0,ELSB=0,ELSA=0,ICRST=0 */
#define FTM0_C3SC_VALUE      0x00U
#define FTM0_C3SC_MASK       0xFFFFFFBEU
/* FTM0_C4SC: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CHF=0,MSB=0,MSA=0,ELSB=0,ELSA=0,ICRST=0 */
#define FTM0_C4SC_VALUE      0x00U
#define FTM0_C4SC_MASK       0xFFFFFFBEU
/* FTM0_C5SC: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CHF=0,MSB=0,MSA=0,ELSB=0,ELSA=0,ICRST=0 */
#define FTM0_C5SC_VALUE      0x00U
#define FTM0_C5SC_MASK       0xFFFFFFBEU
/* FTM0_C6SC: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CHF=0,MSB=0,MSA=0,ELSB=0,ELSA=0,ICRST=0 */
#define FTM0_C6SC_VALUE      0x00U
#define FTM0_C6SC_MASK       0xFFFFFFBEU
/* FTM0_C7SC: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CHF=0,MSB=0,MSA=0,ELSB=0,ELSA=0,ICRST=0 */
#define FTM0_C7SC_VALUE      0x00U
#define FTM0_C7SC_MASK       0xFFFFFFBEU
/* FTM0_C1V: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,VAL=0 */
#define FTM0_C1V_VALUE       0x00U
#define FTM0_C1V_MASK        0xFFFFFFFFU
/* FTM0_FILTER: CH1FVAL=0 */
#define FTM0_FILTER_VALUE    0x00U
#define FTM0_FILTER_MASK     0xF0U
/* FTM0_OUTINIT: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
#define FTM0_OUTINIT_VALUE   0x00U
#define FTM0_OUTINIT_MASK    0xFFFFFF00U
/* FTM0_FLTCTRL: FFVAL=0 */
#define FTM0_FLTCTRL_VALUE_1 0x00U
#define FTM0_FLTCTRL_MASK_1  0x0F00U
/* FTM0_FLTPOL: FLT3POL=0,FLT2POL=0,FLT1POL=0,FLT0POL=0 */
#define FTM0_FLTPOL_VALUE    0x00U
#define FTM0_FLTPOL_MASK     0x0FU
/* FTM0_FLTCTRL: FFLTR3EN=0,FFLTR2EN=0,FFLTR1EN=0,FFLTR0EN=0,FAULT3EN=0,FAULT2EN=0,FAULT1EN=0,FAULT0EN=0 */
#define FTM0_FLTCTRL_VALUE_2 0x00U
#define FTM0_FLTCTRL_MASK_2  0xFFU
/* FTM0_SYNC: SWSYNC=0,TRIG2=0,TRIG1=0,TRIG0=0,SYNCHOM=0,REINIT=0,CNTMAX=0,CNTMIN=0 */
#define FTM0_SYNC_VALUE      0x00U
#define FTM0_SYNC_MASK       0xFFU
/* FTM0_EXTTRIG: TRIGF=0,INITTRIGEN=0,CH1TRIG=0,CH0TRIG=0,CH5TRIG=0,CH4TRIG=0,CH3TRIG=0,CH2TRIG=0 */
#define FTM0_EXTTRIG_VALUE   0x00U
#define FTM0_EXTTRIG_MASK    0xFFU
/* FTM0_MOD: MOD=0xFFFF */
#define FTM0_MOD_VALUE       0xFFFFU
#define FTM0_MOD_MASK        0xFFFFU
/* FTM0_CNTIN: INIT=0 */
#define FTM0_CNTIN_VALUE     0x00U
#define FTM0_CNTIN_MASK      0xFFFFU
/* FTM0_DEADTIME: DTPS=0,DTVAL=0 */
#define FTM0_DEADTIME_VALUE  0x00U
#define FTM0_DEADTIME_MASK   0xFFU
/* FTM0_MODE: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,FAULTIE=0,FAULTM=0,PWMSYNC=0,INIT=0,FTMEN=1 */
#define FTM0_MODE_VALUE_3    0x01U
#define FTM0_MODE_MASK_3     0xFFFFFFEBU
/* FTM0_SYNCONF: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,SYNCMODE=0,??=0,SWOC=0,INVC=0,??=0,CNTINC=0,??=0,HWTRIGMODE=0 */
#define FTM0_SYNCONF_VALUE   0x00U
#define FTM0_SYNCONF_MASK    0xFFE0E0FFU
/* FTM0_SC: TOF=0,TOIE=0,CPWMS=1,CLKS=0,PS=0 */
#define FTM0_SC_VALUE_3      0x20U
#define FTM0_SC_MASK_3       0xFFU
/* FTM0_FMS: FAULTF=0,WPEN=0,FAULTF3=0,FAULTF2=0,FAULTF1=0,FAULTF0=0 */
#define FTM0_FMS_VALUE       0x00U
#define FTM0_FMS_MASK        0xCFU
/* FTM0_SWOCTRL: CH1OCV=1,CH1OC=1 */
#define FTM0_SWOCTRL_VALUE   0x0202U
#define FTM0_SWOCTRL_MASK    0x0202U
/* FTM0_CONF: GTBEOUT=0,GTBEEN=0,BDMMODE=0,NUMTOF=0 */
#define FTM0_CONF_VALUE      0x00U
#define FTM0_CONF_MASK       0x06DFU
/* FTM0_PWMLOAD: LDOK=0,CH7SEL=0,CH6SEL=0,CH5SEL=0,CH4SEL=0,CH3SEL=0,CH2SEL=0,CH1SEL=0,CH0SEL=0 */
#define FTM0_PWMLOAD_VALUE   0x00U
#define FTM0_PWMLOAD_MASK    0x02FFU

#define FTM0_AUTOINIT

#define INT_FTM0_ISR FTM0_ISR

/* END M1_FTM0_PWM1. */
#endif /* #ifndef __FTM0_Config_H_ */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
