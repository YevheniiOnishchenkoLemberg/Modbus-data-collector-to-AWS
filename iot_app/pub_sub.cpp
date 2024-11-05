/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <thread>

#include "src/AwsMqtt5ClientBuilder.h"
#include "src/AwsMqtt5SubscriptionManager.h"

using namespace Aws::Crt;

int main(int argc, char *argv[])
{

    /************************ Setup ****************************/

    AwsMqtt5ClientBuilder clientBuilder;
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = clientBuilder.buildClient();

    if (client == nullptr)
    {
        fprintf(
            stdout, "Failed to Init Mqtt5Client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    /************************ Run the sample ****************************/

    // Start mqtt5 connection session
    if (client->Start())
    {
        if (clientBuilder.isConnected() == false)
        {
            return -1;
        }

        AwsMqtt5SubscriptionManager subManager;
        if (client->Subscribe(subManager.getSubPacket(), subManager.getOnSubAckHandler()))
        {
            // Waiting for subscription completed.
            if (subManager.getSubscribeSuccess() == true)
            {
                fprintf(stdout, "Subscription Success.\n");

                uint32_t publishedCount = 0;
                while (publishedCount < 10)
                {
                    subManager.setMessage("\"" + String{"TEST"} + std::to_string(publishedCount + 1).c_str() + "\"");
                    if (client->Publish(subManager.getPubPacket(), subManager.getOnPublishCompleteHandler()))
                    {
                        ++publishedCount;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }

                {
                    std::unique_lock<std::mutex> receivedLock(clientBuilder.getReceiveMutex());
                    clientBuilder.getReceiveSignal().wait(receivedLock, [&] { return clientBuilder.getReceivedCount() >= 10; });
                }

                // Unsubscribe from the topic.
                if (!client->Unsubscribe(subManager.getUnsubPacket(), subManager.getUnsubHandler()))
                {
                    fprintf(stdout, "Unsubscription failed.\n");
                    exit(-1);
                }
                fprintf(stdout, "Unsubscription Success.\n");
                subManager.unsubWait();
            }
            else
            {
                fprintf(stdout, "Subscription failed.\n");
            }
        }
        else
        {
            fprintf(stdout, "Subscribe operation failed on client.\n");
        }

        // Disconnect
        if (!client->Stop())
        {
            fprintf(stdout, "Failed to disconnect from the mqtt connection. Exiting..\n");
            return -1;
        }
        clientBuilder.stop();
    }
    return 0;
}
