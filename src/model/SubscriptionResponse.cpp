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