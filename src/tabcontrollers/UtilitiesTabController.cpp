#include "UtilitiesTabController.h"
#include <QQuickWindow>
#include <QApplication>
#include "../overlaycontroller.h"
#include <chrono>
#include <thread>
#ifdef _WIN32
#    include "keyboardinput/KeyboardInputWindows.h"
#else
#    include "keyboardinput/KeyboardInputDummy.h"
#endif

// application namespace
namespace advsettings
{
constexpr auto steamDesktopOverlaykey = "valve.steam.desktop";

void UtilitiesTabController::initStage1()
{
#ifdef _WIN32
    keyboardInput.reset( new KeyboardInputWindows() );
#else
    keyboardInput.reset( new KeyboardInputDummy() );
#endif
    auto settings = OverlayController::appSettings();
    settings->beginGroup( "utilitiesSettings" );
    auto qAlarmEnabled = settings->value( "alarmEnabled", m_alarmEnabled );
    auto qAlarmIsModal = settings->value( "alarmIsModal", m_alarmIsModal );
    auto qAlarmHour = settings->value( "alarmHour", 0 );
    auto qAlarmMinute = settings->value( "alarmMinute", 0 );
    auto qDesktopWidth = settings->value( "desktopWidth", 4.0 );
    settings->endGroup();
    m_alarmEnabled = qAlarmEnabled.toBool();
    m_alarmIsModal = qAlarmIsModal.toBool();
    m_alarmTime = QTime( qAlarmHour.toInt(), qAlarmMinute.toInt() );

    setUpDesktopOverlay( qDesktopWidth.toFloat() );
}

void UtilitiesTabController::initStage2( OverlayController* var_parent,
                                         QQuickWindow* var_widget )
{
    this->m_parent = var_parent;
    this->m_widget = var_widget;
}

void UtilitiesTabController::sendKeyboardInput( QString input )
{
    keyboardInput->sendKeyboardInput( input );
}

void UtilitiesTabController::sendKeyboardEnter()
{
    keyboardInput->sendKeyboardEnter();
}

void UtilitiesTabController::sendKeyboardAltEnter()
{
    keyboardInput->sendKeyboardAltEnter();
}

void UtilitiesTabController::sendKeyboardAltTab()
{
    keyboardInput->sendKeyboardAltTab();
}

void UtilitiesTabController::sendKeyboardBackspace( const int count )
{
    keyboardInput->sendKeyboardBackspace( count );
}

void UtilitiesTabController::sendMediaNextSong()
{
    keyboardInput->sendMediaNextSong();
}
void UtilitiesTabController::sendMediaPreviousSong()
{
    keyboardInput->sendMediaPreviousSong();
}
void UtilitiesTabController::sendMediaPausePlay()
{
    keyboardInput->sendMediaPausePlay();
}
void UtilitiesTabController::sendMediaStopSong()
{
    keyboardInput->sendMediaStopSong();
}

bool UtilitiesTabController::alarmEnabled() const
{
    return m_alarmEnabled;
}

bool UtilitiesTabController::alarmIsModal() const
{
    return m_alarmIsModal;
}

int UtilitiesTabController::alarmTimeHour() const
{
    return m_alarmTime.hour();
}

int UtilitiesTabController::alarmTimeMinute() const
{
    return m_alarmTime.minute();
}

void UtilitiesTabController::setAlarmEnabled( bool enabled, bool notify )
{
    if ( m_alarmEnabled != enabled )
    {
        m_alarmEnabled = enabled;
        auto settings = OverlayController::appSettings();
        settings->beginGroup( "utilitiesSettings" );
        settings->setValue( "alarmEnabled", m_alarmEnabled );
        settings->endGroup();
        settings->sync();
        m_alarmLastCheckTime = QTime();
        if ( notify )
        {
            emit alarmEnabledChanged( m_alarmEnabled );
        }
    }
}

void UtilitiesTabController::setAlarmIsModal( bool modal, bool notify )
{
    if ( m_alarmIsModal != modal )
    {
        m_alarmIsModal = modal;
        auto settings = OverlayController::appSettings();
        settings->beginGroup( "utilitiesSettings" );
        settings->setValue( "alarmIsModal", m_alarmIsModal );
        settings->endGroup();
        settings->sync();
        if ( notify )
        {
            emit alarmIsModalChanged( m_alarmIsModal );
        }
    }
}

void UtilitiesTabController::setAlarmTimeHour( int hour, bool notify )
{
    if ( m_alarmTime.hour() != hour )
    {
        if ( m_alarmTime.isValid() )
        {
            m_alarmTime = QTime( hour, m_alarmTime.minute() );
        }
        else
        {
            m_alarmTime = QTime( hour, 0 );
        }
        auto settings = OverlayController::appSettings();
        settings->beginGroup( "utilitiesSettings" );
        settings->setValue( "alarmHour", m_alarmTime.hour() );
        settings->setValue( "alarmMinute", m_alarmTime.minute() );
        settings->endGroup();
        settings->sync();
        m_alarmLastCheckTime = QTime();
        if ( notify )
        {
            emit alarmTimeHourChanged( hour );
        }
    }
}

void UtilitiesTabController::setAlarmTimeMinute( int min, bool notify )
{
    if ( m_alarmTime.minute() != min )
    {
        if ( m_alarmTime.isValid() )
        {
            m_alarmTime = QTime( m_alarmTime.hour(), min );
        }
        else
        {
            m_alarmTime = QTime( 0, min );
        }
        auto settings = OverlayController::appSettings();
        settings->beginGroup( "utilitiesSettings" );
        settings->setValue( "alarmHour", m_alarmTime.hour() );
        settings->setValue( "alarmMinute", m_alarmTime.minute() );
        settings->endGroup();
        settings->sync();
        m_alarmLastCheckTime = QTime();
        if ( notify )
        {
            emit alarmTimeMinuteChanged( min );
        }
    }
}

void UtilitiesTabController::setAlarmTimeToCurrentTime()
{
    auto now = QTime::currentTime();
    setAlarmTimeHour( now.hour() );
    setAlarmTimeMinute( now.minute() );
}

void UtilitiesTabController::modAlarmTimeHour( int value, bool notify )
{
    auto h = m_alarmTime.hour();
    m_alarmTime = m_alarmTime.addSecs( value * 60 * 60 );
    auto settings = OverlayController::appSettings();
    settings->beginGroup( "utilitiesSettings" );
    settings->setValue( "alarmHour", m_alarmTime.hour() );
    settings->setValue( "alarmMinute", m_alarmTime.minute() );
    settings->endGroup();
    settings->sync();
    if ( h != m_alarmTime.hour() )
    {
        m_alarmLastCheckTime = QTime();
        if ( notify )
        {
            emit alarmTimeHourChanged( m_alarmTime.hour() );
        }
    }
}

void UtilitiesTabController::modAlarmTimeMinute( int value, bool notify )
{
    auto h = m_alarmTime.hour();
    auto m = m_alarmTime.minute();
    m_alarmTime = m_alarmTime.addSecs( value * 60 );
    auto settings = OverlayController::appSettings();
    settings->beginGroup( "utilitiesSettings" );
    settings->setValue( "alarmHour", m_alarmTime.hour() );
    settings->setValue( "alarmMinute", m_alarmTime.minute() );
    settings->endGroup();
    settings->sync();
    if ( m != m_alarmTime.minute() || h != m_alarmTime.hour() )
    {
        m_alarmLastCheckTime = QTime();
        if ( notify )
        {
            if ( h != m_alarmTime.hour() )
            {
                emit alarmTimeHourChanged( m_alarmTime.hour() );
            }
            if ( m != m_alarmTime.minute() )
            {
                emit alarmTimeMinuteChanged( m_alarmTime.minute() );
            }
        }
    }
}

QString getBatteryIconPath( int batteryState )
{
    QString batteryPrefix = "/res/img/battery/battery_";
    return QStandardPaths::locate(
        QStandardPaths::AppDataLocation,
        batteryPrefix + QString::number( batteryState ) + ".png" );
}

vr::VROverlayHandle_t createBatteryOverlay( vr::TrackedDeviceIndex_t index )
{
    vr::VROverlayHandle_t handle = vr::k_ulOverlayHandleInvalid;
    std::string batteryKey = std::string( application_strings::applicationKey )
                             + ".battery." + std::to_string( index );
    vr::VROverlayError overlayError = vr::VROverlay()->CreateOverlay(
        batteryKey.c_str(), batteryKey.c_str(), &handle );
    if ( overlayError == vr::VROverlayError_None )
    {
        QString batteryIconPath = getBatteryIconPath( 0 );
        if ( QFile::exists( batteryIconPath ) )
        {
            vr::VROverlay()->SetOverlayFromFile(
                handle, batteryIconPath.toStdString().c_str() );
            vr::VROverlay()->SetOverlayWidthInMeters( handle, 0.05f );
            vr::HmdMatrix34_t notificationTransform
                = { { { 1.0f, 0.0f, 0.0f, 0.00f },
                      { 0.0f, -1.0f, 0.0f, 0.01f },
                      { 0.0f, 0.0f, -1.0f, -0.013f } } };
            vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(
                handle, index, &notificationTransform );
            LOG( INFO ) << "Created battery overlay for device " << index;
        }
        else
        {
            LOG( ERROR ) << "Could not find battery icon \"" << batteryIconPath
                         << "\"";
        }
    }
    else
    {
        LOG( ERROR ) << "Could not create ptt notification overlay: "
                     << vr::VROverlay()->GetOverlayErrorNameFromEnum(
                            overlayError );
    }

    return handle;
}

void UtilitiesTabController::eventLoopTick()
{
    if ( settingsUpdateCounter >= k_utilitiesSettingsUpdateCounter )
    {
        vr::VROverlayHandle_t pOverlayHandle;
        auto error = vr::VROverlay()->FindOverlay( steamDesktopOverlaykey,
                                                   &pOverlayHandle );
        if ( error == vr::VROverlayError_None )
        {
            if ( !m_steamDesktopOverlayAvailable )
            {
                m_steamDesktopOverlayAvailable = true;
                emit steamDesktopOverlayAvailableChanged(
                    m_steamDesktopOverlayAvailable );
            }
            float width;
            error = vr::VROverlay()->GetOverlayWidthInMeters( pOverlayHandle,
                                                              &width );
            if ( error != vr::VROverlayError_None )
            {
                LOG( ERROR )
                    << "Could not read overlay width of \""
                    << steamDesktopOverlaykey << "\": "
                    << vr::VROverlay()->GetOverlayErrorNameFromEnum( error );
            }
            else
            {
                setSteamDesktopOverlayWidth( width, true, false );
            }
        }
        else
        {
            if ( m_steamDesktopOverlayAvailable )
            {
                m_steamDesktopOverlayAvailable = false;
                emit steamDesktopOverlayAvailableChanged(
                    m_steamDesktopOverlayAvailable );
            }
        }

        if ( m_alarmEnabled && m_alarmTime.isValid() )
        {
            auto now = QTime::currentTime();
            if ( m_alarmLastCheckTime.isValid()
                 && m_alarmLastCheckTime <= m_alarmTime && m_alarmTime <= now )
            {
                setAlarmEnabled( false );
                char alarmMessageBuffer[1024];
                std::snprintf( alarmMessageBuffer,
                               1024,
                               "The alarm at %02i:%02i went off.",
                               alarmTimeHour(),
                               alarmTimeMinute() );
                if ( m_alarmIsModal )
                {
                    std::thread messageThread(
                        []( UtilitiesTabController* parent,
                            std::string message ) {
                            auto res = vr::VROverlay()->ShowMessageOverlay(
                                message.c_str(),
                                "Alarm Clock",
                                "Ok",
                                "+15 min" );
                            if ( res
                                 == vr::VRMessageOverlayResponse_ButtonPress_1 )
                            {
                                parent->setAlarmTimeToCurrentTime();
                                parent->modAlarmTimeMinute( 15 );
                                parent->setAlarmEnabled( true );
                            }
                            else if (
                                res
                                >= vr::VRMessageOverlayResponse_CouldntFindSystemOverlay )
                            {
                                static const char* errorMessages[]
                                    = { "CouldntFindSystemOverlay",
                                        "CouldntFindOrCreateClientOverlay",
                                        "ApplicationQuit" };
                                int errorCode
                                    = res
                                      - vr::VRMessageOverlayResponse_CouldntFindSystemOverlay;
                                if ( res < 3 )
                                {
                                    LOG( ERROR )
                                        << "Could not create Alarm Overlay: "
                                        << errorMessages[errorCode];
                                }
                                else
                                {
                                    LOG( ERROR ) << "Could not create Alarm "
                                                    "Overlay: Unknown Error";
                                }
                            }
                        },
                        this,
                        std::string( alarmMessageBuffer ) );
                    messageThread.detach();
                }
                else
                {
                    vr::VRNotificationId notificationId;
                    vr::EVRInitError eError;
                    vr::IVRNotifications* vrnotification
                        = static_cast<vr::IVRNotifications*>(
                            vr::VR_GetGenericInterface(
                                vr::IVRNotifications_Version, &eError ) );
                    if ( eError != vr::VRInitError_None )
                    {
                        LOG( ERROR )
                            << "Error while getting IVRNotifications interface"
                            << vr::VR_GetVRInitErrorAsEnglishDescription(
                                   eError );
                        vrnotification = nullptr;
                    }
                    else
                    {
                        vr::NotificationBitmap_t* messageIconPtr = nullptr;
                        /*
                        // Can i even create this object on the stack, who takes
                        ownership? vr::NotificationBitmap_t messageIcon;

                        // First get image width and height
                        vr::VROverlay()->GetOverlayImageData(parent->overlayThumbnailHandle(),
                        nullptr, 0, (uint32_t*)&messageIcon.m_nWidth,
                        (uint32_t*)&messageIcon.m_nHeight);

                        messageIcon.m_nBytesPerPixel = 4;
                        unsigned bufferSize =
                        messageIcon.m_nWidth*messageIcon.m_nHeight*messageIcon.m_nBytesPerPixel;

                        // Can I delete this buffer after this section? Who
                        takes ownership? std::unique_ptr<char> imageBuffer;
                        imageBuffer.reset(new char[bufferSize]);
                        messageIcon.m_pImageData = imageBuffer.get();

                        // Get image data
                        auto iconError =
                        vr::VROverlay()->GetOverlayImageData(parent->overlayThumbnailHandle(),
                        messageIcon.m_pImageData, bufferSize,
                        (uint32_t*)&messageIcon.m_nWidth,
                        (uint32_t*)&messageIcon.m_nHeight); if (iconError !=
                        vr::VROverlayError_None) { LOG(ERROR) << "Error while
                        getting message overlay icon: " <<
                        vr::VROverlay()->GetOverlayErrorNameFromEnum(iconError);
                        } else {
                            messageIconPtr = &messageIcon;
                        }
                        */
                        auto nError = vrnotification->CreateNotification(
                            m_parent->overlayHandle(),
                            666,
                            vr::EVRNotificationType_Transient,
                            alarmMessageBuffer,
                            vr::EVRNotificationStyle_Application,
                            messageIconPtr,
                            &notificationId );
                        if ( nError != vr::VRNotificationError_OK )
                        {
                            LOG( ERROR )
                                << "Error while creating notification: "
                                << nError;
                            vrnotification = nullptr;
                        }
                    }
                }
            }
            m_alarmLastCheckTime = now;
        }

        // attach battery overlay to all tracked devices that aren't a
        // controller or hmd
        for ( vr::TrackedDeviceIndex_t i = 0; i < vr::k_unMaxTrackedDeviceCount;
              i++ )
        {
            vr::ETrackedDeviceClass deviceClass
                = vr::VRSystem()->GetTrackedDeviceClass( i );
            if ( deviceClass
                 == vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker )
            {
                if ( m_batteryOverlayHandles[i] == 0 )
                {
                    LOG( INFO ) << "Creating battery overlay for device " << i;
                    m_batteryOverlayHandles[i] = createBatteryOverlay( i );
                    m_batteryVisible[i] = true;
                }

                bool shouldShow = vr::VROverlay()->IsDashboardVisible();

                if ( shouldShow != m_batteryVisible[i] )
                {
                    if ( shouldShow )
                    {
                        vr::VROverlay()->ShowOverlay(
                            m_batteryOverlayHandles[i] );
                    }
                    else
                    {
                        vr::VROverlay()->HideOverlay(
                            m_batteryOverlayHandles[i] );
                    }
                    m_batteryVisible[i] = shouldShow;
                }

                bool hasBatteryStatus
                    = vr::VRSystem()->GetBoolTrackedDeviceProperty(
                        i,
                        vr::ETrackedDeviceProperty::
                            Prop_DeviceProvidesBatteryStatus_Bool );
                if ( hasBatteryStatus )
                {
                    float battery
                        = vr::VRSystem()->GetFloatTrackedDeviceProperty(
                            i,
                            vr::ETrackedDeviceProperty::
                                Prop_DeviceBatteryPercentage_Float );
                    int batteryState = static_cast<int>(
                        ceil( static_cast<double>( battery * 5 ) ) );

                    if ( batteryState != m_batteryState[i] )
                    {
                        LOG( INFO )
                            << "Updating battery overlay for device " << i
                            << " to " << batteryState << "(" << battery << ")"
                            << QString::number( m_batteryOverlayHandles[i] );
                        QString batteryIconPath
                            = getBatteryIconPath( batteryState );
                        if ( QFile::exists( batteryIconPath ) )
                        {
                            vr::VROverlay()->SetOverlayFromFile(
                                m_batteryOverlayHandles[i],
                                batteryIconPath.toStdString().c_str() );
                        }
                        else
                        {
                            LOG( ERROR ) << "Could not find battery icon \""
                                         << batteryIconPath << "\"";
                        }
                        m_batteryState[i] = batteryState;
                    }
                }
            }
        }

        settingsUpdateCounter = 0;
    }
    else
    {
        settingsUpdateCounter++;
    }
}

bool steamDesktopOverlayExists()
{
    vr::VROverlayHandle_t pOverlayHandle = 0;
    auto error = vr::VROverlay()->FindOverlay( steamDesktopOverlaykey,
                                               &pOverlayHandle );
    if ( error != vr::VROverlayError_None )
    {
        LOG( DEBUG ) << "Could not find overlay \"" << steamDesktopOverlaykey
                     << "\": "
                     << vr::VROverlay()->GetOverlayErrorNameFromEnum( error );
        return false;
    }
    return true;
}

void UtilitiesTabController::setUpDesktopOverlay( float desktopWidthInMeters )
{
    auto setOverlayVariablesSuccess = [this, desktopWidthInMeters]() {
        m_steamDesktopOverlayAvailable = true;
        setSteamDesktopOverlayWidth( desktopWidthInMeters, true, true );
        LOG( INFO ) << "Found overlay \"" << steamDesktopOverlaykey << "\".";
    };

    if ( steamDesktopOverlayExists() )
    {
        setOverlayVariablesSuccess();
        return;
    }

    constexpr auto timeBetweenAdditionalAttempts = std::chrono::seconds( 1 );
    constexpr auto maxAttempts = 7;
    for ( int attempts = 0; attempts < maxAttempts; ++attempts )
    {
        std::this_thread::sleep_for( timeBetweenAdditionalAttempts );
        if ( steamDesktopOverlayExists() )
        {
            setOverlayVariablesSuccess();
            return;
        }
    }
    LOG( INFO ) << "Could not find overlay \"" << steamDesktopOverlaykey
                << "\" after " << maxAttempts << " extra attempts.";
}

bool UtilitiesTabController::steamDesktopOverlayAvailable() const
{
    return m_steamDesktopOverlayAvailable;
}

float UtilitiesTabController::steamDesktopOverlayWidth() const
{
    return m_steamDesktopOverlayWidth;
}

void UtilitiesTabController::setSteamDesktopOverlayWidth( float width,
                                                          bool notify,
                                                          bool notifyOpenVr )
{
    if ( std::abs( m_steamDesktopOverlayWidth - width ) > 0.01f )
    {
        m_steamDesktopOverlayWidth = width;

        auto settings = OverlayController::appSettings();
        settings->beginGroup( "utilitiesSettings" );
        settings->setValue( "desktopWidth", m_steamDesktopOverlayWidth );
        settings->endGroup();

        if ( notifyOpenVr )
        {
            vr::VROverlayHandle_t pOverlayHandle;
            auto error = vr::VROverlay()->FindOverlay( steamDesktopOverlaykey,
                                                       &pOverlayHandle );
            if ( error != vr::VROverlayError_None )
            {
                LOG( ERROR )
                    << "Could not find overlay \"" << steamDesktopOverlaykey
                    << "\": "
                    << vr::VROverlay()->GetOverlayErrorNameFromEnum( error );
            }
            else
            {
                error = vr::VROverlay()->SetOverlayWidthInMeters(
                    pOverlayHandle, m_steamDesktopOverlayWidth );
                if ( error != vr::VROverlayError_None )
                {
                    LOG( ERROR )
                        << "Could not modify overlay width of \""
                        << steamDesktopOverlaykey << "\": "
                        << vr::VROverlay()->GetOverlayErrorNameFromEnum(
                               error );
                }
            }
        }
        if ( notify )
        {
            emit steamDesktopOverlayWidthChanged( m_steamDesktopOverlayWidth );
        }
    }
}

} // namespace advsettings
