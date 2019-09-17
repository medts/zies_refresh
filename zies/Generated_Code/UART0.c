/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : UART0.c
**     Project     : zies
**     Processor   : MK22FN512VLH12
**     Component   : AsynchroSerial
**     Version     : Component 02.611, Driver 01.01, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-09-04, 15:49, # CodeGen: 7
**     Abstract    :
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
**     Settings    :
**          Component name                                 : UART0
**          Channel                                        : UART0
**          Interrupt service/event                        : Enabled
**            Interrupt RxD                                : INT_UART0_RX_TX
**            Interrupt RxD priority                       : medium priority
**            Interrupt TxD                                : INT_UART0_RX_TX
**            Interrupt TxD priority                       : medium priority
**            Interrupt Error                              : INT_UART0_ERR
**            Interrupt Error priority                     : medium priority
**            Input buffer size                            : 512
**            Output buffer size                           : 512
**            Handshake                                    : 
**              CTS                                        : Disabled
**              RTS                                        : Disabled
**          Settings                                       : 
**            Parity                                       : none
**            Width                                        : 8 bits
**            Stop bit                                     : 1
**            Receiver                                     : Enabled
**              RxD                                        : ADC0_SE7b/PTD6/LLWU_P15/SPI0_PCS3/UART0_RX/FTM0_CH6/FBa_AD0/FTM0_FLT0/SPI1_SOUT
**            Transmitter                                  : Enabled
**              TxD                                        : PTD7/UART0_TX/FTM0_CH7/FTM0_FLT1/SPI1_SIN
**            Baud rate                                    : 115200 baud
**            Break signal                                 : Enabled
**            Wakeup condition                             : Idle line wakeup
**            Transmitter output                           : Not inverted
**            Receiver input                               : Not inverted
**            Stop in wait mode                            : no
**            Idle line mode                               : starts after start bit
**            Break generation length                      : Short
**          Initialization                                 : 
**            Enabled in init. code                        : no
**            Events enabled in init.                      : yes
**          CPU clock/speed selection                      : 
**            High speed mode                              : This component enabled
**            Low speed mode                               : This component disabled
**            Slow speed mode                              : This component disabled
**          Referenced components                          : 
**            Serial_LDD                                   : Serial_LDD
**     Contents    :
**         Enable          - byte UART0_Enable(void);
**         Disable         - byte UART0_Disable(void);
**         RecvChar        - byte UART0_RecvChar(UART0_TComData *Chr);
**         SendBlock       - byte UART0_SendBlock(UART0_TComData *Ptr, word Size, word *Snd);
**         ClearRxBuf      - byte UART0_ClearRxBuf(void);
**         ClearTxBuf      - byte UART0_ClearTxBuf(void);
**         GetCharsInRxBuf - word UART0_GetCharsInRxBuf(void);
**         GetCharsInTxBuf - word UART0_GetCharsInTxBuf(void);
**         GetError        - byte UART0_GetError(UART0_TError *Err);
**         GetBreak        - byte UART0_GetBreak(bool *Brk);
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
** @file UART0.c
** @version 01.01
** @brief
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
*/         
/*!
**  @addtogroup UART0_module UART0 module documentation
**  @{
*/         

/* MODULE UART0. */

#include "UART0.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


#define OVERRUN_ERR      0x01U         /* Overrun error flag bit    */
#define FRAMING_ERR      0x02U         /* Framing error flag bit    */
#define PARITY_ERR       0x04U         /* Parity error flag bit     */
#define CHAR_IN_RX       0x08U         /* Char is in RX buffer      */
#define FULL_TX          0x10U         /* Full transmit buffer      */
#define RUNINT_FROM_TX   0x20U         /* Interrupt is in progress  */
#define FULL_RX          0x40U         /* Full receive buffer       */
#define NOISE_ERR        0x80U         /* Noise error flag bit      */
#define IDLE_ERR         0x0100U       /* Idle character flag bit   */
#define BREAK_ERR        0x0200U       /* Break detect              */

