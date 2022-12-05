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
#include"SubscriptionInstance.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, SubscriptionInstance& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);

			j.at("XappEventInstanceId").get_to(ref.XappEventInstanceId);
			j.at("E2EventInstanceId").get_to(ref.E2EventInstanceId);
			j.at("ErrorCause").get_to(ref.ErrorCause);
			j.at("ErrorSource").get_to(ref.ErrorSource);
			j.at("TimeoutType").get_to(ref.TimeoutType);
		}
		void from_json(const nlohmann::json& j, std::vector<SubscriptionInstance>& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			for (auto& element : j) {
				SubscriptionInstance tmp;
				tmp.validate_json(element);
				for (auto& val : element.items())
				{


					if (val.key() == "XappEventInstanceId") {

						tmp.XappEventInstanceId = val.value();

					}
					else if (val.key() == "E2EventInstanceId") {
						tmp.E2EventInstanceId = val.value();
					}
					else if (val.key() == "ErrorCause") {
						tmp.ErrorCause = val.value();
					}
					else if (val.key() == "ErrorSource") {
						tmp.ErrorSource = val.value();
					}
					else if (val.key() == "TimeoutType") {
						tmp.TimeoutType = val.value();
					}


				}
				ref.push_back(tmp);


			}


		}

		void to_json(nlohmann::json& j, const SubscriptionInstance& ref) {

			j = nlohmann::json{
				{"XappEventInstanceId", ref.XappEventInstanceId},
				{"E2EventInstanceId", ref.E2EventInstanceId},
				{"ErrorCause", ref.ErrorCause},
				{"ErrorSource", ref.ErrorSource},
				{"TimeoutType", ref.TimeoutType}
			};
		}
		void to_json(nlohmann::json& j, const std::vector<SubscriptionInstance>& ref) {


			for (int i = 0; i < ref.size(); i++)
			{

				nlohmann::json tmp = nlohmann::json{
				{"XappEventInstanceId", ref[i].XappEventInstanceId},
				{"E2EventInstanceId", ref[i].E2EventInstanceId},
				{"ErrorCause", ref[i].ErrorCause},
				{"ErrorSource", ref[i].ErrorSource},
				{"TimeoutType", ref[i].TimeoutType}
				};
				j.push_back(tmp);
			}
		}
	}
}