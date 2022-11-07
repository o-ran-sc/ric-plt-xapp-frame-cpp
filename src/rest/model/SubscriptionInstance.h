#ifndef XAPP_MODEL_SubscriptionInstance_H
#define XAPP_MODEL_SubscriptionInstance_H
#include "ModelBase.h"

namespace xapp {
namespace model {

struct SubscriptionInstance: ModelBase {
    int E2EventInstanceID;
    std::string ErrorCause;
    std::string ErrorSource;
    std::string TimeoutType;
    int XappEventInstanceID;

    json validator_schema = R"(
    {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "SubscriptionInstance",
    "description": "xApp service address and port",
    "properties": {
        "XappEventInstanceId": {
            "type": "integer",
            "minimum": 0,
            "maximum": 65535
        },
        "E2EventInstanceId": {
            "type": "integer",
            "minimum": 0,
            "maximum": 65535
        },
        "ErrorCause": {
            "description": "Descriptive error cause. Empty string when no error.",
            "type": "string"
        },
        "ErrorSource": {
            "description": "Source of error cause.",
            "type": "string",
            "enum": ["SUBMGR", "RTMGR", "DBAAS", "ASN1", "E2Node"]
        },
        "TimeoutType": {
            "description": "Type timeout. xApp should retry if timeout occurs.",
            "type": "string",
            "enum": ["E2-Timeout", "RTMGR-Timeout", "DBAAS-Timeout"]
        }
    },
    "required": [
                 "XappEventInstanceId",
                 "E2EventInstanceId"
                 ],
    "type": "object"
    })"_json;

    virtual json get_validator_schema() const { return validator_schema; }
};

void from_json(const json& j, SubscriptionInstance& ref) {

    std::cout << __PRETTY_FUNCTION__ << std::endl;
    ref.validate_json(j);

    j.at("XappEventInstanceId").get_to(ref.XappEventInstanceID);
    j.at("E2EventInstanceId").get_to(ref.E2EventInstanceID);
    j.at("ErrorCause").get_to(ref.ErrorCause);
    j.at("ErrorSource").get_to(ref.ErrorSource);
    j.at("TimeoutType").get_to(ref.TimeoutType);
}

void to_json(json& j, const SubscriptionInstance& ref) {

    j = json {
        {"XappEventInstanceId", ref.XappEventInstanceID},
        {"E2EventInstanceId", ref.E2EventInstanceID},
        {"ErrorCause", ref.ErrorCause},
        {"ErrorSource", ref.ErrorSource},
        {"TimeoutType", ref.TimeoutType}
    };
}

using SubscriptionInstances = std::vector<SubscriptionInstance>;

} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_SubscriptionInstance_H*/
