/*! *********************************************************************************
* \addtogroup HID Device
* @{
********************************************************************************** */
/*!
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
* \file app.c
* This file is the source file for the HID Device application
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

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/* Framework / Drivers */
#include "RNG_interface.h"
#include "Keyboard.h"
#include "Led.h"
#include "TimersManager.h"
#include "FunctionLib.h"
#include "fsl_os_abstraction.h"
#include "panic.h"

/* BLE Host Stack */
#include "gatt_interface.h"
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gatt_database.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"
#include "gatt_db_handles.h"
#include "hci_transport.h"

/* Profile / Services */
#include "battery_interface.h"
#include "device_info_interface.h"
#include "hid_interface.h"

#include "board.h"
#include "ApplMain.h"
#include "HID_dev_app.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define AXIS_MIN  100
#define AXIS_MAX  500
#define MOUSE_STEP 10

#define mBatteryLevelReportInterval_c   (10)        /* battery level report interval in seconds  */
/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
#if gBondingSupported_d
    fastWhiteListAdvState_c,
#endif
    fastAdvState_c,
    slowAdvState_c
}advType_t;

typedef struct advState_tag{
    bool_t      advOn;
    advType_t   advType;
}advState_t;

typedef struct mouseHidReport_tag{
  uint8_t buttonStatus;
  uint8_t xAxis;
  uint8_t yAxis;
}mouseHidReport_t;
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/* Host Stack Data*/
static bleDeviceAddress_t   maBleDeviceAddress;
static deviceId_t           mPeerDeviceId = gInvalidDeviceId_c;

/* Adv Parmeters */
static advState_t  mAdvState;
static gapAdvertisingParameters_t advParams = gGapDefaultAdvertisingParameters_d;

#if gBondingSupported_d
static bleDeviceAddress_t   maPeerDeviceAddress;
static uint8_t mcBondedDevices = 0;
static bleAddressType_t     mPeerDeviceAddressType;
#endif

/* Service Data*/
static basConfig_t basServiceConfig = {service_battery, 0};
static disConfig_t disServiceConfig = {service_device_info};
static hidConfig_t hidServiceConfig = {service_hid, gHid_ReportProtocolMode_c};
static uint16_t cpHandles[] = { value_hid_control_point };

/* Application specific data*/
static tmrTimerID_t mAdvTimerId;
static tmrTimerID_t mHidDemoTimerId;
static tmrTimerID_t mBatteryMeasurementTimerId;

static uint16_t xAxis = AXIS_MIN;
static uint16_t yAxis = AXIS_MIN;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

/* Gatt and Att callbacks */
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent);
static void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent);
static void BleApp_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent);
static void BleApp_Config();

/* Timer Callbacks */
static void AdvertisingTimerCallback (void *);
static void TimerHidMouseCallback (void *);
static void BatteryMeasurementTimerCallback (void *);

static void BleApp_Advertise(void);

/* Mouse events */
static void SendReport(mouseHidReport_t *pReport);
static void MoveMouseLeft(uint8_t pixels);
static void MoveMouseRight(uint8_t pixels);
static void MoveMouseUp(uint8_t pixels);
static void MoveMouseDown(uint8_t pixels);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief    Initializes application specific functionality before the BLE stack init.
*
********************************************************************************** */
void BleApp_Init(void)
{
    /* Initialize application support for drivers */
    BOARD_InitAdc();


}

/*! *********************************************************************************
* \brief    Starts the BLE application.
*
********************************************************************************** */
void BleApp_Start(void)
{
#if gBondingSupported_d
    if (mcBondedDevices > 0)
    {
        mAdvState.advType = fastWhiteListAdvState_c;
    }
    else
    {
#endif
        mAdvState.advType = fastAdvState_c;
#if gBondingSupported_d
    }
#endif

    BleApp_Advertise();
}

