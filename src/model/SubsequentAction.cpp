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
#include"SubsequentAction.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, SubsequentAction& ref) {

			std::cout << __PRETTY_FUNCTION__ << "\n";
			ref.validate_json(j);

			j.at("SubsequentActionType").get_to(ref.SubsequentActionType);
			j.at("TimeToWait").get_to(ref.TimeToWait);
		}

		void to_json(nlohmann::json& j, const SubsequentAction& ref) {

			j = nlohmann::json{
				{"SubsequentActionType",ref.SubsequentActionType},
				{"TimeToWait", ref.TimeToWait}
			};
		}
	}
}