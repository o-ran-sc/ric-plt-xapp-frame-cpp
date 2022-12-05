#ifndef XAPP_MODEL_SubscriptionParams_ClientEndpoint_H
#define XAPP_MODEL_SubscriptionParams_ClientEndpoint_H
#include "ModelBase.h"
#include"nlohmann/json.hpp"

namespace xapp {
	namespace model {

		struct SubscriptionParams_ClientEndpoint :public ModelBase {
			int HTTPPort;
			std::string Host;
			int RMRPort;
			nlohmann::json validator_schema = R"(
			{
			"$schema": "http://json-schema.org/draft-07/schema#",
			"title": "SubscriptionParams_ClientEndpoint",
			"description": "xApp service address and port",
			"properties": {
			    "HTTPPort": {
			        "description": "xApp HTTP service address port",
			        "type": "integer",
			        "minimum": 0,
			        "maximum": 65535
			    },
			    "Host": {
			        "description": "xApp service address name like service-ricxapp-xappname-http.ricxapp",
			        "type": "string"
			    },
			    "RMRPort": {
			        "description": "xApp RMR service address port",
			        "type": "integer",
			        "minimum": 0,
			        "maximum": 65535
			    }
			},
			"required": [
			             "HTTPPort",
			             "Host",
			             "RMRPort"
			             ],
			"type": "object"
			})"_json;

			virtual nlohmann::json get_validator_schema() const { return validator_schema; }

		};
		void from_json(const nlohmann::json& j, SubscriptionParams_ClientEndpoint& ref);
		void to_json(nlohmann::json& j, const SubscriptionParams_ClientEndpoint& ref);

		
	} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_SubscriptionParams_ClientEndpoint_H*/

