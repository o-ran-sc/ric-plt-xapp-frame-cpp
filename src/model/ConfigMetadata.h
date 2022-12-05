#ifndef XAPP_MODEL_ConfigMetadata_H
#define XAPP_MODEL_ConfigMetadata_H

#include "ModelBase.h"

namespace xapp {
	namespace model {

		struct ConfigMetadata : public ModelBase {
			std::string ConfigType;
			std::string XappName;
			nlohmann::json validator_schema = R"(
				{
				"$schema": "http://json-schema.org/draft-07/schema#",
				"title": "ConfigMetadata",
				"properties": {
				    "configType": {
				        "description": "Type of Config",
				        "type": "string",
				        "enum": ["json", "xml", "other"]
				    },
				    "xappName": {
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

			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
		};
		void from_json(const nlohmann::json& j, ConfigMetadata& ref);
		void to_json(nlohmann::json& j, const ConfigMetadata& ref);

	} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_ConfigMetadata_H*/