/*! *********************************************************************************
* \brief        Handles keyboard events.
*
* \param[in]    events    Key event structure.
********************************************************************************** */
void BleApp_HandleKeys(key_event_t events)
{
    switch (events)
    {
        case gKBD_EventPressPB1_c:
        {
            BleApp_Start();
            break;
        }
        case gKBD_EventPressPB2_c:
        {
        hidProtocolMode_t protocolMode;

        /* Toggle Protocol Mode */
        Hid_GetProtocolMode(service_hid, &protocolMode);
        protocolMode = (protocolMode == gHid_BootProtocolMode_c)?gHid_ReportProtocolMode_c:gHid_BootProtocolMode_c;
        Hid_SetProtocolMode(service_hid, protocolMode);
        break;
        }

        case gKBD_EventLongPB1_c:
        {
            if (mPeerDeviceId != gInvalidDeviceId_c)
                Gap_Disconnect(mPeerDeviceId);
            break;
        }
        case gKBD_EventLongPB2_c:
        default:
            break;
    }
}

/*! *********************************************************************************
* \brief        Handles BLE generic callback.
*
* \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
********************************************************************************** */
void BleApp_GenericCallback (gapGenericEvent_t* pGenericEvent)
{
    switch (pGenericEvent->eventType)
    {
        case gInitializationComplete_c:
        {
            BleApp_Config();
        }
        break;

        case gPublicAddressRead_c:
        {
            /* Use address read from the controller */
            FLib_MemCpyReverseOrder(maBleDeviceAddress, pGenericEvent->eventData.aAddress, sizeof(bleDeviceAddress_t));
        }
        break;

        case gAdvertisingDataSetupComplete_c:
        {
        }
        break;

        case gAdvertisingParametersSetupComplete_c:
        {
            App_StartAdvertising(BleApp_AdvertisingCallback, BleApp_ConnectionCallback);
        }
        break;

        case gInternalError_c:
        {
            panic(0,0,0,0);
        }
        break;

        default:
            break;
    }
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief        Configures BLE Stack after initialization. Usually used for
*               configuring advertising, scanning, white list, services, et al.
*
********************************************************************************** */
static void BleApp_Config()
{
    /* Read public address from controller */
    Gap_ReadPublicDeviceAddress();

    /* Register for callbacks*/
    GattServer_RegisterHandlesForWriteNotifications(sizeof(cpHandles), cpHandles);
    App_RegisterGattServerCallback(BleApp_GattServerCallback);

    /* Register security requirements */
#if gUseServiceSecurity_d
    Gap_RegisterDeviceSecurityRequirements(&deviceSecurityRequirements);
#endif

    /* Set local passkey */
#if gBondingSupported_d
    Gap_SetLocalPasskey(gPasskeyValue_c);
#endif

    /* Setup Advertising and scanning data */
    Gap_SetAdvertisingData(&gAppAdvertisingData, &gAppScanRspData);

    /* Populate White List if bonding is supported */
#if gBondingSupported_d
    bleDeviceAddress_t aBondedDevAdds[gcGapMaximumBondedDevices_d];
    bleResult_t result = Gap_GetBondedStaticAddresses(aBondedDevAdds, gcGapMaximumBondedDevices_d, &mcBondedDevices);

    if (gBleSuccess_c == result && mcBondedDevices > 0)
    {
        for (uint8_t i = 0; i < mcBondedDevices; i++)
        {
            Gap_AddDeviceToWhiteList(gBleAddrTypePublic_c, aBondedDevAdds[i]);
        }
    }
#endif

    mAdvState.advOn = FALSE;

    /* Start services */
    basServiceConfig.batteryLevel = BOARD_GetBatteryLevel();
    Bas_Start(&basServiceConfig);
    Dis_Start(&disServiceConfig);
    Hid_Start(&hidServiceConfig);

    /* Allocate application timers */
    mAdvTimerId = TMR_AllocateTimer();
    mHidDemoTimerId = TMR_AllocateTimer();
    mBatteryMeasurementTimerId = TMR_AllocateTimer();
}

static void BleApp_Advertise(void)
{
    uint32_t timeout;

    switch (mAdvState.advType)
    {
#if gBondingSupported_d
        case fastWhiteListAdvState_c:
        {
            advParams.minInterval = gFastConnMinAdvInterval_c;
            advParams.maxInterval = gFastConnMaxAdvInterval_c;
            advParams.filterPolicy = (gapAdvertisingFilterPolicyFlags_t)(gProcessConnWhiteListFlag_c | gProcessScanWhiteListFlag_c);
            timeout = gFastConnWhiteListAdvTime_c;
        }
        break;
#endif
        case fastAdvState_c:
        {
            advParams.minInterval = gFastConnMinAdvInterval_c;
            advParams.maxInterval = gFastConnMaxAdvInterval_c;
            advParams.filterPolicy = gProcessAll_c;
            timeout = gFastConnAdvTime_c - gFastConnWhiteListAdvTime_c;
        }
        break;

        case slowAdvState_c:
        {
            advParams.minInterval = gReducedPowerMinAdvInterval_c;
            advParams.maxInterval = gReducedPowerMinAdvInterval_c;
            advParams.filterPolicy = gProcessAll_c;
            timeout = gReducedPowerAdvTime_c;
        }
        break;
    }

    /* Set advertising parameters*/
    Gap_SetAdvertisingParameters(&advParams);

    /* Start advertising timer */
    TMR_StartLowPowerTimer(mAdvTimerId,gTmrLowPowerSecondTimer_c,
               TmrSeconds(timeout), AdvertisingTimerCallback, NULL);
}

/*! *********************************************************************************
* \brief        Handles BLE Advertising callback from host stack.
*
* \param[in]    pAdvertisingEvent    Pointer to gapAdvertisingEvent_t.
********************************************************************************** */
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent)
{
    switch (pAdvertisingEvent->eventType)
    {
        case gAdvertisingStateChanged_c:
        {
            mAdvState.advOn = !mAdvState.advOn;
            LED_StopFlashingAllLeds();
            Led1Flashing();

            if(!mAdvState.advOn)
            {
                Led2Flashing();
                Led3Flashing();
                Led4Flashing();
            }
        }
        break;

        case gAdvertisingCommandFailed_c:
        {
            panic(0,0,0,0);
        }
        break;

        default:
            break;
    }
}

