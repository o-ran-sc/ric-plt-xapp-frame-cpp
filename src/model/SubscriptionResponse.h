#ifndef XAPP_MODEL_SubscriptionResponse_H
#define XAPP_MODEL_SubscriptionResponse_H
#include"SubscriptionInstance.h"
namespace xapp
{
	namespace model {
		struct SubscriptionResponse:public ModelBase
		{
			nlohmann::json validator_schema = R"(
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

			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
			std::string SubscriptionId{ "" };
			std::vector<SubscriptionInstance > SubInst;

		};
		void from_json(const  nlohmann::json& j, SubscriptionResponse& ref);
		void to_json(nlohmann::json& j, const SubscriptionResponse& ref);

	}

}
#endif

