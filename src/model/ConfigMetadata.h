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
#ifndef XAPP_MODEL_ConfigMetadata_H
#define XAPP_MODEL_ConfigMetadata_H

#include "ModelBase.h"

namespace xapp {
	namespace model {

		struct ConfigMetadata : public ModelBase {
			std::string ConfigType;
			std::string XappName;
			nlohmann::json validator_schema = R"(
				{
				"$schema": "http://json-schema.org/draft-07/schema#",
				"title": "ConfigMetadata",
				"properties": {
				    "configType": {
				        "description": "Type of Config",
				        "type": "string",
				        "enum": ["json", "xml", "other"]
				    },
				    "xappName": {
				        "description": "Name of xApp",
				        "type": "string"
				    }
				},
				"required": [
				             "ConfigType",
				             "XappName"
				             ],
				"type": "object"
				})"_json;

			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
		};
		void from_json(const nlohmann::json& j, ConfigMetadata& ref);
		void to_json(nlohmann::json& j, const ConfigMetadata& ref);

	} /*namespace model*/
} /*namespace xapp*/
#endif /*XAPP_MODEL_ConfigMetadata_H*/
