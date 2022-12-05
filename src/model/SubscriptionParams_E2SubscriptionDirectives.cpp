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
#include"SubscriptionParams_E2SubscriptionDirectives.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, SubscriptionParams_E2SubscriptionDirectives& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);
			j.at("E2RetryCount").get_to(ref.E2RetryCount);
			j.at("E2TimeoutTimerValue").get_to(ref.E2TimeoutTimerValue);
			j.at("RMRRoutingNeeded").get_to(ref.RMRRoutingNeeded);
		}

		void to_json(nlohmann::json& j, const SubscriptionParams_E2SubscriptionDirectives& ref) {

			j = nlohmann::json{
				{"E2RetryCount", ref.E2RetryCount},
				{"E2TimeoutTimerValue", ref.E2TimeoutTimerValue},
				{"RMRRoutingNeeded", ref.RMRRoutingNeeded},
			};
		}
	}
}