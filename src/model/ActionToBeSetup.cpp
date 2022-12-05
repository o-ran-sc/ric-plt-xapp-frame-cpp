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
#include"ActionToBeSetup.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json & j, ActionToBeSetup& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);
			if (j.contains("ActionDefinition")) {
				j.at("ActionDefinition").get_to(ref.ActionDefinition);
			}
			j.at("ActionID").get_to(ref.ActionID);
			j.at("ActionType").get_to(ref.ActionType);
			if (j.contains("SubsequentAction")) {
				j.at("SubsequentAction").get_to(ref.m_SubsequentAction);
			}
			

		}

		void from_json(const nlohmann::json & j, std::vector<ActionToBeSetup>& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;

			for (auto& element : j) {
				ActionToBeSetup tmp;
				tmp.validate_json(element);
				for (auto& val : element.items())
				{
					if (val.key() == "ActionDefinition") {

						tmp.ActionDefinition.assign(val.value().begin(), val.value().end());

					}
					else if (val.key() == "ActionID") {
						tmp.ActionID = val.value();
					}
					else if (val.key() == "ActionType") {
						tmp.ActionType = val.value();
					}
					else if (val.key() == "SubsequentAction") {
						tmp.m_SubsequentAction.SubsequentActionType = val.value()["SubsequentActionType"];
						tmp.m_SubsequentAction.TimeToWait = val.value()["TimeToWait"];
					}


				}
				ref.push_back(tmp);
			}

		}
		void to_json(nlohmann::json& j, const ActionToBeSetup& ref) {

			j = nlohmann::json{
				//{"ActionDefinition", ref.ActionDefinition},
				{"ActionID", ref.ActionID},
				{"ActionType", ref.ActionType}//,
				//{"SubsequentAction", {{"SubsequentActionType",ref.m_SubsequentAction.SubsequentActionType},{"TimeToWait", ref.m_SubsequentAction.TimeToWait } }}
			};
			if (ref.ActionDefinition.size() > 0) {
				j["ActionDefinition"] = ref.ActionDefinition;
			}
			if (ref.m_SubsequentAction.SubsequentActionType.length() > 0 && ref.m_SubsequentAction.TimeToWait.length() > 0) {
				j["SubsequentAction"] = ref.m_SubsequentAction;
			}

		}

		void to_json(nlohmann::json& j, const std::vector<ActionToBeSetup>& ref) {
			/*
			for (int i = 0; i < ref.size(); i++) {
				nlohmann::json tmp;
				tmp = nlohmann::json{
				{"ActionDefinition", ref[i].ActionDefinition},
				{"ActionID", ref[i].ActionID},
				{"ActionType", ref[i].ActionType},
				{"SubsequentAction", {{"SubsequentActionType",ref[i].m_SubsequentAction.SubsequentActionType},{"TimeToWait", ref[i].m_SubsequentAction.TimeToWait } }}
				};
				j.push_back(tmp);
			}
			*/
			for (int i = 0; i < ref.size(); i++) {
				nlohmann::json tmp;
				to_json(tmp, ref[i]);
				j.push_back(tmp);
			}
		}
	}
}
