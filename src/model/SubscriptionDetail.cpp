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
#include"SubscriptionDetail.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, SubscriptionDetail& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);

			j.at("XappEventInstanceId").get_to(ref.XappEventInstanceId);
			j.at("EventTriggers").get_to(ref.EventTriggers);
			j.at("ActionToBeSetupList").get_to(ref.Data);
		}

		void from_json(const nlohmann::json& j, std::vector<SubscriptionDetail>& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;


			for (auto& element : j) {
				SubscriptionDetail tmp;
				tmp.validate_json(element);
				for (auto& val : element.items()) {
					if (val.key() == "XappEventInstanceId") {

						//tmp.SetXappEventInstanceId(val.value());
						tmp.XappEventInstanceId = val.value();

					}
					else if (val.key() == "EventTriggers") {
						//tmp.setEventTriggers(val.value());
						tmp.EventTriggers.assign(val.value().begin(), val.value().end());


					}
					else if (val.key() == "ActionToBeSetupList") {

						nlohmann::json tmp_j(val.value());
						from_json(tmp_j, tmp.Data);
					}
				}
				ref.push_back(tmp);

			}

			/*
			for (int i = 0; i < j.size(); i++)
			{
				ref[i].validate_json(j.at(i));
				j.at(i).at("XappEventInstanceId").get_to(ref[i].XappEventInstanceId);
				j.at(i).at("EventTriggers").get_to(ref[i].EventTriggers);
				j.at(i).at("ActionToBeSetupList").get_to(ref[i].Data);
			}
			*/

		}

		void to_json(nlohmann::json& j, const SubscriptionDetail & ref) {

			j = nlohmann::json{
				{"XappEventInstanceId", ref.XappEventInstanceId},
				{"EventTriggers", ref.EventTriggers}
			};
			nlohmann::json tmp_j;
			to_json(tmp_j, ref.Data);
			j["ActionToBeSetupList"] = tmp_j;
			//for (int i = 0; i < ref.Data.size(); i++) {
			//	nlohmann::json tmp_j;
			//	to_json(tmp_j, ref.Data[i]);
			//	j["ActionToBeSetupList"] += tmp_j;
			//}
		}
		void to_json(nlohmann::json& j, const std::vector<SubscriptionDetail> & ref) {
			for (int i = 0; i < ref.size(); i++)
			{
				nlohmann::json tmp_j;
				to_json(tmp_j, ref[i]);
				j += tmp_j;
			}
		}
	}
}