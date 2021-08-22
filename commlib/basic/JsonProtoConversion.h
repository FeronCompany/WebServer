

#pragma once

#include <string>
#include <google/protobuf/util/json_util.h>

namespace CWSLib
{
    void JsonStringToProto(const std::string& jsonString, google::protobuf::Message& message);
    void ProtoToJsonString(const google::protobuf::Message& message, std::string& jsonString);
}
