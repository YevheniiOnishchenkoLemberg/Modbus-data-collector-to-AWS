#include "AwsMqtt5ClientBuilder.h"

AwsMqtt5ClientBuilder::AwsMqtt5ClientBuilder() : 
    hostName("ajgymapv683a4-ats.iot.eu-central-1.amazonaws.com")
    , certPath("../cert.crt")
    , pkeyPath("../priv_key.key")
{
    builder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(hostName, certPath, pkeyPath));
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
    }

    connectOptions = std::make_shared<Mqtt5::ConnectPacket>();
    connectOptions->WithClientId("Modbus client");
    builder->WithConnectOptions(connectOptions);
    setupCallbacks();
}

AwsMqtt5ClientBuilder::AwsMqtt5ClientBuilder(std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder> builder) : builder(builder.get())
{
}

std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> AwsMqtt5ClientBuilder::buildClient()
{
    return builder->Build();
}

bool AwsMqtt5ClientBuilder::isConnected()
{
    return connectionPromise.get_future().get();
}

void AwsMqtt5ClientBuilder::stop()
{
    stoppedPromise.get_future().wait();
}

std::mutex &AwsMqtt5ClientBuilder::getReceiveMutex()
{
    return receiveMutex;
}

std::condition_variable &AwsMqtt5ClientBuilder::getReceiveSignal()
{
    return receiveSignal;
}

uint32_t &AwsMqtt5ClientBuilder::getReceivedCount()
{
    return receivedCount;
}

void AwsMqtt5ClientBuilder::SetupWithClientConnectionSuccessCallback(const Mqtt5::OnConnectionSuccessEventData &eventData)
{
    fprintf(
        stdout,
        "Mqtt5 Client connection succeed, clientid: %s.\n",
        eventData.negotiatedSettings->getClientId().c_str());
    connectionPromise.set_value(true);
}

void AwsMqtt5ClientBuilder::SetupWithClientConnectionFailureCallback(const Mqtt5::OnConnectionFailureEventData &eventData)
{
    fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
    connectionPromise.set_value(false);
}

void AwsMqtt5ClientBuilder::SetupWithClientStoppedCallback(const Mqtt5::OnStoppedEventData &)
{
    fprintf(stdout, "Mqtt5 Client stopped.\n");
    stoppedPromise.set_value();
}

void AwsMqtt5ClientBuilder::SetupWithClientAttemptingConnectCallback(const Mqtt5::OnAttemptingConnectEventData &)
{
    fprintf(stdout, "Mqtt5 Client attempting connection...\n");
}

void AwsMqtt5ClientBuilder::SetupWithClientDisconnectionCallback(const Mqtt5::OnDisconnectionEventData &eventData)
{
    fprintf(stdout, "Mqtt5 Client disconnection with reason: %s.\n", aws_error_debug_str(eventData.errorCode));
    disconnectPromise.set_value();
}

void AwsMqtt5ClientBuilder::SetupWithPublishReceivedCallback(const Mqtt5::PublishReceivedEventData &eventData)
{
    if (eventData.publishPacket == nullptr)
        return;

    std::lock_guard<std::mutex> lock(receiveMutex);
    ++receivedCount;
    fprintf(stdout, "Publish received on topic %s:", eventData.publishPacket->getTopic().c_str());
    fwrite(eventData.publishPacket->getPayload().ptr, 1, eventData.publishPacket->getPayload().len, stdout);
    fprintf(stdout, "\n");

    for (Mqtt5::UserProperty prop : eventData.publishPacket->getUserProperties())
    {
        fprintf(stdout, "\twith UserProperty:(%s,%s)\n", prop.getName().c_str(), prop.getValue().c_str());
    }
    receiveSignal.notify_all();
}

void AwsMqtt5ClientBuilder::setupCallbacks()
{
    builder->WithClientConnectionSuccessCallback(std::bind(&AwsMqtt5ClientBuilder::SetupWithClientConnectionSuccessCallback, this, std::placeholders::_1));
    builder->WithClientConnectionFailureCallback(std::bind(&AwsMqtt5ClientBuilder::SetupWithClientConnectionFailureCallback, this, std::placeholders::_1));
    builder->WithClientStoppedCallback(std::bind(&AwsMqtt5ClientBuilder::SetupWithClientStoppedCallback, this, std::placeholders::_1));
    builder->WithClientAttemptingConnectCallback(std::bind(&AwsMqtt5ClientBuilder::SetupWithClientAttemptingConnectCallback, this, std::placeholders::_1));
    builder->WithClientDisconnectionCallback(std::bind(&AwsMqtt5ClientBuilder::SetupWithClientDisconnectionCallback, this, std::placeholders::_1));
    builder->WithPublishReceivedCallback(std::bind(&AwsMqtt5ClientBuilder::SetupWithPublishReceivedCallback, this, std::placeholders::_1));
}
