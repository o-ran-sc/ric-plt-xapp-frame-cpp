#include "restapi_server.hpp"
namespace xapp {
using namespace Pistache;

RestServer::RestServer(std::string addr, int port)
{
    Address addr_;
    if(addr.empty())
        addr_ = Address(Ipv4::any(), port);
    else
        addr_ = Address(addr, port);

    sp_Endpoint = std::make_shared<Http::Endpoint>(addr_);
}

RestServer::~RestServer()
{
}

void RestServer::addRoute(const std::string& resource, \
        void (*handler)(const Rest::Request&, Http::ResponseWriter), Http::Method method)
{
    if(resource.empty() || NULL == handler)
        return;

    m_router.addRoute(method, resource, Rest::Routes::bind(handler));
    return;
}

Http::Endpoint::Options& RestServer::setOptions()
{
    return m_options;
}

void RestServer::Init()
{
    return;
}

void RestServer::Start()
{
    sp_Endpoint->init(m_options);
    sp_Endpoint->setHandler(m_router.handler());
    sp_Endpoint->serve();
    return;
}

void RestServer::Stop()
{
    return;
}

void RestServer::Reset()
{
    return;
}

} /*namespace xapp*/
