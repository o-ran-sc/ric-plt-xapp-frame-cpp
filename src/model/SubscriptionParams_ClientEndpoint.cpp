#include"SubscriptionParams_ClientEndpoint.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, SubscriptionParams_ClientEndpoint& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);
			j.at("HTTPPort").get_to(ref.HTTPPort);
			j.at("Host").get_to(ref.Host);
			j.at("RMRPort").get_to(ref.RMRPort);
		}

		void to_json(nlohmann::json& j, const SubscriptionParams_ClientEndpoint& ref) {
			j = nlohmann::json{
				{"HTTPPort", ref.HTTPPort},
				{"Host", ref.Host},
				{"RMRPort", ref.RMRPort},
			};
		}
	}
}