#ifndef XAPP_MODEL_SubscriptionData_H
#define XAPP_MODEL_SubscriptionData_H
#include "ModelBase.h"
#include "SubscriptionInstance.h"
namespace xapp {
namespace model {

struct SubscriptionData: ModelBase {
    std::vector<std::string> ClientEndpoint;
    std::string Meid;
    int SubscriptionID;
    SubscriptionInstances m_SubscriptionInstances;

    json validator_schema = R"(
    {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "SubscriptionData",
    "properties": {
        "SubscriptionId": {
            "type": "integer"
        },
        "Meid": {
            "type": "string"
        },
        "ClientEndpoint": {
            "type": "array",
            "items": {
                "type": "string"
            }
        },
        "SubscriptionInstances": {
            "type": "array"
        }
    },
    "required": [
                 "SubscriptionId",
                 "Meid",
                 "ClientEndpoint",
                 "SubscriptionInstances"
                 ],
    "type": "object"
    })"_json;

    virtual json get_validator_schema() const { return validator_schema; }
};

void from_json(const json& j, SubscriptionData& ref) {

    std::cout << __PRETTY_FUNCTION__ << std::endl;
    ref.validate_json(j);

    j.at("SubscriptionId").get_to(ref.SubscriptionID);
    j.at("SubscriptionInstances").get_to(ref.m_SubscriptionInstances);
    j.at("Meid").get_to(ref.Meid);
    j.at("ClientEndpoint").get_to(ref.ClientEndpoint);
}

void to_json(json& j, const SubscriptionData& ref) {

    j = json {
        {"SubscriptionId",ref.SubscriptionID},
        {"Meid",ref.Meid},
        {"ClientEndpoint", ref.ClientEndpoint},
        {"SubscriptionInstances", ref.m_SubscriptionInstances}
    };
}

using SubscriptionList = std::vector<SubscriptionData>;

} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_SubscriptionData_H*/
