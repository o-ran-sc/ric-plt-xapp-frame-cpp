#ifndef XAPP_MODEL_ConfigMetadata_H
#define XAPP_MODEL_ConfigMetadata_H

#include "ModelBase.h"

namespace xapp {
namespace model {

struct ConfigMetadata: ModelBase {
    std::string ConfigType;
    std::string XappName;
    json validator_schema = R"(
    {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "ConfigMetadata",
    "properties": {
        "ConfigType": {
            "description": "Type of Config",
            "type": "string",
            "enum": ["json", "xml", "other"]
        },
        "XappName": {
            "description": "Name of xApp",
            "type": "string"
        }
    },
    "required": [
                 "ConfigType",
                 "XappName"
                 ],
    "type": "object"
    })"_json;

    virtual json get_validator_schema() const { return validator_schema; }
};

void from_json(const json& j, ConfigMetadata& ref) {

    std::cout << __func__ << " ConfigMetadata " << std::endl;
    ref.validate_json(j);
    j.at("ConfigType").get_to(ref.ConfigType);
    j.at("XappName").get_to(ref.XappName);
}

void to_json(json& j, const ConfigMetadata& ref) {
    j = json {
        {"ConfigType",ref.ConfigType},
        {"XappName", ref.XappName}
    };
}

} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_ConfigMetadata_H*/
