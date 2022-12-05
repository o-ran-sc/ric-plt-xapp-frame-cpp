#ifndef _SubscriptionDetails_H
#define _SubscriptionDetails_H
#include"ActionToBeSetup.h"

namespace xapp
{
	namespace model
	{
		struct SubscriptionDetail : public ModelBase
		{
			std::vector<ActionToBeSetup> Data;
			int XappEventInstanceId{0};
			std::vector<int> EventTriggers{};
			friend void from_json(const nlohmann::json& j, SubscriptionDetail& ref);
			friend void to_json(nlohmann::json& j, const SubscriptionDetail& ref);
			friend void from_json(const nlohmann::json& j, std::vector<SubscriptionDetail>& ref);
			nlohmann::json validator_schema = R"(
			{
			"$schema": "http://json-schema.org/draft-07/schema#",
			"title": "Subscription detail",
			"properties": {
			    "XappEventInstanceId": {
			        "type": "integer",
			        "minimum": 0,
			        "maximum": 255
			    },
			    "EventTriggers": {
			        "description": "Identification of Action",
			        "type": "array",
			        "items": {
			            "type": "integer"
			        }
			    },
			    "ActionToBeSetupList": {
			        "type": "array"
			    }
			},
			"required": [
			             "XappEventInstanceId",
			             "EventTriggers",
			             "ActionToBeSetupList"
			            ],
			"type": "object"
			})"_json;

			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
			

		};
		void from_json(const nlohmann::json& j, SubscriptionDetail& ref);
		void from_json(const nlohmann::json& j, std::vector<SubscriptionDetail>& ref);
		void to_json(nlohmann::json& j, const SubscriptionDetail & ref);
		void to_json(nlohmann::json& j, const std::vector<SubscriptionDetail> & ref);
		

	}
}

#endif