/*! *********************************************************************************
* \brief        Handles BLE Connection callback from host stack.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    pConnectionEvent    Pointer to gapConnectionEvent_t.
********************************************************************************** */
static void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent)
{
    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
            mPeerDeviceId = peerDeviceId;

            /* Advertising stops when connected */
            mAdvState.advOn = FALSE;

#if gBondingSupported_d
            /* Copy peer device address information */
            mPeerDeviceAddressType = pConnectionEvent->eventData.connectedEvent.peerAddressType;
            FLib_MemCpy(maPeerDeviceAddress, pConnectionEvent->eventData.connectedEvent.peerAddress, sizeof(bleDeviceAddress_t));
#endif
#if gUseServiceSecurity_d
            {
                bool_t isBonded = FALSE ;

                if (gBleSuccess_c == Gap_CheckIfBonded(peerDeviceId, &isBonded) &&
                    FALSE == isBonded)
                {
                      Gap_SendSlaveSecurityRequest(peerDeviceId, TRUE, gSecurityMode_1_Level_3_c);
                }
            }
#endif
            /* Subscribe client*/
            Bas_Subscribe(peerDeviceId);
            Hid_Subscribe(peerDeviceId);

            /* UI */
            LED_StopFlashingAllLeds();
            Led1On();

            /* Stop Advertising Timer*/
            mAdvState.advOn = FALSE;
            TMR_StopTimer(mAdvTimerId);

            /* Start HID demo */
            TMR_StartLowPowerTimer(mHidDemoTimerId, gTmrLowPowerSingleShotMillisTimer_c,
                       TmrSeconds(1), TimerHidMouseCallback, NULL);

            /* Start battery measurements */
            TMR_StartLowPowerTimer(mBatteryMeasurementTimerId, gTmrLowPowerIntervalMillisTimer_c,
                       TmrSeconds(mBatteryLevelReportInterval_c), BatteryMeasurementTimerCallback, NULL);
        }
        break;

    case gConnEvtDisconnected_c:

        /* Unsubscribe client */
        Bas_Unsubscribe();
        Hid_Unsubscribe();

        mPeerDeviceId = gInvalidDeviceId_c;

            if (pConnectionEvent->eventData.disconnectedEvent.reason == gHciConnectionTimeout_c)
        {
                /* Link loss detected*/
            BleApp_Start();
        }
        else
        {
              /* Connection was terminated by peer or application */
            BleApp_Start();
        }
        break;

