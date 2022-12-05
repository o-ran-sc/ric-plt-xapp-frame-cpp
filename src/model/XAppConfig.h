#ifndef XAPP_MODEL_XAppConfig_H
#define XAPP_MODEL_XAppConfig_H
#include"ConfigMetadata.h"

namespace xapp {
	namespace model {
		struct  XAppConfig : public ModelBase {
			ConfigMetadata m_ConfigMetadata;
			nlohmann::json config;// = nlohmann::json::object();
			nlohmann::json validator_schema = R"(
				{
				"$schema": "http://json-schema.org/draft-07/schema#",
				"title": "XAppConfig",
				"properties": {
				    
				    "metadata": {
				        "type": "object"
				    },
					 "config": {
				        "type": "object"
				    }
				},
				"required": [
				             "metadata",
							 "config"
				             ],
				"type": "object"
				})"_json;
			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
		};
		void from_json(const nlohmann::json& j, XAppConfig& ref);
		void from_json(const nlohmann::json& j, std::vector<XAppConfig>& ref);
		void to_json(nlohmann::json& j, const XAppConfig& ref);
		void to_json(nlohmann::json& j, const std::vector<XAppConfig>& ref);

	}
}
#endif
