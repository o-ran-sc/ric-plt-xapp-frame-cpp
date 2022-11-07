#ifndef XAPP_MODEL_SubscriptionParams_ClientEndpoint_H
#define XAPP_MODEL_SubscriptionParams_ClientEndpoint_H
#include "ModelBase.h"

namespace xapp {
namespace model {

struct SubscriptionParams_ClientEndpoint: ModelBase {
    int HTTPPort;
    std::string Host;
    int RMRPort;
    json validator_schema = R"(
    {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "SubscriptionParams_ClientEndpoint",
    "description": "xApp service address and port",
    "properties": {
        "HTTPPort": {
            "description": "xApp HTTP service address port",
            "type": "integer",
            "minimum": 0,
            "maximum": 65535
        },
        "Host": {
            "description": "xApp service address name like service-ricxapp-xappname-http.ricxapp",
            "type": "string"
        },
        "RMRPort": {
            "description": "xApp RMR service address port",
            "type": "integer",
            "minimum": 0,
            "maximum": 65535
        }
    },
    "required": [
                 "HTTPPort",
                 "Host",
                 "RMRPort"
                 ],
    "type": "object"
    })"_json;

    virtual json get_validator_schema() const { return validator_schema; }
};

void from_json(const json& j, SubscriptionParams_ClientEndpoint& ref) {

    std::cout << __PRETTY_FUNCTION__ << std::endl;
    ref.validate_json(j);
    j.at("HTTPPort").get_to(ref.HTTPPort);
    j.at("Host").get_to(ref.Host);
    j.at("RMRPort").get_to(ref.RMRPort);
}

void to_json(json& j, const SubscriptionParams_ClientEndpoint& ref) {
    j = json {
        {"HTTPPort", ref.HTTPPort},
        {"Host", ref.Host},
        {"RMRPort", ref.RMRPort},
    };
}

} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_SubscriptionParams_ClientEndpoint_H*/
