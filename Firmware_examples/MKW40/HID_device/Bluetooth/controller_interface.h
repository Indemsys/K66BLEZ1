/*! *********************************************************************************
 * \defgroup CONTROLLER - Controller Interface
 * @{
 ********************************************************************************** */
/*!
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file controller_interface.h
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _CONTROLLER_INTERFACE_H_
#define _CONTROLLER_INTERFACE_H_


/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "fsl_osa_ext.h"
#include "ble_general.h"


/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

#define Controller_SetAdvertisingTxPowerLevel(level) \
    Controller_SetTxPowerLevel(level,gAdvTxChannel_c)

#define Controller_SetConnectionTxPowerLevel(level) \
    Controller_SetTxPowerLevel(level,gConnTxChannel_c)

/************************************************************************************
*************************************************************************************
* Structures/Data Types
*************************************************************************************
************************************************************************************/
typedef void (*gHostRecvCallback_t)
(
    hciPacketType_t packetType,
    void* pHciPacket,
    uint16_t hciPacketLength
);

typedef enum txChannelType_tag {
    gAdvTxChannel_c,
    gConnTxChannel_c
} txChannelType_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern osaEventId_t mControllerTaskEvent;
extern bool_t gActivateHybridMode;
/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
 * \brief  Performs initialization of the Controller.
 * \param[in]  callback HCI Host Receive Callback
 *
 * \return osaStatus_Success or osaStatus_Error
 ********************************************************************************** */
osaStatus_t Controller_Init(gHostRecvCallback_t callback);

/*! *********************************************************************************
 * \brief  Controller Receive Interface
 * \param[in]  packetType HCI packet Type
 * \param[in]  pPacket    data buffer      
 * \param[in]  packetSize data buffer length 
 *
 * \return none
 ********************************************************************************** */
void Controller_RecvPacket( hciPacketType_t packetType, void* pPacket, 
                                uint16_t packetSize);

/*! *********************************************************************************
 * \brief  Controller Get Next instance
 * \param[in]  pNextInstant Pointer to the calculated_instant CE instant
 *   for applying new channel map or for updating connection parameters.
 * \return none
 ********************************************************************************** */
uint16_t Controller_GetNextInstance(uint16_t* pNextInstant);

/*! *********************************************************************************
 * \brief  Sets the TX Power on the advertising or connection channel.
 * \param[in]  level    Power level (range 0-15) as defined in the table bellow.
 * \param[in]  channel  Advertising or connection channel.      
 *
 * \return gBleSuccess_c or error.
 *
 * \remarks This function executes synchronously.
 *
 * \remarks The MKW40Z BLE controller transmission is an open loop power control system. 
 * The following correspondence between power level and output power is a typical case, 
 * but may vary among parts (chips). 
 * 
 * LEVEL        [dBm]
 *  0	        -19.5
 *  1	        -12.1
 *  2	        -7
 *  3	        -4.2
 *  4	        -1.9
 *  5	        -0.38
 *  6	        0.84
 *  7	        1.76
 *  8	        2.36
 *  9	        3.01
 *  10	        3.56
 *  11	        4.02
 *  12	        4.41
 *  13	        4.77
 *  14	        5.06
 *  15	        5.32
 ********************************************************************************** */
bleResult_t Controller_SetTxPowerLevel(uint8_t level, txChannelType_t channel);

#ifdef __cplusplus
}
#endif 

#endif /* _CONTROLLER_INTERFACE_H_ */

/*! *********************************************************************************
* @}
********************************************************************************** */

