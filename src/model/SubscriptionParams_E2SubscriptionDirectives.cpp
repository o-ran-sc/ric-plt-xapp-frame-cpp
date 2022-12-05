#include"SubscriptionParams_E2SubscriptionDirectives.h"
namespace xapp {
	namespace model {
		void from_json(const nlohmann::json& j, SubscriptionParams_E2SubscriptionDirectives& ref) {

			std::cout << __PRETTY_FUNCTION__ << std::endl;
			ref.validate_json(j);
			j.at("E2RetryCount").get_to(ref.E2RetryCount);
			j.at("E2TimeoutTimerValue").get_to(ref.E2TimeoutTimerValue);
			j.at("RMRRoutingNeeded").get_to(ref.RMRRoutingNeeded);
		}

		void to_json(nlohmann::json& j, const SubscriptionParams_E2SubscriptionDirectives& ref) {

			j = nlohmann::json{
				{"E2RetryCount", ref.E2RetryCount},
				{"E2TimeoutTimerValue", ref.E2TimeoutTimerValue},
				{"RMRRoutingNeeded", ref.RMRRoutingNeeded},
			};
		}
	}
}