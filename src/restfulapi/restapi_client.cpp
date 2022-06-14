#include "restapi_client.hpp"
#include <exception>

namespace xapp {
using namespace RestModel;
using namespace io::swagger::client::api;
using namespace io::swagger::client::model;

RestClient::RestClient(std::string baseUrl)
{
    if(!baseUrl.empty())
    {
        p_ApiConfiguration = std::make_shared<ApiConfiguration>();
        p_ApiConfiguration->setBaseUrl(baseUrl);
        p_ApiClient = std::make_shared<ApiClient>();
        p_ApiClient->setConfiguration(p_ApiConfiguration);
    }
}

RestClient::~RestClient()
{
}

void RestClient::Subscribe(std::shared_ptr<RestModel::SubscriptionParams> p_SubscriptionParams)
{
    if(NULL == p_SubscriptionParams)
        return;

    CommonApi commonapi(p_ApiClient);

    commonapi.subscribe(p_SubscriptionParams).then([=](pplx::task<std::shared_ptr<SubscriptionResponse>> p_SubscriptionResponse)
    {
        try
        {
            std::cerr << "Subscribe Successful" << std::endl;
            std::cerr << p_SubscriptionResponse.get()->getSubscriptionId() << std::endl;
        } catch(const std::exception& e) {
            std::cerr<< "getExample() exception: " << e.what() << std::endl;
        }
    });
}

void RestClient::registerXapp(std::shared_ptr<RegisterData> p_RegisterData)
{
    if(NULL == p_RegisterData)
        return;

    CommonApi commonapi(p_ApiClient);

    commonapi.registerXapp(p_RegisterData).then([=](pplx::task<void>)
    {
        try
        {
            std::cerr << "Register Successful" << std::endl;
        } catch(const std::exception& e) {
            std::cerr << "getExample() exception: " << e.what() << std::endl;
        }
    }).wait();
}

void RestClient::Unsubscribe(std::string subId)
{
    if(subId.empty())
        return;

    CommonApi commonapi(p_ApiClient);
    commonapi.unsubscribe(subId).then([=]()
    {
        try
        {
            /* code */
            std::cerr << "Unsubscribe Successful with SubscriptionId = " << subId << std::endl;
            return;
        }
        catch(const std::exception& e)
        {
            std::cerr << "getExample() exception: " << e.what() << '\n';
        }
    });
}

void RestClient::QuerySubscriptions()
{
    return;
}

pplx::task<std::string> RestClient::doPost(std::string path, std::string json)
{
    if(path.empty() || json.empty())
        throw std::runtime_error("Invalid argument");

    std::map<utility::string_t, utility::string_t> queryParams;
    std::map<utility::string_t, utility::string_t> headerParams(p_ApiConfiguration->getDefaultHeaders());
    std::map<utility::string_t, utility::string_t> formParams;
    std::map<utility::string_t, std::shared_ptr<HttpContent>> fileParams;

    // use JSON if possible
    if (responseHttpContentType.empty())
        throw std::runtime_error("responseContentType is blank");

    headerParams[utility::conversions::to_string_t("Accept")] = responseHttpContentType;

    std::shared_ptr<IHttpBody> httpBody;

    // use JSON if possible
    if (!requestHttpContentType.empty())
    {
        web::json::value json_par;
        if(!json.empty())
            httpBody = std::shared_ptr<IHttpBody>(new JsonBody(json_par.parse(json)));
    }
    else
    {
        //throw ApiException(415, utility::conversions::to_string_t("CommonApi->subscribe does not consume any supported media type"));
        throw std::runtime_error("requestHttpContentType is blank");
    }

    return p_ApiClient->callApi(path, utility::conversions::to_string_t("POST"), queryParams, httpBody, headerParams, formParams, fileParams, requestHttpContentType)
    .then([=](web::http::http_response response)
    {
        // 1xx - informational : OK
        // 2xx - successful       : OK
        // 3xx - redirection   : OK
        // 4xx - client error  : not OK
        // 5xx - client error  : not OK
        if (response.status_code() >= 400)
        {
            throw ApiException(response.status_code()
                , utility::conversions::to_string_t("error doPost: ") + response.reason_phrase()
                , std::make_shared<std::stringstream>(response.extract_utf8string(true).get()));
        }

        // check response content type
        if(response.headers().has(utility::conversions::to_string_t("Content-Type")))
        {
            utility::string_t contentType = response.headers()[utility::conversions::to_string_t("Content-Type")];
            if(0 != responseHttpContentType.compare(contentType))
            {
                throw ApiException(500
                    , utility::conversions::to_string_t("error doPost: unexpected response type: ") + contentType
                    , std::make_shared<std::stringstream>(response.extract_utf8string(true).get()));
            }
        }

        return response.extract_string();
    });
}

pplx::task<std::string> RestClient::doPost(std::string path, web::json::value json)
{
    if(path.empty())
        throw std::runtime_error("Invalid argument");

    std::map<utility::string_t, utility::string_t> queryParams;
    std::map<utility::string_t, utility::string_t> headerParams(p_ApiConfiguration->getDefaultHeaders());
    std::map<utility::string_t, utility::string_t> formParams;
    std::map<utility::string_t, std::shared_ptr<HttpContent>> fileParams;

    // use JSON if possible
    if (responseHttpContentType.empty())
        throw std::runtime_error("responseContentType is blank");

    headerParams[utility::conversions::to_string_t("Accept")] = responseHttpContentType;

    std::shared_ptr<IHttpBody> httpBody;

    // use JSON if possible
    if (!requestHttpContentType.empty())
    {
        httpBody = std::shared_ptr<IHttpBody>(new JsonBody(json));
    }
    else
    {
        //throw ApiException(415, utility::conversions::to_string_t("CommonApi->subscribe does not consume any supported media type"));
        throw std::runtime_error("requestHttpContentType is blank");
    }

    return p_ApiClient->callApi(path, utility::conversions::to_string_t("POST"), queryParams, httpBody, headerParams, formParams, fileParams, requestHttpContentType)
    .then([=](web::http::http_response response)
    {
        // 1xx - informational : OK
        // 2xx - successful       : OK
        // 3xx - redirection   : OK
        // 4xx - client error  : not OK
        // 5xx - client error  : not OK
        if (response.status_code() >= 400)
        {
            throw ApiException(response.status_code()
                , utility::conversions::to_string_t("error doPost: ") + response.reason_phrase()
                , std::make_shared<std::stringstream>(response.extract_utf8string(true).get()));
        }

        // check response content type
        if(response.headers().has(utility::conversions::to_string_t("Content-Type")))
        {
            utility::string_t contentType = response.headers()[utility::conversions::to_string_t("Content-Type")];
            if(0 != responseHttpContentType.compare(contentType))
            {
                throw ApiException(500
                    , utility::conversions::to_string_t("error doPost: unexpected response type: ") + contentType
                    , std::make_shared<std::stringstream>(response.extract_utf8string(true).get()));
            }
        }

        return response.extract_string();
    });
}

pplx::task<std::string> RestClient::doGet(std::string path)
{
    if(path.empty())
        throw std::runtime_error("Invalid argument");

    if (responseHttpContentType.empty())
        throw std::runtime_error("responseContentType is blank");

    if (requestHttpContentType.empty())
        //throw ApiException(415, utility::conversions::to_string_t("CommonApi->subscribe does not consume any supported media type"));
        throw std::runtime_error("requestHttpContentType is blank");

    std::map<utility::string_t, utility::string_t> queryParams;
    std::map<utility::string_t, utility::string_t> headerParams(p_ApiConfiguration->getDefaultHeaders());
    std::map<utility::string_t, utility::string_t> formParams;
    std::map<utility::string_t, std::shared_ptr<HttpContent>> fileParams;


    headerParams[utility::conversions::to_string_t("Accept")] = responseHttpContentType;

    std::shared_ptr<IHttpBody> httpBody;

    return p_ApiClient->callApi(path, utility::conversions::to_string_t("GET"), queryParams, httpBody, headerParams, formParams, fileParams, requestHttpContentType)
    .then([=](web::http::http_response response)
    {
        // 1xx - informational : OK
        // 2xx - successful       : OK
        // 3xx - redirection   : OK
        // 4xx - client error  : not OK
        // 5xx - client error  : not OK
        if (response.status_code() >= 400)
        {
            throw ApiException(response.status_code()
                , utility::conversions::to_string_t("error doGet: ") + response.reason_phrase()
                , std::make_shared<std::stringstream>(response.extract_utf8string(true).get()));
        }

        // check response content type
        if(response.headers().has(utility::conversions::to_string_t("Content-Type")))
        {
            utility::string_t contentType = response.headers()[utility::conversions::to_string_t("Content-Type")];
            if(0 != responseHttpContentType.compare(contentType))
            {
                throw ApiException(500
                    , utility::conversions::to_string_t("error doGet: unexpected response type: ") + contentType
                    , std::make_shared<std::stringstream>(response.extract_utf8string(true).get()));
            }
        }

        return response.extract_string();
    });
}

pplx::task<void> RestClient::doDelete(std::string path, std::string json)
{
    if(path.empty())
        throw std::runtime_error("Invalid argument");

    if (responseHttpContentType.empty())
        throw std::runtime_error("responseContentType is blank");

    if (requestHttpContentType.empty())
        //throw ApiException(415, utility::conversions::to_string_t("CommonApi->subscribe does not consume any supported media type"));
        throw std::runtime_error("requestHttpContentType is blank");

    std::map<utility::string_t, utility::string_t> queryParams;
    std::map<utility::string_t, utility::string_t> headerParams(p_ApiConfiguration->getDefaultHeaders());
    std::map<utility::string_t, utility::string_t> formParams;
    std::map<utility::string_t, std::shared_ptr<HttpContent>> fileParams;


    headerParams[utility::conversions::to_string_t("Accept")] = responseHttpContentType;

    std::shared_ptr<IHttpBody> httpBody;

    if(!json.empty())
        httpBody = std::shared_ptr<IHttpBody>(new JsonBody(web::json::value::string(json)));

    return p_ApiClient->callApi(path, utility::conversions::to_string_t("DELETE"), queryParams, httpBody, headerParams, formParams, fileParams, requestHttpContentType)
    .then([=](web::http::http_response response)
    {
        // 1xx - informational : OK
        // 2xx - successful       : OK
        // 3xx - redirection   : OK
        // 4xx - client error  : not OK
        // 5xx - client error  : not OK
        if (response.status_code() >= 400)
        {
            throw ApiException(response.status_code()
                , utility::conversions::to_string_t("error doGet: ") + response.reason_phrase()
                , std::make_shared<std::stringstream>(response.extract_utf8string(true).get()));
        }

        // check response content type
        if(response.headers().has(utility::conversions::to_string_t("Content-Type")))
        {
            utility::string_t contentType = response.headers()[utility::conversions::to_string_t("Content-Type")];
            if(0 != responseHttpContentType.compare(contentType))
            {
                throw ApiException(500
                    , utility::conversions::to_string_t("error doGet: unexpected response type: ") + contentType
                    , std::make_shared<std::stringstream>(response.extract_utf8string(true).get()));
            }
        }

        return void();
    });
}

pplx::task<void> RestClient::doPut(std::string path, std::string json)
{
    if(path.empty() || json.empty())
        throw std::runtime_error("Invalid argument");

    if (responseHttpContentType.empty())
        throw std::runtime_error("responseContentType is blank");

    if (requestHttpContentType.empty())
        //throw ApiException(415, utility::conversions::to_string_t("CommonApi->subscribe does not consume any supported media type"));
        throw std::runtime_error("requestHttpContentType is blank");

    std::map<utility::string_t, utility::string_t> queryParams;
    std::map<utility::string_t, utility::string_t> headerParams(p_ApiConfiguration->getDefaultHeaders());
    std::map<utility::string_t, utility::string_t> formParams;
    std::map<utility::string_t, std::shared_ptr<HttpContent>> fileParams;


    headerParams[utility::conversions::to_string_t("Accept")] = responseHttpContentType;

    std::shared_ptr<IHttpBody> httpBody;

    if(!json.empty())
        httpBody = std::shared_ptr<IHttpBody>(new JsonBody(web::json::value::string(json)));

    return p_ApiClient->callApi(path, utility::conversions::to_string_t("PUT"), queryParams, httpBody, headerParams, formParams, fileParams, requestHttpContentType)
    .then([=](web::http::http_response response)
    {
        // 1xx - informational : OK
        // 2xx - successful       : OK
        // 3xx - redirection   : OK
        // 4xx - client error  : not OK
        // 5xx - client error  : not OK
        if (response.status_code() >= 400)
        {
            throw ApiException(response.status_code()
                , utility::conversions::to_string_t("error doGet: ") + response.reason_phrase()
                , std::make_shared<std::stringstream>(response.extract_utf8string(true).get()));
        }

        // check response content type
        if(response.headers().has(utility::conversions::to_string_t("Content-Type")))
        {
            utility::string_t contentType = response.headers()[utility::conversions::to_string_t("Content-Type")];
            if(0 != responseHttpContentType.compare(contentType))
            {
                throw ApiException(500
                    , utility::conversions::to_string_t("error doGet: unexpected response type: ") + contentType
                    , std::make_shared<std::stringstream>(response.extract_utf8string(true).get()));
            }
        }

        return void();
    });
}

void RestClient::setResponseHttpContentType(std::string type)
{
    if(type.empty())
        return;

    responseHttpContentType = type;
}

std::string RestClient::getResponseHttpContentType()
{
    return responseHttpContentType;
}

void RestClient::setRequestHttpContentType(std::string type)
{
    if(type.empty())
        return;

    requestHttpContentType = type;
}

std::string RestClient::getRequestHttpContentType()
{
    return requestHttpContentType;
}

void RestClient::setBaseUrl(std::string baseUrl)
{
    if(baseUrl.empty())
        return;

    this->p_ApiConfiguration->setBaseUrl(baseUrl);
}
}/*namespace xapp*/