LDD_TDeviceData *ASerialLdd1_DeviceDataPtr; /* Device data pointer */
static bool EnUser;                    /* Enable/Disable SCI */
static bool EnMode;                    /* Enable/Disable SCI in speed mode */
static word SerFlag;                   /* Flags for serial communication */
                                       /* Bits: 0 - OverRun error */
                                       /*       1 - Framing error */
                                       /*       2 - Parity error */
                                       /*       3 - Char in RX buffer */
                                       /*       4 - Full TX buffer */
                                       /*       5 - Running int from TX */
                                       /*       6 - Full RX buffer */
                                       /*       7 - Noise error */
                                       /*       8 - Idle character  */
                                       /*       9 - Break detected  */
                                       /*      10 - Unused */
                                       /*      11 - Unused */
static word ErrFlag;                   /* Error flags mirror of SerFlag */
static word UART0_InpLen;              /* Length of input buffer's content */
static word InpIndexR;                 /* Index for reading from input buffer */
static word InpIndexW;                 /* Index for writing to input buffer */
static UART0_TComData InpBuffer[UART0_INP_BUF_SIZE]; /* Input buffer for SCI communication */
static UART0_TComData BufferRead;      /* Input char for SCI communication */
static word UART0_OutLen;              /* Length of output bufer's content */
static word OutIndexR;                 /* Index for reading from output buffer */
static word OutIndexW;                 /* Index for writing to output buffer */
static UART0_TComData OutBuffer[UART0_OUT_BUF_SIZE]; /* Output buffer for SCI communication */

/*
** ===================================================================
**     Method      :  HWEnDi (component AsynchroSerial)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the bean.
**         The method is called automatically as a part of the Enable and 
**         Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void HWEnDi(void)
{
  if (EnMode && EnUser) {              /* Enable device? */
    (void)ASerialLdd1_Enable(ASerialLdd1_DeviceDataPtr); /* Enable device */
    (void)ASerialLdd1_ReceiveBlock(ASerialLdd1_DeviceDataPtr, &BufferRead, 1U); /* Receive one data byte */
    if ((UART0_OutLen) != 0U) {        /* Is number of bytes in the transmit buffer greater then 0? */
      SerFlag |= RUNINT_FROM_TX;       /* Set flag "running int from TX"? */
      (void)ASerialLdd1_SendBlock(ASerialLdd1_DeviceDataPtr, (LDD_TData *)&OutBuffer[OutIndexR], 1U); /* Send one data byte */
    }
  } else {
    (void)ASerialLdd1_Disable(ASerialLdd1_DeviceDataPtr); /* Disable device */
  }
}

/*
** ===================================================================
**     Method      :  UART0_Enable (component AsynchroSerial)
**     Description :
**         Enables the component - it starts the send and receive
**         functions. Events may be generated
**         ("DisableEvent"/"EnableEvent").
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte UART0_Enable(void)
{
  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  if (!EnUser) {                       /* Is the device disabled by user? */
    EnUser = TRUE;                     /* If yes then set the flag "device enabled" */
    HWEnDi();                          /* Enable the device */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  UART0_Disable (component AsynchroSerial)
