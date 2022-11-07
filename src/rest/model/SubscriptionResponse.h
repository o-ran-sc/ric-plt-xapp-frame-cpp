#ifndef XAPP_MODEL_SubscriptionResponse_H
#define XAPP_MODEL_SubscriptionResponse_H
#include "ModelBase.h"
#include "SubscriptionInstance.h"

namespace xapp {
namespace model {

struct SubscriptionResponse: ModelBase {
    int SubscriptionID;
    SubscriptionInstances m_SubscriptionInstances;

    json validator_schema = R"(
    {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "SubscriptionResponse",
    "properties": {
        "SubscriptionId": {
            "description": "Indentification of Subscription",
            "type": "integer"
        },
        "SubscriptionInstances": {
            "description": "List of Subscription Instance",
            "type": "array"
        }
    },
    "required": [
                 "SubscriptionId",
                 "SubscriptionInstances"
                 ],
    "type": "object"
    })"_json;

    virtual json get_validator_schema() const { return validator_schema; }
};

void from_json(const json& j, SubscriptionResponse& ref) {

    std::cout << __PRETTY_FUNCTION__ << std::endl;
    ref.validate_json(j);

    j.at("SubscriptionId").get_to(ref.SubscriptionID);
    j.at("SubscriptionInstances").get_to(ref.m_SubscriptionInstances);
}

void to_json(json& j, const SubscriptionResponse& ref) {

    j = json {
        {"SubscriptionId",ref.SubscriptionID},
        {"SubscriptionInstances", ref.m_SubscriptionInstances}
    };
}

} /*model*/
} /*xapp*/
#endif /*XAPP_MODEL_SubscriptionResponse_H*/
