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
#include"XAppConfig.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, XAppConfig& ref) {
			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);
			j.at("metadata").get_to(ref.m_ConfigMetadata);
			j.at("config").get_to(ref.config);

		}
		void from_json(const nlohmann::json& j, std::vector<XAppConfig>& ref) {
			for (auto& element : j) {
				XAppConfig tmp;
				tmp.validate_json(element);
				for (auto& val : element.items())
				{
					if (val.key() == "metadata") {
						tmp.m_ConfigMetadata.ConfigType = val.value()["configType"];
						tmp.m_ConfigMetadata.XappName = val.value()["xappName"];
					}
					else if (val.key() == "config") {
						tmp.config = val.value();
					}
				}
				ref.push_back(tmp);
			}
		}

		void to_json(nlohmann::json& j, const XAppConfig& ref) {
			j = nlohmann::json{
				{"metadata",{{"configType",ref.m_ConfigMetadata.ConfigType},{"xappName",ref.m_ConfigMetadata.XappName}}},
				{"config",ref.config}
			};
		}

		void to_json(nlohmann::json& j, const std::vector<XAppConfig>& ref) {
			for (int i = 0; i < ref.size(); i++)
			{
				nlohmann::json tmp_j;
				to_json(tmp_j, ref[i]);
				j += tmp_j;
			}
		}
	}
}