#if gBondingSupported_d
        case gConnEvtKeysReceived_c:
        {
            /* Copy peer device address information when IRK is used */
            if (pConnectionEvent->eventData.keysReceivedEvent.pKeys->aIrk != NULL)
            {
                mPeerDeviceAddressType = pConnectionEvent->eventData.keysReceivedEvent.pKeys->addressType;
                FLib_MemCpy(maPeerDeviceAddress, pConnectionEvent->eventData.keysReceivedEvent.pKeys->aAddress, sizeof(bleDeviceAddress_t));
            }
        }

        break;

        case gConnEvtPairingComplete_c:
        {
            if (pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful &&
                pConnectionEvent->eventData.pairingCompleteEvent.pairingCompleteData.withBonding)
            {
                /* If a bond is created, write device address in controller’s White List */
                Gap_AddDeviceToWhiteList(mPeerDeviceAddressType, maPeerDeviceAddress);
            }
        }
        break;

        case gConnEvtPairingRequest_c:
        {
            gPairingParameters.centralKeys = pConnectionEvent->eventData.pairingEvent.centralKeys;
            Gap_AcceptPairingRequest(peerDeviceId, &gPairingParameters);
        }
        break;

        case gConnEvtKeyExchangeRequest_c:
        {
            gapSmpKeys_t sentSmpKeys = gSmpKeys;

            if (!(pConnectionEvent->eventData.keyExchangeRequestEvent.requestedKeys & gLtk_c))
            {
                sentSmpKeys.aLtk = NULL;
                /* When the LTK is NULL EDIV and Rand are not sent and will be ignored. */
            }

            if (!(pConnectionEvent->eventData.keyExchangeRequestEvent.requestedKeys & gIrk_c))
            {
                sentSmpKeys.aIrk = NULL;
                /* When the IRK is NULL the Address and Address Type are not sent and will be ignored. */
            }

            if (!(pConnectionEvent->eventData.keyExchangeRequestEvent.requestedKeys & gCsrk_c))
            {
                sentSmpKeys.aCsrk = NULL;
            }

            Gap_SendSmpKeys(peerDeviceId, &sentSmpKeys);
            break;
        }

        case gConnEvtLongTermKeyRequest_c:
        {
            if (pConnectionEvent->eventData.longTermKeyRequestEvent.ediv == gSmpKeys.ediv &&
                pConnectionEvent->eventData.longTermKeyRequestEvent.randSize == gSmpKeys.cRandSize)
            {
                /* EDIV and RAND both matched */
                Gap_ProvideLongTermKey(peerDeviceId, gSmpKeys.aLtk, gSmpKeys.cLtkSize);
            }
            else
            /* EDIV or RAND size did not match */
            {
                Gap_DenyLongTermKey(peerDeviceId);
            }
        }
        break;

#endif

    default:
        break;
    }
}

/*! *********************************************************************************
* \brief        Handles GATT server callback from host stack.
*
* \param[in]    deviceId        Peer device ID.
* \param[in]    pServerEvent    Pointer to gattServerEvent_t.
********************************************************************************** */
static void BleApp_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent)
{
    uint16_t handle;
    uint8_t status;

    switch (pServerEvent->eventType)
    {
        case gEvtAttributeWritten_c:
        {
            handle = pServerEvent->eventData.attributeWrittenEvent.handle;
            status = gAttErrCodeNoError_c;

            if (handle == value_hid_control_point)
            {
                status = Hid_ControlPointHandler(service_hid, pServerEvent->eventData.attributeWrittenEvent.aValue[0]);
            }
               GattServer_SendAttributeWrittenStatus(deviceId, handle, status);
        }
        break;

        case gEvtCharacteristicCccdWritten_c:
        {
            Gap_SaveCccd(
                           deviceId,
                           pServerEvent->eventData.charCccdWrittenEvent.handle,
                           pServerEvent->eventData.charCccdWrittenEvent.newCccd);
        }
        break;
    default:
        break;
    }
}


