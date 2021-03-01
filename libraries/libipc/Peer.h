#pragma once

#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io_new/Connection.h>

#include <libutils/Callback.h>
#include <libutils/ResultOr.h>

namespace ipc
{

template <typename Protocol>
class Peer
{
private:
    System::Connection _connection;
    OwnPtr<Notifier> _notifier = nullptr;

public:
    bool connected()
    {
        return System::valid(_connection);
    }

    Peer(System::Connection &&connection)
        : _connection{move(connection)}
    {
        _notifier = own<Notifier>(_connection, POLL_READ, [this]() {
            auto result_or_message = Protocol::decode_message(_connection);

            if (result_or_message)
            {
                handle_message(*result_or_message);
            }
            else
            {
                close();
            }
        });
    }

    virtual ~Peer()
    {
        close();
    }

    Result send(const Protocol::Message &message)
    {
        auto result = Protocol::encode_message(_connection, message);

        if (result != SUCCESS)
        {
            close();
        }

        return result;
    }

    ResultOr<typename Protocol::Message> receive()
    {
        auto result_or_message = Protocol::decode_message(_connection);

        if (!result_or_message.success())
        {
            close();
        }

        return result_or_message;
    }

    template <typename TPredicate>
    ResultOr<typename Protocol::Message> send_and_wait_for(Protocol::Message message, TPredicate predicate)
    {
        auto send_result = send(message);

        if (send_result != SUCCESS)
        {
            return send_result;
        }

        auto result_or_message = receive();

        while (result_or_message && !predicate(*result_or_message))
        {
            handle_message(*result_or_message);

            auto result_or_message = receive();
        }

        return result_or_message;
    }

    void close()
    {
        if (System::valid(_connection))
        {
            handle_disconnect();

            _notifier = nullptr;
            System::close(_connection);
        }
    }

    virtual void handle_message(const Protocol::Message &) {}
    virtual void handle_disconnect() {}
};

} // namespace ipc
