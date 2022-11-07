#ifndef XAPP_MODEL_ActionToBeSetup_H
#define XAPP_MODEL_ActionToBeSetup_H
#include "ModelBase.h"
#include "SubsequentAction.h"

namespace xapp {
namespace model {

using namespace xapp::model;
using ActionDefinition = std::vector<int>;

struct ActionToBeSetup: ModelBase {
    ActionDefinition m_ActionDefinition;
    int ActionID;
    std::string ActionType;
    SubsequentAction m_SubsequentAction;

    json validator_schema = R"(
    {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "SubsequentAction",
    "properties": {
        "ActionDefinition": {
            "description": "Action Definition",
            "type": "array",
            "items": {
                "type": "integer"
            }
        },
        "ActionID": {
            "description": "Identification of Action",
            "type": "integer",
            "minimum": 0,
            "maximum": 255
        },
        "ActionType": {
            "description": "Type of Action",
            "type": "string",
            "enum": ["policy", "insert", "report"]
        },
        "SubsequentAction": {
            "description": "Subsequent Action",
            "type": "object"
        }
    },
    "required": [
                 "ActionDefinition",
                 "ActionID",
                 "ActionType",
                 "SubsequentAction"
                ],
    "type": "object"
    })"_json;

    virtual json get_validator_schema() const { return validator_schema; }
};

void from_json(const json& j, ActionToBeSetup& ref) {

    std::cout << __PRETTY_FUNCTION__ << std::endl;
    ref.validate_json(j);
    j.at("ActionDefinition").get_to(ref.m_ActionDefinition);
    j.at("ActionID").get_to(ref.ActionID);
    j.at("ActionType").get_to(ref.ActionType);
    j.at("SubsequentAction").get_to(ref.m_SubsequentAction);

}

void to_json(json& j, const ActionToBeSetup& ref) {

    j = json {
        {"ActionDefinition", ref.m_ActionDefinition},
        {"ActionID", ref.ActionID},
        {"ActionType", ref.ActionType},
        {"SubsequentAction", ref.m_SubsequentAction}
    };
}

using ActionsToBeSetup = std::vector<ActionToBeSetup>;

} /*namespace model*/
} /*namespace xapp*/

#endif /* XAPP_MODEL_ActionToBeSetup_H */

