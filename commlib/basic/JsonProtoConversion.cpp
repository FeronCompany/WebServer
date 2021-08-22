

#include "JsonProtoConversion.h"

namespace CWSLib
{
    void JsonStringToProto(const std::string& jsonString, google::protobuf::Message& message)
    {
        google::protobuf::util::JsonParseOptions options;
        options.ignore_unknown_fields = true;
        auto status = google::protobuf::util::JsonStringToMessage(jsonString, &message, options);
    }

    void ProtoToJsonString(const google::protobuf::Message& message, std::string& jsonString)
    {
        google::protobuf::util::JsonPrintOptions options;
        //options.add_whitespace = true;
        options.always_print_primitive_fields = true;
        options.preserve_proto_field_names = true;
        auto status = google::protobuf::util::MessageToJsonString(message, &jsonString, options);
    }
}
