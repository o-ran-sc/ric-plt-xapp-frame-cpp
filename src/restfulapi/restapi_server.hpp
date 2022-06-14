#ifndef _restapi_server_hpp
#define _restapi_server_hpp

#include <iostream>
#include <functional>
#include <string>
#include <map>

#include <pistache/common.h>
#include <pistache/cookie.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/http_headers.h>
#include <pistache/net.h>
#include <pistache/peer.h>
#include <pistache/router.h>

namespace xapp {

using namespace Pistache;
typedef Rest::Request HttpRequest;
typedef Http::ResponseWriter HttpResponse;
typedef Http::Method HttpMethod;

class RestServer {
    private:
        std::shared_ptr<Http::Endpoint> sp_Endpoint;
        Rest::Router m_router;
        Http::Endpoint::Options m_options = Http::Endpoint::options();
    public:
        RestServer(std::string addr, int port);
        ~RestServer();
        void addRoute(const std::string& resource,\
                void (*handler)(const Rest::Request&, Http::ResponseWriter), Http::Method method);

        template<typename Obj>
        void addRoute(const std::string& resource,\
                void (*handler)(const Rest::Request&, Http::ResponseWriter), Obj *p_Obj, Http::Method method);

        Http::Endpoint::Options& setOptions();
        void Init();
        void Start();
        void Stop();
        void Reset();
};
};/*namespace xapp*/
#endif /*_restapi_server_hpp*/
