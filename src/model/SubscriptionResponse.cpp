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
#include"SubscriptionResponse.h"
namespace xapp {
	namespace model {
		void from_json(const  nlohmann::json& j, SubscriptionResponse& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);

			j.at("SubscriptionId").get_to(ref.SubscriptionId);
			j.at("SubscriptionInstances").get_to(ref.SubInst);
		}

		void to_json(nlohmann::json& j, const SubscriptionResponse& ref) {

			//j = nlohmann::json{
			//	{"SubscriptionId",ref.SubscriptionId},
			//	{"SubscriptionInstances", ref.SubInst}
			//};
			j = nlohmann::json{
				{"SubscriptionId",ref.SubscriptionId}
			};
			for (int i = 0; i < ref.SubInst.size(); i++)
			{
				j["SubscriptionInstances"] += nlohmann::json{
				{"XappEventInstanceId", ref.SubInst[i].XappEventInstanceId},
				{"E2EventInstanceId", ref.SubInst[i].E2EventInstanceId},
				{"ErrorCause", ref.SubInst[i].ErrorCause},
				{"ErrorSource", ref.SubInst[i].ErrorSource},
				{"TimeoutType", ref.SubInst[i].TimeoutType}
				};
			}

		}
	}
}