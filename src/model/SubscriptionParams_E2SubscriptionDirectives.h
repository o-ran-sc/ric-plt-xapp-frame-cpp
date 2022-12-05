/*
# ==================================================================================
# Copyright (c) 2020 HCL Technologies Limited.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==================================================================================
*/
#ifndef XAPP_MODEL_SubscriptionParams_E2SubscriptionDirectives_H
#define XAPP_MODEL_SubscriptionParams_E2SubscriptionDirectives_H
#include"ModelBase.h"
#include"nlohmann/json.hpp"

namespace xapp {
	namespace model {

		struct SubscriptionParams_E2SubscriptionDirectives : public ModelBase {

			int E2RetryCount=INT_MIN;
			int E2TimeoutTimerValue=INT_MIN;
			bool RMRRoutingNeeded;
			nlohmann::json validator_schema = R"(
			{
			"$schema": "http://json-schema.org/draft-07/schema#",
			"title": "SubscriptionParams_E2SubscriptionDirectives",
			"description": "Optional. If not set Submgr uses its default values",
			"properties": {
			    "E2RetryCount": {
			        "description": "How many times E2 subscription request is retried",
			        "type": "integer",
			        "minimum": 0,
			        "maximum": 10
			    },
			    "E2TimeoutTimerValue": {
			        "description": "How long time response is waited from E2 node",
			        "type": "integer",
			        "minimum": 0,
			        "maximum": 10
			    },
			    "RMRRoutingNeeded": {
			        "description": "Subscription needs RMR route from E2Term to xApp",
			        "type": "boolean"
			    }
			},
			"required": [
			             "E2TimeoutTimerValue",
			             "E2RetryCount",
			             "RMRRoutingNeeded"
			            ],
			"type": "object"
			})"_json;
			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
		};
		
		void from_json(const nlohmann::json& j, SubscriptionParams_E2SubscriptionDirectives& ref);
		void to_json(nlohmann::json& j, const SubscriptionParams_E2SubscriptionDirectives& ref);

		
	} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_SubscriptionParams_ClientEndpoint_H*/