**     Description :
**         Disables the component - it stops the send and receive
**         functions. No events will be generated.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte UART0_Disable(void)
{
  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  if (EnUser) {                        /* Is the device enabled by user? */
    EnUser = FALSE;                    /* If yes then set the flag "device disabled" */
    HWEnDi();                          /* Disable the device */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  UART0_RecvChar (component AsynchroSerial)
**     Description :
**         If any data is received, this method returns one character,
**         otherwise it returns an error code (it does not wait for
**         data). This method is enabled only if the receiver property
**         is enabled.
**         [Note:] Because the preferred method to handle error and
**         break exception in the interrupt mode is to use events
**         <OnError> and <OnBreak> the return value ERR_RXEMPTY has
**         higher priority than other error codes. As a consequence the
**         information about an exception in interrupt mode is returned
**         only if there is a valid character ready to be read.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Chr             - Pointer to a received character
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_RXEMPTY - No data in receiver
**                           ERR_BREAK - Break character is detected
**                           (only when the <Interrupt service> property
**                           is disabled and the <Break signal> property
**                           is enabled)
**                           ERR_COMMON - common error occurred (the
**                           <GetError> method can be used for error
**                           specification)
** ===================================================================
*/
byte UART0_RecvChar(UART0_TComData *Chr)
{
  byte Result = ERR_OK;                /* Return error code */

  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  if (UART0_InpLen > 0x00U) {          /* Is number of received chars greater than 0? */
    EnterCritical();                   /* Disable global interrupts */
    UART0_InpLen--;                    /* Decrease number of received chars */
    *Chr = InpBuffer[InpIndexR++];     /* Received char */
    if (InpIndexR >= UART0_INP_BUF_SIZE) { /* Is the index out of the receive buffer? */
      InpIndexR = 0x00U;               /* Set index to the first item into the receive buffer */
    }
    Result = (byte)((SerFlag & (OVERRUN_ERR|FRAMING_ERR|PARITY_ERR|NOISE_ERR|BREAK_ERR|FULL_RX))? ERR_COMMON : ERR_OK);
    SerFlag &= (word)~(word)(OVERRUN_ERR|FRAMING_ERR|PARITY_ERR|NOISE_ERR|BREAK_ERR|FULL_RX|CHAR_IN_RX); /* Clear all errors in the status variable */
    ExitCritical();                    /* Enable global interrupts */
  } else {
    return ERR_RXEMPTY;                /* Receiver is empty */
  }
  return Result;                       /* Return error code */
}

/*
** ===================================================================
**     Method      :  UART0_SendBlock (component AsynchroSerial)
**     Description :
**         Sends a block of characters to the channel.
**         This method is available only if non-zero length of the
**         output buffer is defined and the transmitter property is
**         enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Ptr             - Pointer to the block of data to send
**         Size            - Size of the block
**       * Snd             - Pointer to number of data that are sent
**                           (moved to buffer)
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_TXFULL - It was not possible to send
**                           requested number of bytes
** ===================================================================
*/
byte UART0_SendBlock(UART0_TComData *Ptr, word Size, word *Snd)
{
  word count = 0x00U;                  /* Number of sent chars */
  UART0_TComData *TmpPtr = Ptr;        /* Temporary output buffer pointer */

  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  while ((count < Size) && (UART0_OutLen < UART0_OUT_BUF_SIZE)) { /* While there is some char desired to send left and output buffer is not full do */
    EnterCritical();                   /* Enter the critical section */
    UART0_OutLen++;                    /* Increase number of bytes in the transmit buffer */
    OutBuffer[OutIndexW++] = *TmpPtr++; /* Store char to buffer */
    if (OutIndexW >= UART0_OUT_BUF_SIZE) { /* Is the index out of the transmit buffer? */
      OutIndexW = 0x00U;               /* Set index to the first item in the transmit buffer */
    }
    count++;                           /* Increase the count of sent data */
    if ((EnUser) && ((SerFlag & RUNINT_FROM_TX) == 0U)) { /* Is the device enabled by user? */
      SerFlag |= RUNINT_FROM_TX;       /* Set flag "running int from TX"? */
      (void)ASerialLdd1_SendBlock(ASerialLdd1_DeviceDataPtr, (LDD_TData *)&OutBuffer[OutIndexR], 1U); /* Send one data byte */
    }
    ExitCritical();                    /* Exit the critical section */
  }
  *Snd = count;                        /* Return number of sent chars */
  if (count != Size) {                 /* Is the number of sent chars less then desired number of chars */
    return ERR_TXFULL;                 /* If yes then error */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  UART0_ClearRxBuf (component AsynchroSerial)
**     Description :
**         Clears the receive buffer.
**         This method is available only if non-zero length of the
**         input buffer is defined and the receiver property is enabled.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte UART0_ClearRxBuf(void)
{
  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  EnterCritical();                     /* Disable global interrupts */
  UART0_InpLen = 0x00U;                /* Set number of chars in the transmit buffer to 0 */
  InpIndexW = 0x00U;                   /* Set index on the first item in the transmit buffer */
  InpIndexR = 0x00U;
  ExitCritical();                      /* Enable global interrupts */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  UART0_ClearTxBuf (component AsynchroSerial)
**     Description :
**         Clears the transmit buffer.
**         This method is available only if non-zero length of the
**         output buffer is defined and the receiver property is
**         enabled.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte UART0_ClearTxBuf(void)
{
  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  EnterCritical();                     /* Disable global interrupts */
  UART0_OutLen = 0x00U;                /* Set number of chars in the receive buffer to 0 */
  OutIndexW = 0x00U;                   /* Set index on the first item in the receive buffer */
  OutIndexR = 0x00U;
  ExitCritical();                      /* Enable global interrupts */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  UART0_GetCharsInRxBuf (component AsynchroSerial)
**     Description :
**         Returns the number of characters in the input buffer. This
**         method is available only if the receiver property is enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the input
**                           buffer.
** ===================================================================
*/
word UART0_GetCharsInRxBuf(void)
{
  return UART0_InpLen;                 /* Return number of chars in receive buffer */
}

/*
** ===================================================================
**     Method      :  UART0_GetCharsInTxBuf (component AsynchroSerial)
**     Description :
**         Returns the number of characters in the output buffer. This
**         method is available only if the transmitter property is
**         enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the output
**                           buffer.
** ===================================================================
*/
word UART0_GetCharsInTxBuf(void)
{
  return UART0_OutLen;                 /* Return number of chars in the transmitter buffer */
}

/*
** ===================================================================
**     Method      :  UART0_GetError (component AsynchroSerial)
**     Description :
**         Returns a set of errors on the channel (errors that cannot
**         be returned by given methods). The errors accumulate in a
**         set; after calling [GetError] this set is returned and
**         cleared.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Err             - Pointer to the returned set of errors
**     Returns     :
**         ---             - Error code (if GetError did not succeed),
**                           possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte UART0_GetError(UART0_TError *Err)
{
  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  EnterCritical();                     /* Disable global interrupts */
  Err->err = 0U;
  Err->errName.OverRun = (((ErrFlag & OVERRUN_ERR) != 0x00U)? 1U : 0U); /* Overrun error */
  Err->errName.Framing = (((ErrFlag & FRAMING_ERR ) != 0x00U)? 1U : 0U); /* Framing error */
  Err->errName.Parity = (((ErrFlag & PARITY_ERR) != 0x00U)? 1U : 0U); /* Parity error */
  Err->errName.RxBufOvf = (((ErrFlag & FULL_RX) != 0x00U)? 1U : 0U); /* Buffer overflow */
  Err->errName.Noise = (((ErrFlag & NOISE_ERR) != 0x00U)? 1U : 0U); /* Noise error */
  Err->errName.Break = (((ErrFlag & BREAK_ERR) != 0x00U)? 1U : 0U); /* Break error */
  ErrFlag = 0U;                        /* Clear error flags */
  ExitCritical();                      /* Enable global interrupts */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  UART0_GetBreak (component AsynchroSerial)
**     Description :
**         Tests the internal input break flag, returns it (whether the
**         break has occurred or not) and clears it. This method is
**         available only if the property <Break signal> and the
**         property receiver are enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Brk             - Pointer to the returned internal break flag
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte UART0_GetBreak(bool *Brk)
{
  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  EnterCritical();                     /* Save the PS register */
  *Brk = (((SerFlag & BREAK_ERR) != 0U) ? 1U : 0U); /* Has break signal been detected? */
  SerFlag &= (word)(~(word)BREAK_ERR); /* Reset break signal flag */
  ExitCritical();                      /* Restore the PS register */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  UART0_Init (component AsynchroSerial)
**
**     Description :
**         Initializes the associated peripheral(s) and the bean internal 
**         variables. The method is called automatically as a part of the 
**         application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void UART0_Init(void)
{
  SerFlag = 0x00U;                     /* Reset flags */
  EnUser = FALSE;                      /* Disable device */
  UART0_InpLen = 0x00U;                /* No char in the receive buffer */
  InpIndexR = 0x00U;                   /* Set index on the first item in the receive buffer */
  InpIndexW = 0x00U;
  UART0_OutLen = 0x00U;                /* No char in the transmit buffer */
  OutIndexR = 0x00U;                   /* Set index on the first item in the transmit buffer */
  OutIndexW = 0x00U;
  ASerialLdd1_DeviceDataPtr = ASerialLdd1_Init(NULL); /* Calling init method of the inherited component */
  EnMode = TRUE;                       /* Set the flag "device enabled" in the actual speed CPU mode */
  HWEnDi();                            /* Enable/disable device according to status flags */
}

/*
** ===================================================================
**     Method      :  UART0_SetClockConfiguration (component AsynchroSerial)
**
**     Description :
**         This method changes the clock configuration.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void UART0_SetClockConfiguration(LDD_TClockConfiguration ClockConfiguration)
{
  switch (ClockConfiguration) {
    case CPU_CLOCK_CONFIG_0:
      EnMode = TRUE;                   /* Set the flag "device enabled" in the actual speed CPU mode */
      break;
    default:
      EnMode = FALSE;                  /* Set the flag "device disabled" in the actual speed CPU mode */
      break;
  }
  if (EnMode && EnUser) {              /* Enable device? */
    HWEnDi();                          /* Enable/disable device according to status flags */
  }
}

#define ON_ERROR    0x01U
#define ON_FULL_RX  0x02U
#define ON_RX_CHAR  0x04U
/*
** ===================================================================
**     Method      :  UART0_ASerialLdd1_OnBlockReceived (component AsynchroSerial)
**
**     Description :
**         This event is called when the requested number of data is 
**         moved to the input buffer.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void ASerialLdd1_OnBlockReceived(LDD_TUserData *UserDataPtr)
{
  register byte Flags = 0U;            /* Temporary variable for flags */

  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  if (UART0_InpLen < UART0_INP_BUF_SIZE) { /* Is number of bytes in the receive buffer lower than size of buffer? */
    UART0_InpLen++;                    /* Increase number of chars in the receive buffer */
    InpBuffer[InpIndexW++] = (UART0_TComData)BufferRead; /* Save received char to the receive buffer */
    if (InpIndexW >= UART0_INP_BUF_SIZE) { /* Is the index out of the receive buffer? */
      InpIndexW = 0x00U;               /* Set index on the first item into the receive buffer */
    }
    Flags |= ON_RX_CHAR;               /* If yes then set the OnRxChar flag */
    if (UART0_InpLen == UART0_INP_BUF_SIZE) { /* Is number of bytes in the receive buffer equal to the size of buffer? */
      Flags |= ON_FULL_RX;             /* Set flag "OnFullRxBuf" */
    }
  } else {
    SerFlag |= FULL_RX;                /* Set flag "full RX buffer" */
    ErrFlag |= FULL_RX;                /* Set flag "full RX buffer" for GetError method */
    Flags |= ON_ERROR;                 /* Set the OnError flag */
  }
  if ((Flags & ON_ERROR) != 0U) {      /* Is any error flag set? */
    UART0_OnError();                   /* Invoke user event */
  } else {
    if ((Flags & ON_RX_CHAR) != 0U) {  /* Is OnRxChar flag set? */
      UART0_OnRxChar();                /* Invoke user event */
    }
    if ((Flags & ON_FULL_RX) != 0U) {  /* Is OnTxChar flag set? */
      UART0_OnFullRxBuf();             /* Invoke user event */
    }
  }
  (void)ASerialLdd1_ReceiveBlock(ASerialLdd1_DeviceDataPtr, &BufferRead, 1U); /* Receive one data byte */
}

#define ON_FREE_TX  0x01U
#define ON_TX_CHAR  0x02U
/*
** ===================================================================
**     Method      :  UART0_ASerialLdd1_OnBlockSent (component AsynchroSerial)
**
**     Description :
**         This event is called after the last character from the output 
**         buffer is moved to the transmitter.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void ASerialLdd1_OnBlockSent(LDD_TUserData *UserDataPtr)
{
  word OnFlags = 0x00U;                /* Temporary variable for flags */

  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  OutIndexR++;
  if (OutIndexR >= UART0_OUT_BUF_SIZE) { /* Is the index out of the transmit buffer? */
    OutIndexR = 0x00U;                 /* Set index on the first item into the transmit buffer */
  }
  UART0_OutLen--;                      /* Decrease number of chars in the transmit buffer */
  if (UART0_OutLen != 0U) {            /* Is number of bytes in the transmit buffer greater then 0? */
    SerFlag |= RUNINT_FROM_TX;         /* Set flag "running int from TX"? */
    (void)ASerialLdd1_SendBlock(ASerialLdd1_DeviceDataPtr, (LDD_TData *)&OutBuffer[OutIndexR], 1U); /* Send one data byte */
  } else {
    SerFlag &= (byte)~(RUNINT_FROM_TX); /* Clear "running int from TX" and "full TX buff" flags */
  }
}

/*
** ===================================================================
**     Method      :  UART0_ASerialLdd1_OnError (component AsynchroSerial)
**
**     Description :
**         This event is called when a channel error (not the error 
**         returned by a given method) occurs.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void ASerialLdd1_OnError(LDD_TUserData *UserDataPtr)
{
  LDD_SERIAL_TError SerialErrorMask;   /* Serial error mask variable */

  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  (void)ASerialLdd1_GetError(ASerialLdd1_DeviceDataPtr, &SerialErrorMask); /* Get error state */
  if (SerialErrorMask != 0U) {
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_PARITY_ERROR) != 0U ) ? PARITY_ERR : 0U);
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_NOISE_ERROR) != 0U ) ? NOISE_ERR : 0U);
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_RX_OVERRUN) != 0U ) ? OVERRUN_ERR : 0U);
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_FRAMING_ERROR) != 0U ) ? FRAMING_ERR : 0U);
    ErrFlag |= (SerFlag);              /* Add new error flags into the ErrorFlag status variable */
  }
  UART0_OnError();                     /* Invoke user event */
}

/*
** ===================================================================
**     Method      :  UART0_ASerialLdd1_OnTxComplete (component AsynchroSerial)
**
**     Description :
**         This event indicates that the transmitter is finished 
**         transmitting all data, preamble, and break characters and is 
**         idle.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void ASerialLdd1_OnTxComplete(LDD_TUserData *UserDataPtr)
{
  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  UART0_OnTxComplete();                /* Invoke user event */
}

/*
** ===================================================================
**     Method      :  UART0_ASerialLdd1_OnBreak (component AsynchroSerial)
**
**     Description :
**         This event is called when a break occurs on the input channel.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void ASerialLdd1_OnBreak(LDD_TUserData *UserDataPtr)
{
  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  SerFlag |= BREAK_ERR;                /* Set error break flag */
  UART0_OnBreak();                     /* Invoke user event */
}


/* END UART0. */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
