#pragma once

#include <libsystem/io_new/Socket.h>
#include <libipc/Peer.h>
#include <libsettings/Protocol.h>

namespace Settings
{

class ServerConnection : public ipc::Peer<Protocol>
{
public:
    Callback<void(const Path &path, const Json::Value &value)> on_notify;

    static ResultOr<OwnPtr<ServerConnection>> open()
    {
        auto connect_result = System::Socket::connect("/Session/settings.ipc");
        TRY(connect_result);
        return own<ServerConnection>(connect_result.take_value());
    }

    ServerConnection(System::Connection &&connection)
        : Peer(move(connection))
    {
    }

    ResultOr<Message>
    request(Message message, Message::Type expected)
    {
        return send_and_wait_for(message, [&](const Message &incoming) { return incoming.type == expected; });
    }

    void handle_message(const Message &message) override
    {
        if (message.type == Message::SERVER_NOTIFY)
        {
            on_notify(*message.path, *message.payload);
        }
    }
};

} // namespace Settings
