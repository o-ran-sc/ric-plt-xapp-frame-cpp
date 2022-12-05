#ifndef SubsequentAction_H
#define SubsequentAction_H
#include"ModelBase.h"
namespace xapp
{
	namespace model
	{
		struct SubsequentAction : public ModelBase {
			std::string SubsequentActionType;
			std::string TimeToWait;
			nlohmann::json validator_schema = R"(
			{
			"$schema": "http://json-schema.org/draft-07/schema#",
			"title": "SubsequentAction",
			"properties": {
			    "SubsequentActionType": {
			        "description": "Type of Subsequent Action",
			        "type": "string",
			        "enum": ["wait", "continue"]

			    },
			    "TimeToWait": {
			        "description": "Time to waiting",
			        "type": "string",
			        "enum": ["zero", "w1ms", "w2ms", "w5ms", "w10ms", "w20ms", "w30ms",
			                "w40ms", "w50ms", "w100ms", "w200ms", "w500ms", "w1s",
			                "w2s", "w5s", "w10s", "w20s", "w60s"]
			    }
			},
			"required": [
			             "SubsequentActionType",
			             "TimeToWait"
			             ],
			"type": "object"
			})"_json;
			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
		};
		void from_json(const nlohmann::json& j, SubsequentAction& ref);
		void to_json(nlohmann::json& j, const SubsequentAction& ref);

	}
	
}

#endif
