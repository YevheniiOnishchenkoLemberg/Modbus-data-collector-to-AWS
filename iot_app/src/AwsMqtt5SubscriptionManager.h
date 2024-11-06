#pragma once

#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <thread>
using namespace Aws::Crt;

class AwsMqtt5SubscriptionManager {
private:
    String topic;
    ByteCursor payload;

    std::shared_ptr<Mqtt5::SubscribePacket> subPacket;
    std::shared_ptr<Mqtt5::PublishPacket> publishPacket;
    std::shared_ptr<Mqtt5::UnsubscribePacket> unsubPacket;
    Mqtt5::Subscription subscription;
    std::promise<bool> subscribeSuccess;
    std::promise<void> unsubscribeFinishedPromise;

public:
    AwsMqtt5SubscriptionManager();
    std::shared_ptr<Mqtt5::SubscribePacket> getSubPacket();
    std::shared_ptr<Mqtt5::PublishPacket> getPubPacket();
    std::shared_ptr<Mqtt5::UnsubscribePacket> getUnsubPacket();

    Mqtt5::OnSubscribeCompletionHandler getOnSubAckHandler();
    Mqtt5::OnPublishCompletionHandler getOnPublishCompleteHandler();
    Mqtt5::OnUnsubscribeCompletionHandler getUnsubHandler();


    bool getSubscribeSuccess();
    void unsubWait();

    void setMessage(String msg);
    void setMessageBytes(ByteCursor payload);


private:
    void onSubAck(int error_code, std::shared_ptr<Mqtt5::SubAckPacket> suback);
    void onPublishComplete(int, std::shared_ptr<Aws::Crt::Mqtt5::PublishResult> result);
    void onUnsubComplete(int, std::shared_ptr<Mqtt5::UnSubAckPacket>);
};