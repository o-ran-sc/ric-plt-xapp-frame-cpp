#ifndef SubscriptionInstance_H
#define SubscriptionInstance_H
#include"ModelBase.h"

namespace xapp
{
	namespace model
	{
		struct SubscriptionInstance : public ModelBase
		{
		public:
			nlohmann::json validator_schema = R"(
			{
			"$schema": "http://json-schema.org/draft-07/schema#",
			"title": "SubscriptionInstance",
			"description": "xApp service address and port",
			"properties": {
			    "XappEventInstanceId": {
			        "type": "integer",
			        "minimum": 0,
			        "maximum": 65535
			    },
			    "E2EventInstanceId": {
			        "type": "integer",
			        "minimum": 0,
			        "maximum": 65535
			    },
			    "ErrorCause": {
			        "description": "Descriptive error cause. Empty string when no error.",
			        "type": "string"
			    },
			    "ErrorSource": {
			        "description": "Source of error cause.",
			        "type": "string",
			        "enum": ["SUBMGR", "RTMGR", "DBAAS", "ASN1", "E2Node"]
			    },
			    "TimeoutType": {
			        "description": "Type timeout. xApp should retry if timeout occurs.",
			        "type": "string",
			        "enum": ["E2-Timeout", "RTMGR-Timeout", "DBAAS-Timeout"]
			    }
			},
			"required": [
			             "XappEventInstanceId",
			             "E2EventInstanceId"
			             ],
			"type": "object"
			})"_json;
			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
			int XappEventInstanceId{ 0 };
			int	E2EventInstanceId{ 0 };
			std::string	ErrorCause{ "" };
			std::string	ErrorSource{ "" };
			std::string	TimeoutType{ "" };
		};
		void from_json(const nlohmann::json& j, SubscriptionInstance& ref);
		void from_json(const nlohmann::json& j, std::vector<SubscriptionInstance>& ref);
		void to_json(nlohmann::json& j, const SubscriptionInstance& ref);
		void to_json(nlohmann::json& j, const std::vector<SubscriptionInstance>& ref);

	}
	
}
#endif
