#include"ConfigMetadata.h"
namespace xapp {
	namespace model {

		void from_json(const nlohmann::json& j, ConfigMetadata& ref) {

			std::cout << __func__ << " ConfigMetadata " << std::endl;
			ref.validate_json(j);
			j.at("configType").get_to(ref.ConfigType);
			j.at("xappName").get_to(ref.XappName);
		}

		void to_json(nlohmann::json& j, const ConfigMetadata& ref) {
			j = nlohmann::json{
				{"configType",ref.ConfigType},
				{"xappName", ref.XappName}
			};
		}
	}
}