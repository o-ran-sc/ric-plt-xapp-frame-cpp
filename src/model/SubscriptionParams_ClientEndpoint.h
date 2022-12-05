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

