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
#ifndef _CleintModelBase_H
#define _CleintModelBase_H
#include<nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <type_traits>
#include<vector>
#include<string>
#include<iostream>
#include<limits.h>
#include"temperory.h"
namespace xapp
{
	namespace model
	{
		struct ModelBase {
			nlohmann::json validator_schema = R"(
			{
			    "$schema": "http://json-schema.org/draft-07/schema#",
			    "title": "ModelBase"
			})"_json;



			void validate_json(const nlohmann::json& _json) {
				nlohmann::json_schema::json_validator validator;
				validator.set_root_schema(get_validator_schema());
				
				try {
					validator.validate(_json);
				}
				catch (const std::exception& e) {
					throw;
				}
				
				return;
			}

			virtual nlohmann::json get_validator_schema() const { return validator_schema; }
		};

	}

}


#endif
