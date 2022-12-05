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
#ifndef XAPP_MODEL_SubscriptionParams_H
#define XAPP_MODEL_SubscriptionParams_H
#include"SubscriptionDetail.h"
#include"SubscriptionParams_ClientEndpoint.h"
#include"SubscriptionParams_E2SubscriptionDirectives.h"

namespace xapp

{
	namespace model {

		struct SubscriptionParams:public ModelBase {

			std::string Host{ "" };
			int HTTPPort{ 0 };
			int RMRPort{ 0 };
			SubscriptionParams_ClientEndpoint ClientEndpoint;
			std::string Meid{ "" };
			int RANFunctionID{ 0 };
			std::string SubscriptionId{ "" };
			std::vector<SubscriptionDetail> Data;
			SubscriptionParams_E2SubscriptionDirectives E2SubscriptionDirectives;

			nlohmann::json validator_schema = R"(
			{
			"$schema": "http://json-schema.org/draft-07/schema#",
			"title": "SubscriptionParams",
			"properties": {
			    "SubscriptionId": {
			        "description": "Optional subscription ID '(Submgr allocates if not given)'",
			        "type": "string"
			    },
			    "Meid": {
			        "type": "string"
			    },
			    "RANFunctionID": {
			        "type": "integer",
			        "minimum": 0,
			        "maximum": 4095
			    },
				"ClientEndpoint":{
					"type": "object"
				},
				"E2SubscriptionDirectives":{
					"type": "object"
				},
				"SubscriptionDetails":{
					"type": "array"
				}
			},
			"required": [
			             "ClientEndpoint",
			             "Meid",
			             "RANFunctionID",
			             "SubscriptionDetails"
			            ],
			"type": "object"
			})"_json;

			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
		};
		void from_json(const nlohmann::json& j, SubscriptionParams& ref);
		void to_json(nlohmann::json& j, const SubscriptionParams& ref);


	}
}
#endif /*XAPP_MODEL_SubscriptionParams_H*/
