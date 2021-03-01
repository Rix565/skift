#include <libsettings/Protocol.h>

namespace Settings
{

struct MessageHeader
{
    Message::Type type;
    size_t path_length;
    size_t payload_length;
};

Result Protocol::encode_message(IO::Writer &writer, const Message &message)
{
    String path_buffer = "";

    if (message.path)
    {
        IO::MemoryWriter memory;
        IO::Prettifier pretty{memory};
        message.path->prettify(pretty);
        path_buffer = memory.string();
    }

    String payload_buffer = "";

    if (message.payload)
    {
        IO::MemoryWriter memory;
        IO::Prettifier pretty{memory};
        Json::prettify(pretty, *message.payload);
        payload_buffer = memory.string();
    }

    MessageHeader header;
    header.type = message.type;
    header.path_length = path_buffer.length();
    header.payload_length = payload_buffer.length();

    TRY(writer.write(&header, sizeof(MessageHeader)));

    if (path_buffer.length())
    {
        TRY(IO::write_string(writer, path_buffer));
    }

    if (payload_buffer.length())
    {
        TRY(IO::write_string(writer, payload_buffer));
    }

    return SUCCESS;
}

ResultOr<Message> Protocol::decode_message(IO::Reader &reader)
{
    MessageHeader header;

    TRY(reader.read(&header, sizeof(header)));

    Message message;
    message.type = header.type;

    if (header.path_length > 0)
    {
        IO::MemoryWriter memory;
        TRY(IO::copy(reader, memory, header.path_length));
        message.path = Path::parse(memory.string());
    }

    if (header.payload_length > 0)
    {
        IO::MemoryWriter memory;
        TRY(IO::copy(reader, memory, header.payload_length));
        message.payload = Json::parse(memory.string());
    }

    return message;
}

} // namespace Settings
