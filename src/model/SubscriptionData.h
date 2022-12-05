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
#ifndef XAPP_MODEL_SubscriptionData_H
#define XAPP_MODEL_SubscriptionData_H
#include "SubscriptionInstance.h"
namespace xapp {
	namespace model {

		struct SubscriptionData : ModelBase {
			std::vector<std::string> ClientEndpoint;
			std::string Meid;
			std::string SubscriptionID="";
			std::vector<SubscriptionInstance> m_SubscriptionInstances;

			nlohmann::json validator_schema = R"(
				{
				"$schema": "http://json-schema.org/draft-07/schema#",
				"title": "SubscriptionData",
				"properties": {
				    "SubscriptionId": {
				        "type": "string"
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


