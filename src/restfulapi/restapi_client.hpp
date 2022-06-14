#ifndef _restapi_client_hpp
#define _restapi_client_hpp

#include <iostream>
#include <string>
#include <memory>

#include "config.hpp"
#include "cpprest-client/ModelBase.h"
#include "cpprest-client/api/CommonApi.h"
#include "cpprest-client/api/XappApi.h"
#include "cpprest-client/ApiConfiguration.h"
#include "cpprest-client/ApiClient.h"
#include "cpprest-client/JsonBody.h"

namespace xapp {
namespace RestModel {

using namespace io::swagger;

typedef client::model::ActionToBeSetup ActionToBeSetup;
typedef client::model::ConfigMetadata ConfigMetadata;
typedef client::model::SubscriptionData SubscriptionData;
typedef client::model::SubscriptionDetail SubscriptionDetail;
typedef client::model::SubscriptionInstance SubscriptionInstance;
typedef client::model::SubscriptionParams_ClientEndpoint \
            SubscriptionParams_ClientEndpoint;
typedef client::model::SubscriptionParams_E2SubscriptionDirectives \
            SubscriptionParams_E2SubscriptionDirectives;
typedef client::model::SubscriptionParams SubscriptionParams;
typedef client::model::SubscriptionResponse SubscriptionResponse;
typedef client::model::SubsequentAction SubsequentAction;
typedef client::model::XAppConfig XAppConfig;
typedef client::model::RegisterData RegisterData;
typedef client::model::DeregisterData DeregisterData;
} /*namespace RestModel*/

using namespace RestModel;
using namespace io::swagger::client::api;
using namespace io::swagger::client::model;
class RestClient {
    private:
        std::shared_ptr<ApiConfiguration> p_ApiConfiguration = NULL;
        std::shared_ptr<ApiClient> p_ApiClient = NULL;
        std::string responseHttpContentType = "application/json";
        std::string requestHttpContentType = "application/json";
    public:
        RestClient(std::string baseUrl);
        ~RestClient();

        void Subscribe(std::shared_ptr<SubscriptionParams> p_SubscriptionParams);
        void Unsubscribe(std::string subId);
        void QuerySubscriptions();


        pplx::task<std::string> doPost(std::string path, std::string json);
        pplx::task<std::string> doPost(std::string path, web::json::value json);
        pplx::task<std::string> doGet(std::string path);
        pplx::task<void> doPut(std::string path, std::string json);
        pplx::task<void> doDelete(std::string path, std::string json);

        void setResponseHttpContentType(std::string type);
        std::string getResponseHttpContentType();

        void setRequestHttpContentType(std::string type);
        std::string getRequestHttpContentType();

        void setBaseUrl(std::string baseUrl);
        std::string getBaseUrl();
};
}/*namespace xapp*/

#endif /*_restapi_client_hpp*/
