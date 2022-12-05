#ifndef XAPP_MODEL_SubscriptionData_H
#define XAPP_MODEL_SubscriptionData_H
#include "SubscriptionInstance.h"
namespace xapp {
	namespace model {

		struct SubscriptionData : ModelBase {
			std::vector<std::string> ClientEndpoint;
			std::string Meid;
			int SubscriptionID=INT_MIN;
			std::vector<SubscriptionInstance> m_SubscriptionInstances;

			nlohmann::json validator_schema = R"(
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
				"type": "object"
				})"_json;

			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
		};
		void from_json(const nlohmann::json& j, SubscriptionData& ref);
		void from_json(const nlohmann::json& j, std::vector<SubscriptionData>& ref);
		void to_json(nlohmann::json& j, const SubscriptionData& ref);
		void to_json(nlohmann::json& j, const std::vector<SubscriptionData>& ref);


	} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_SubscriptionData_H*/


