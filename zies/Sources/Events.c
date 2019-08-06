/* ###################################################################
 **     Filename    : Events.c
 **     Project     : zies
 **     Processor   : MK22FN512VLH12
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2019-04-09, 12:21, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Contents    :
 **         Cpu_OnNMI - void Cpu_OnNMI(void);
 **
 ** ###################################################################*/
/*!
 ** @file Events.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup Events_module Events module documentation
 **  @{
 */
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "Init_Config.h"
#include "PDD_Includes.h"

#ifdef __cplusplus
extern "C"
{
#endif 

/* User includes (#include below this line is not maintained by Processor Expert) */

/*
 ** ===================================================================
 **     Event       :  Debug_UART1_OnError (module Events)
 **
 **     Component   :  Debug_UART1 [AsynchroSerial]
 **     Description :
 **         This event is called when a channel error (not the error
 **         returned by a given method) occurs. The errors can be read
 **         using <GetError> method.
 **         The event is available only when the <Interrupt
 **         service/event> property is enabled.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void Debug_UART1_OnError ( void )
{
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  Debug_UART1_OnRxChar (module Events)
 **
 **     Component   :  Debug_UART1 [AsynchroSerial]
 **     Description :
 **         This event is called after a correct character is received.
 **         The event is available only when the <Interrupt
 **         service/event> property is enabled and either the <Receiver>
 **         property is enabled or the <SCI output mode> property (if
 **         supported) is set to Single-wire mode.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void Debug_UART1_OnRxChar ( void )
{
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  Debug_UART1_OnFullRxBuf (module Events)
 **
 **     Component   :  Debug_UART1 [AsynchroSerial]
 **     Description :
 **         This event is called when the input buffer is full;
 **         i.e. after reception of the last character
 **         that was successfully placed into input buffer.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void Debug_UART1_OnFullRxBuf ( void )
{
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  Debug_UART1_OnBreak (module Events)
 **
 **     Component   :  Debug_UART1 [AsynchroSerial]
 **     Description :
 **         This event is called when a break occurs on the input
 **         channel.
 **         The event is available only when both <Interrupt
 **         service/event> and <Break signal> properties are enabled.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void Debug_UART1_OnBreak ( void )
{
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  Debug_UART1_OnTxComplete (module Events)
 **
 **     Component   :  Debug_UART1 [AsynchroSerial]
 **     Description :
 **         This event indicates that the transmitter is finished
 **         transmitting all data, preamble, and break characters and is
 **         idle. It can be used to determine when it is safe to switch
 **         a line driver (e.g. in RS-485 applications).
 **         The event is available only when both <Interrupt
 **         service/event> and <Transmitter> properties are enabled.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void Debug_UART1_OnTxComplete ( void )
{
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  UART0_OnError (module Events)
 **
 **     Component   :  UART0 [AsynchroSerial]
 **     Description :
 **         This event is called when a channel error (not the error
 **         returned by a given method) occurs. The errors can be read
 **         using <GetError> method.
 **         The event is available only when the <Interrupt
 **         service/event> property is enabled.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void UART0_OnError ( void )
{
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  UART0_OnRxChar (module Events)
 **
 **     Component   :  UART0 [AsynchroSerial]
 **     Description :
 **         This event is called after a correct character is received.
 **         The event is available only when the <Interrupt
 **         service/event> property is enabled and either the <Receiver>
 **         property is enabled or the <SCI output mode> property (if
 **         supported) is set to Single-wire mode.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void UART0_OnRxChar ( void )
{
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  UART0_OnFullRxBuf (module Events)
 **
 **     Component   :  UART0 [AsynchroSerial]
 **     Description :
 **         This event is called when the input buffer is full;
 **         i.e. after reception of the last character
 **         that was successfully placed into input buffer.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void UART0_OnFullRxBuf ( void )
{
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  UART0_OnBreak (module Events)
 **
 **     Component   :  UART0 [AsynchroSerial]
 **     Description :
 **         This event is called when a break occurs on the input
 **         channel.
 **         The event is available only when both <Interrupt
 **         service/event> and <Break signal> properties are enabled.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void UART0_OnBreak ( void )
{
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  UART0_OnTxComplete (module Events)
 **
 **     Component   :  UART0 [AsynchroSerial]
 **     Description :
 **         This event indicates that the transmitter is finished
 **         transmitting all data, preamble, and break characters and is
 **         idle. It can be used to determine when it is safe to switch
 **         a line driver (e.g. in RS-485 applications).
 **         The event is available only when both <Interrupt
 **         service/event> and <Transmitter> properties are enabled.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void UART0_OnTxComplete ( void )
{
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  Cpu_OnHardFault (module Events)
 **
 **     Component   :  Cpu [MK22FN512LH12]
 */
/*!
 **     @brief
 **         This event is called when the Hard Fault exception had
 **         occurred. This event is automatically enabled when the [Hard
 **         Fault] property is set to 'Enabled'.
 */
/* ===================================================================*/
void Cpu_OnHardFault ( void )
{
	/* Write your code here ... */
}

/* END Events */

#ifdef __cplusplus
} /* extern "C" */
#endif 

/* ===================================================================*/
PE_ISR(FTM0_ISR)
{

}

PE_ISR(FTM1_ISR)
{

}

PE_ISR(FTM2_ISR)
{

}

PE_ISR(FTM3_ISR)
{

}

/* ===================================================================*/
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
