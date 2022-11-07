#ifndef XAPP_MODEL_SubscriptionParams_E2SubscriptionDirectives_H
#define XAPP_MODEL_SubscriptionParams_E2SubscriptionDirectives_H
#include "ModelBase.h"

namespace xapp {
namespace model {

struct SubscriptionParams_E2SubscriptionDirectives: ModelBase {
    int E2RetryCount;
    int E2TimeoutTimerValue;
    bool RMRRoutingNeeded;
    json validator_schema = R"(
    {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "SubscriptionParams_E2SubscriptionDirectives",
    "description": "Optional. If not set Submgr uses its default values",
    "properties": {
        "E2RetryCount": {
            "description": "How many times E2 subscription request is retried",
            "type": "integer",
            "minimum": 0,
            "maximum": 10
        },
        "E2TimeoutTimerValue": {
            "description": "How long time response is waited from E2 node",
            "type": "integer",
            "minimum": 0,
            "maximum": 10
        },
        "RMRRoutingNeeded": {
            "description": "Subscription needs RMR route from E2Term to xApp",
            "type": "boolean"
        }
    },
    "required": [
                 "E2TimeoutTimerValue",
                 "E2RetryCount",
                 "RMRRoutingNeeded"
                ],
    "type": "object"
    })"_json;

    virtual json get_validator_schema() const { return validator_schema; }
};

void from_json(const json& j, SubscriptionParams_E2SubscriptionDirectives& ref) {

    std::cout << __PRETTY_FUNCTION__ << std::endl;

    j.at("E2RetryCount").get_to(ref.E2RetryCount);
    j.at("E2TimeoutTimerValue").get_to(ref.E2TimeoutTimerValue);
    j.at("RMRRoutingNeeded").get_to(ref.RMRRoutingNeeded);
}

void to_json(json& j, const SubscriptionParams_E2SubscriptionDirectives& ref) {

    j = json {
        {"E2RetryCount", ref.E2RetryCount},
        {"E2TimeoutTimerValue", ref.E2TimeoutTimerValue},
        {"RMRRoutingNeeded", ref.RMRRoutingNeeded},
    };
}

} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_SubscriptionParams_ClientEndpoint_H*/