/*! *********************************************************************************
* \brief        Handles advertising timer callback.
*
* \param[in]    pParam        Calback parameters.
********************************************************************************** */
static void AdvertisingTimerCallback(void * pParam)
{
    /* Stop and restart advertising with new parameters */
    Gap_StopAdvertising();
    switch (mAdvState.advType)
    {
#if gBondingSupported_d
        case fastWhiteListAdvState_c:
        {
            mAdvState.advType = fastAdvState_c;
        }
        break;
#endif
        case fastAdvState_c:
        {
            mAdvState.advType = slowAdvState_c;
        }
        break;

        default:
        break;
    }
    BleApp_Advertise();
}

/*! *********************************************************************************
* \brief        Handles HID Mouse timer callback.
*
* \param[in]    pParam        Calback parameters.
********************************************************************************** */
static void TimerHidMouseCallback(void * pParam)
{
    if ((xAxis < AXIS_MAX) && (yAxis == AXIS_MIN))
    {
        MoveMouseRight(MOUSE_STEP);
        xAxis += MOUSE_STEP;
    }

    if ((xAxis == AXIS_MAX) && (yAxis < AXIS_MAX))
    {
        MoveMouseDown(MOUSE_STEP);
        yAxis += MOUSE_STEP;
    }

    if ((xAxis > AXIS_MIN) && (yAxis == AXIS_MAX))
    {
        MoveMouseLeft(MOUSE_STEP);
        xAxis -= MOUSE_STEP;
    }

    if ((xAxis == AXIS_MIN) && (yAxis > AXIS_MIN))
    {
        MoveMouseUp(MOUSE_STEP);
        yAxis -= MOUSE_STEP;
    }

    /* Start measurements */
    TMR_StartLowPowerTimer(mHidDemoTimerId, gTmrLowPowerSingleShotMillisTimer_c,
           100, TimerHidMouseCallback, NULL);
}


/*! *********************************************************************************
* \brief        Handles battery measurement timer callback.
*
* \param[in]    pParam        Calback parameters.
********************************************************************************** */
static void BatteryMeasurementTimerCallback(void * pParam)
{
    basServiceConfig.batteryLevel = BOARD_GetBatteryLevel();
    Bas_RecordBatteryMeasurement(basServiceConfig.serviceHandle, basServiceConfig.batteryLevel);
}

/*! *********************************************************************************
* \brief        Sends HID Report Over-the-Air.
*
* \param[in]    pReport        Pointer to mouseHidReport_t.
********************************************************************************** */
static void SendReport(mouseHidReport_t *pReport)
{
    hidProtocolMode_t protocolMode;

    /* Toggle Protocol Mode */
    Hid_GetProtocolMode(service_hid, &protocolMode);

    if (protocolMode == gHid_BootProtocolMode_c)
    {
        Hid_SendBootMouseInputReport(hidServiceConfig.serviceHandle, sizeof(mouseHidReport_t), pReport);
    }
    else if (protocolMode == gHid_ReportProtocolMode_c)
    {
        Hid_SendInputReport(hidServiceConfig.serviceHandle, sizeof(mouseHidReport_t), pReport);
    }
}

static void MoveMouseLeft(uint8_t pixels)
{
    mouseHidReport_t mouseReport = {0,0,0};
    mouseReport.xAxis = -pixels;
    SendReport(&mouseReport);
}

static void MoveMouseRight(uint8_t pixels)
{
    mouseHidReport_t mouseReport = {0,0,0};
    mouseReport.xAxis = pixels;
    SendReport(&mouseReport);
}

static void MoveMouseUp(uint8_t pixels)
{
    mouseHidReport_t mouseReport = {0,0,0};
    mouseReport.yAxis = -pixels;
    SendReport(&mouseReport);
}

static void MoveMouseDown(uint8_t pixels)
{
    mouseHidReport_t mouseReport = {0,0,0};
    mouseReport.yAxis = pixels;
    SendReport(&mouseReport);
}
/*! *********************************************************************************
* @}
********************************************************************************** */
