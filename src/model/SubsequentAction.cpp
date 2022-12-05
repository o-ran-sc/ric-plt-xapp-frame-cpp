#include"SubsequentAction.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, SubsequentAction& ref) {

			std::cout << __PRETTY_FUNCTION__ << "\n";
			ref.validate_json(j);

			j.at("SubsequentActionType").get_to(ref.SubsequentActionType);
			j.at("TimeToWait").get_to(ref.TimeToWait);
		}

		void to_json(nlohmann::json& j, const SubsequentAction& ref) {

			j = nlohmann::json{
				{"SubsequentActionType",ref.SubsequentActionType},
				{"TimeToWait", ref.TimeToWait}
			};
		}
	}
}