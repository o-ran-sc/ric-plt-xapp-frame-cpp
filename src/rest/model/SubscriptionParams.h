#ifndef XAPP_MODEL_SubscriptionParams_H
#define XAPP_MODEL_SubscriptionParams_H
#include "ModelBase.h"
#include "SubscriptionDetail.h"
#include "SubscriptionParams_ClientEndpoint.h"
#include "SubscriptionParams_E2SubscriptionDirectives.h"

namespace xapp {
namespace model {

struct SubscriptionParams: public ModelBase {

    SubscriptionParams_ClientEndpoint ClientEndpoint;
    SubscriptionParams_E2SubscriptionDirectives E2SubscriptionDirectives;
    std::string Meid;
    int RANFunctionID;
    std::string SubscriptionID;
    SubscriptionDetailsList m_SubscriptionDetailsList;

    json validator_schema = R"(
    {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "SubscriptionParams",
    "properties": {
        "SubscriptionId": {
            "description": "Optional subscription ID '(Submgr allocates if not given)'",
            "type": "string"
        },
        "Meid": {
            "type": "string"
        },
        "RANFunctionId": {
            "type": "integer",
            "minimum": 0,
            "maximum": 4095
        }
    },
    "required": [
                 "ClientEndpoint",
                 "Meid",
                 "RANFunctionId",
                 "SubscriptionDetails"
                ],
    "type": "object"
    })"_json;

    virtual json get_validator_schema() const { return validator_schema; }

};

void from_json(const json& j, SubscriptionParams& ref) {

    std::cout << __PRETTY_FUNCTION__ << std::endl;
    ref.validate_json(j);
    j.at("ClientEndpoint").get_to(ref.ClientEndpoint);

    if (j.contains("E2SubscriptionDirectives"))
    {
        j.at("E2SubscriptionDirectives").get_to(ref.E2SubscriptionDirectives);
    }

    if (j.contains("SubscriptionId"))
    {
        j.at("SubscriptionId").get_to(ref.SubscriptionID);
    }

    j.at("Meid").get_to(ref.Meid);
    j.at("RANFunctionId").get_to(ref.RANFunctionID);
    j.at("SubscriptionDetails").get_to(ref.m_SubscriptionDetailsList);
}

void to_json(json& j, const SubscriptionParams& ref) {

    j = json {
        {"ClientEndpoint", ref.ClientEndpoint},
        {"E2SubscriptionDirectives", ref.E2SubscriptionDirectives},
        {"Meid", ref.Meid},
        {"RANFunctionId", ref.RANFunctionID},
        {"SubscriptionId", ref.SubscriptionID},
        {"SubscriptionDetails", ref.m_SubscriptionDetailsList},
    };
}

} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_SubscriptionParams_H*/
