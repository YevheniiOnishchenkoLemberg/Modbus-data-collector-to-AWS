#include "AwsMqtt5SubscriptionManager.h"

AwsMqtt5SubscriptionManager::AwsMqtt5SubscriptionManager() :
    topic("test_topic")
    , payload(ByteCursorFromString("Nothing"))
    , subPacket(std::make_shared<Mqtt5::SubscribePacket>())
    , publishPacket(std::make_shared<Mqtt5::PublishPacket>(
        topic, payload, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE))
    , unsubPacket(std::make_shared<Mqtt5::UnsubscribePacket>())
{
    subscription = Mqtt5::Subscription(topic, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
    subscription.WithNoLocal(false);
    subPacket->WithSubscription(std::move(subscription));

    unsubPacket->WithTopicFilter(topic);
}

std::shared_ptr<Mqtt5::SubscribePacket> AwsMqtt5SubscriptionManager::getSubPacket()
{
    return subPacket;
}

std::shared_ptr<Mqtt5::PublishPacket> AwsMqtt5SubscriptionManager::getPubPacket()
{
    return publishPacket;
}

std::shared_ptr<Mqtt5::UnsubscribePacket> AwsMqtt5SubscriptionManager::getUnsubPacket()
{
    return unsubPacket;
}

Mqtt5::OnSubscribeCompletionHandler AwsMqtt5SubscriptionManager::getOnSubAckHandler()
{
    return std::bind(&AwsMqtt5SubscriptionManager::onSubAck, this, std::placeholders::_1, std::placeholders::_2);
}

Mqtt5::OnPublishCompletionHandler AwsMqtt5SubscriptionManager::getOnPublishCompleteHandler()
{
    return std::bind(&AwsMqtt5SubscriptionManager::onPublishComplete, this, std::placeholders::_1, std::placeholders::_2);
}

Mqtt5::OnUnsubscribeCompletionHandler AwsMqtt5SubscriptionManager::getUnsubHandler()
{
    return std::bind(&AwsMqtt5SubscriptionManager::onUnsubComplete, this, std::placeholders::_1, std::placeholders::_2);
}

bool AwsMqtt5SubscriptionManager::getSubscribeSuccess()
{
    return subscribeSuccess.get_future().get();
}

void AwsMqtt5SubscriptionManager::unsubWait()
{
    unsubscribeFinishedPromise.get_future().wait();
}

void AwsMqtt5SubscriptionManager::setMessage(String msg)
{
    payload = ByteCursorFromString(msg);
    publishPacket = std::make_shared<Mqtt5::PublishPacket>(
        topic, payload, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
}

void AwsMqtt5SubscriptionManager::onSubAck(int error_code, std::shared_ptr<Mqtt5::SubAckPacket> suback)
{
    if (error_code != 0)
    {
        fprintf(
            stdout,
            "MQTT5 Client Subscription failed with error code: (%d)%s\n",
            error_code,
            aws_error_debug_str(error_code));
        subscribeSuccess.set_value(false);
    }
    if (suback != nullptr)
    {
        for (Mqtt5::SubAckReasonCode reasonCode : suback->getReasonCodes())
        {
            if (reasonCode > Mqtt5::SubAckReasonCode::AWS_MQTT5_SARC_UNSPECIFIED_ERROR)
            {
                fprintf(
                    stdout,
                    "MQTT5 Client Subscription failed with server error code: (%d)%s\n",
                    reasonCode,
                    suback->getReasonString()->c_str());
                subscribeSuccess.set_value(false);
                return;
            }
        }
    }
    subscribeSuccess.set_value(true);
}

void AwsMqtt5SubscriptionManager::onPublishComplete(int, std::shared_ptr<Aws::Crt::Mqtt5::PublishResult> result)
{
    if (!result->wasSuccessful())
    {
        fprintf(stdout, "Publish failed with error_code: %d", result->getErrorCode());
    }
    else if (result != nullptr)
    {
        std::shared_ptr<Mqtt5::PubAckPacket> puback =
            std::dynamic_pointer_cast<Mqtt5::PubAckPacket>(result->getAck());
        if (puback->getReasonCode() == 0)
        {
            fprintf(stdout, "Publish Succeed.\n");
        }
        else
        {
            fprintf(
                stdout,
                "PubACK reason code: %d : %s\n",
                puback->getReasonCode(),
                puback->getReasonString()->c_str());
        }
    };
}

void AwsMqtt5SubscriptionManager::onUnsubComplete(int, std::shared_ptr<Mqtt5::UnSubAckPacket>)
{
    unsubscribeFinishedPromise.set_value();
}
