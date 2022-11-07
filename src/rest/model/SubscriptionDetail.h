#ifndef XAPP_MODEL_SubscriptionDetail_H
#define XAPP_MODEL_SubscriptionDetail_H
#include "ModelBase.h"
#include "ActionToBeSetup.h"

namespace xapp {
namespace model {

using EventTriggerDefinition = std::vector<int>;

struct SubscriptionDetail: ModelBase {
    ActionsToBeSetup ActionToBeSetupList;
    EventTriggerDefinition EventTriggers;
    int XappEventInstanceID;
    json validator_schema = R"(
    {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "Subscription detail",
    "properties": {
        "XappEventInstanceId": {
            "type": "integer",
            "minimum": 0,
            "maximum": 255
        },
        "EventTriggers": {
            "description": "Identification of Action",
            "type": "array",
            "items": {
                "type": "integer"
            }
        },
        "ActionToBeSetupList": {
            "type": "array"
        }
    },
    "required": [
                 "XappEventInstanceId",
                 "EventTriggers",
                 "ActionToBeSetupList"
                ],
    "type": "object"
    })"_json;

    virtual json get_validator_schema() const { return validator_schema; }

};

void from_json(const json& j, SubscriptionDetail& ref) {

    std::cout << __PRETTY_FUNCTION__ << std::endl;
    ref.validate_json(j);

    j.at("XappEventInstanceId").get_to(ref.XappEventInstanceID);
    j.at("EventTriggers").get_to(ref.EventTriggers);
    j.at("ActionToBeSetupList").get_to(ref.ActionToBeSetupList);
}

void to_json(json& j, const SubscriptionDetail& ref) {

    j = json {
        {"XappEventInstanceId", ref.XappEventInstanceID},
        {"EventTriggers", ref.EventTriggers},
        {"ActionToBeSetupList", ref.ActionToBeSetupList},
    };
}

using SubscriptionDetailsList = std::vector<SubscriptionDetail>;

} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_SubscriptionDetail_H*/
