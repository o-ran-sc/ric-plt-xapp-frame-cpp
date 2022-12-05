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
#include"SubscriptionParams_ClientEndpoint.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, SubscriptionParams_ClientEndpoint& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);
			j.at("HTTPPort").get_to(ref.HTTPPort);
			j.at("Host").get_to(ref.Host);
			j.at("RMRPort").get_to(ref.RMRPort);
		}

		void to_json(nlohmann::json& j, const SubscriptionParams_ClientEndpoint& ref) {
			j = nlohmann::json{
				{"HTTPPort", ref.HTTPPort},
				{"Host", ref.Host},
				{"RMRPort", ref.RMRPort},
			};
		}
	}
}