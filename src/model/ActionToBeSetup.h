#ifndef _ActionToBeSetupList_H
#define _ActionToBeSetupList_H
#include"SubsequentAction.h"

namespace xapp
{
	namespace model
	{
		struct ActionToBeSetup: public ModelBase
		{
			int ActionID{0};
			std::string ActionType{""};
			//std::string SubsequentActionType{""};
			//std::string TimeToWait{""};
			SubsequentAction m_SubsequentAction; 
			std::vector<int> ActionDefinition{};

			nlohmann::json validator_schema = R"(
			{
			"$schema": "http://json-schema.org/draft-07/schema#",
			"title": "ActionToBeSetup",
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
			             "ActionID",
			             "ActionType"
			            ],
			"type": "object"
			})"_json;

			virtual nlohmann::json get_validator_schema() const { return validator_schema; }

		};

		void from_json(const nlohmann::json & j, ActionToBeSetup& ref);
		void from_json(const nlohmann::json & j, std::vector<ActionToBeSetup>& ref);
		void to_json(nlohmann::json& j, const ActionToBeSetup& ref);
		void to_json(nlohmann::json& j, const std::vector<ActionToBeSetup>& ref);

	}
	
}


#endif

