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
#include"SubscriptionParams.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, SubscriptionParams& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);
			j.at("ClientEndpoint").get_to(ref.ClientEndpoint);

			if (j.contains("E2SubscriptionDirectives"))
			{
				j.at("E2SubscriptionDirectives").get_to(ref.E2SubscriptionDirectives);
			}

			if (j.contains("SubscriptionId"))
			{
				j.at("SubscriptionId").get_to(ref.SubscriptionId);
			}

			j.at("Meid").get_to(ref.Meid);
			j.at("RANFunctionID").get_to(ref.RANFunctionID);
			j.at("SubscriptionDetails").get_to(ref.Data);
		}

		void to_json(nlohmann::json& j, const SubscriptionParams& ref) {

			j = nlohmann::json{
				{"ClientEndpoint", ref.ClientEndpoint},
				//{"E2SubscriptionDirectives", ref.E2SubscriptionDirectives},
				{"Meid", ref.Meid},
				{"RANFunctionID", ref.RANFunctionID},
				{"SubscriptionId", ref.SubscriptionId}
			};
			if (ref.E2SubscriptionDirectives.E2RetryCount != INT_MIN && ref.E2SubscriptionDirectives.E2TimeoutTimerValue != INT_MIN) {
				//nlohmann::json tmp_j;
				//to_json(tmp_j, ref.E2SubscriptionDirectives);
				//j["E2SubscriptionDirectives"] = tmp_j;
				j["E2SubscriptionDirectives"] = ref.E2SubscriptionDirectives;
			}
			for (int i = 0; i < ref.Data.size(); i++) {
				nlohmann::json tmp_j;
				to_json(tmp_j, ref.Data[i]);
				j["SubscriptionDetails"] += tmp_j;
			}
		}
	}
}
