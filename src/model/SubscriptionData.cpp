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
#include"SubscriptionData.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, SubscriptionData& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);
			if (j.contains("SubscriptionId")) {
				j.at("SubscriptionId").get_to(ref.SubscriptionID);
			}
			if (j.contains("SubscriptionInstances")) {
				j.at("SubscriptionInstances").get_to(ref.m_SubscriptionInstances);
			}
			
			if (j.contains("Meid")) {
				j.at("Meid").get_to(ref.Meid);
			}
			
			if (j.contains("ClientEndpoint")) {
				j.at("ClientEndpoint").get_to(ref.ClientEndpoint);
			}
			
		}

		void from_json(const nlohmann::json& j, std::vector<SubscriptionData>& ref) {
			for (auto& element : j) {
				SubscriptionData tmp;
				tmp.validate_json(element);
				for (auto& val : element.items())
				{
					if (val.key() == "SubscriptionId") {
						tmp.SubscriptionID = val.value();
					}
					else if (val.key() == "Meid") {
						tmp.Meid = val.value();
					}
					else if (val.key() == "ClientEndpoint") {

						tmp.ClientEndpoint.assign(val.value().begin(), val.value().end());
					}
					else if (val.key() == "SubscriptionInstances") {
						from_json(val.value(), tmp.m_SubscriptionInstances);
						/*
						tmp.m_SubscriptionInstances.E2EventInstanceId= val.value()["E2EventInstanceId"];
						tmp.m_SubscriptionInstances.XappEventInstanceId= val.value()["XappEventInstanceId"];
						if (val.value().contains("ErrorCause"))
						{
							tmp.m_SubscriptionInstances.ErrorCause= val.value()["ErrorCause"];
						}
						if (val.value().contains("ErrorSource"))
						{
							tmp.m_SubscriptionInstances.ErrorCause = val.value()["ErrorSource"];
						}
						if (val.value().contains("TimeoutType"))
						{
							tmp.m_SubscriptionInstances.ErrorCause = val.value()["TimeoutType"];
						}
						*/

					}
				}
				ref.push_back(tmp);
			}

		}

		void to_json(nlohmann::json& j, const SubscriptionData& ref) {
			/*
			j = nlohmann::json{
				{"SubscriptionId",ref.SubscriptionID},
				{"Meid",ref.Meid},
				{"ClientEndpoint", ref.ClientEndpoint}//,
				//{"SubscriptionInstances", {{"XappEventInstanceId", ref.m_SubscriptionInstances.XappEventInstanceId},{"E2EventInstanceId", ref.m_SubscriptionInstances.E2EventInstanceId},{"ErrorCause", ref.m_SubscriptionInstances.ErrorCause},{"ErrorSource", ref.m_SubscriptionInstances.ErrorSource},{"TimeoutType", ref.m_SubscriptionInstances.TimeoutType}}}

			};
			nlohmann::json tmp_j;
			to_json(tmp_j, ref.m_SubscriptionInstances);
			j["SubscriptionInstances"] = tmp_j;
			*/
			if (ref.SubscriptionID != "") {
				j["SubscriptionId"] = ref.SubscriptionID;
			}
			if (ref.Meid.length() > 0) {
				j["Meid"] = ref.Meid;
			}
			if (ref.ClientEndpoint.size() > 0) {
				j["ClientEndpoint"] = ref.ClientEndpoint;
			}
			if (ref.m_SubscriptionInstances.size() > 0) {
				nlohmann::json tmp_j;
				to_json(tmp_j, ref.m_SubscriptionInstances);
				j["SubscriptionInstances"] = tmp_j;
			}
		}
		void to_json(nlohmann::json& j, const std::vector<SubscriptionData>& ref) {
			for (int i = 0; i < ref.size(); i++)
			{
				nlohmann::json tmp_j;
				to_json(tmp_j, ref[i]);
				j += tmp_j;
			}
		}
	}
}
