#pragma once

#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <thread>

using namespace Aws::Crt;

class AwsMqtt5ClientBuilder {
public:
    AwsMqtt5ClientBuilder();
    AwsMqtt5ClientBuilder(std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder> builder);

    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> buildClient();
    bool isConnected();
    void stop();

    std::mutex &getReceiveMutex();
    std::condition_variable &getReceiveSignal();
    uint32_t &getReceivedCount();

private:
    std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder> builder;
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions;

    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;
    std::promise<void> disconnectPromise;

    // Variables needed for the sample
    std::mutex receiveMutex;
    std::condition_variable receiveSignal;
    uint32_t receivedCount = 0;

    const String hostName;
    const char *certPath;
    const char *pkeyPath;

private:
    void SetupWithClientConnectionSuccessCallback(const Mqtt5::OnConnectionSuccessEventData &eventData);
    void SetupWithClientConnectionFailureCallback(const Mqtt5::OnConnectionFailureEventData &eventData);
    void SetupWithClientStoppedCallback(const Mqtt5::OnStoppedEventData &);
    void SetupWithClientAttemptingConnectCallback(const Mqtt5::OnAttemptingConnectEventData &);
    void SetupWithClientDisconnectionCallback(const Mqtt5::OnDisconnectionEventData &eventData);
    void SetupWithPublishReceivedCallback(const Mqtt5::PublishReceivedEventData &eventData);

    void setupCallbacks();
};