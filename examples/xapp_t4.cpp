#include <iostream>
#include <thread>
#include "ricxfcpp/message.hpp"
#include "ricxfcpp/msg_component.hpp"
#include <ricxfcpp/metrics.hpp>
#include "ricxfcpp/xapp.hpp"
#include "ricxfcpp/restapi_client.hpp"
#include "ricxfcpp/restapi_server.hpp"

#define SUBMGR_HTTP "http://service-ricplt-submgr-http.ricplt:8088/ric/v1"
#define APPMGR_HTTP "http://service-ricplt-appmgr-http.ricplt:8080/ric/v1"
#define SERVICE_HTTP_PORT "SERVICE_RICXAPP_%s_HTTP_PORT"
#define RMR_HTTP_PORT "SERVICE_RICXAPP_%s_RMR_PORT"

using namespace xapp;
using namespace xapp::RestModel;
using namespace io::swagger;

void subscribeXapp()
{
	auto mSub = std::make_shared<SubscriptionParams>();

	auto mSub_ClientEndpoint = std::make_shared<SubscriptionParams_ClientEndpoint>();
	auto mSub_E2SubscriptionDirectives = std::make_shared<SubscriptionParams_E2SubscriptionDirectives>();
	std::vector<std::shared_ptr<SubscriptionDetail>> m_SubscriptionDetail_List;
	auto  m_SubscriptionDetail = std::make_shared<SubscriptionDetail>();

	auto  m_SubsequentAction = std::make_shared<SubsequentAction>();
	m_SubsequentAction->setSubsequentActionType("continue")
		.setTimeToWait("w10ms");


	std::vector<int32_t> m_EventTrigger;
	std::vector<int32_t> m_ActionDefinition;

	std::vector<std::shared_ptr<ActionToBeSetup>> m_ActionToBeSetup_List;
	std::shared_ptr<ActionToBeSetup> m_ActionToBeSetup = std::make_shared<ActionToBeSetup>();

	mSub_ClientEndpoint->setHost("service-ricxapp-testrest-http.ricxapp")
						.setHTTPPort(8080)
						.setRMRPort(4560);

	mSub_E2SubscriptionDirectives->setE2RetryCount(3)
								.setE2TimeoutTimerValue(10)
								.setRMRRoutingNeeded(true);

	m_EventTrigger.push_back(1234);
	m_ActionDefinition.push_back(5678);
	m_ActionToBeSetup->setActionID(1)
		.setActionType("report")
		.setActionDefinition(m_ActionDefinition)
		.setSubsequentAction(m_SubsequentAction);

	m_ActionToBeSetup_List.push_back(m_ActionToBeSetup);

	m_SubscriptionDetail->setXappEventInstanceId(1)
		.setEventTriggers(m_EventTrigger)
		.setActionToBeSetupList(m_ActionToBeSetup_List);

	m_SubscriptionDetail_List.push_back(m_SubscriptionDetail);

	mSub->setSubscriptionId("")
		.setMeid("gnb_230_1270_b5c67788")
		.setRANFunctionID(0)
		.setClientEndpoint(mSub_ClientEndpoint)
		.setSubscriptionDetails(m_SubscriptionDetail_List);

	auto client  = std::make_shared<RestClient>(SUBMGR_HTTP);
	std::string path = "/subscriptions";
    client->Subscribe(mSub);
    /*
	client->doPost(path, mSub->toJson().serialize()).then([=](pplx::task<std::string> response)
	{
		try
		{
			std::cout << "doSubscribe successful" << "\n";
			std::cout << response.get() << std::endl;
		}
		catch (const std::exception& e) {
			std::cout <<"doSubscribe catch exception: " << e.what() << std::endl;
		}
	}).wait();
    */
}

void registerXapp()
{
	std::string config_path;
	std::string baseUrl(APPMGR_HTTP);
	auto client = std::make_shared<RestClient>(APPMGR_HTTP);
	auto sp_register = std::make_shared<RegisterData>();
	if(const char* ptr = std::getenv("CONFIG_FILE"))
		config_path = ptr;
	else
		config_path = "./config-file.json";

	std::ifstream t(config_path);
	if(t.fail())
	{
		std::cout << "Cannot open config file: " << config_path << "\n";
		return;
	}
	std::stringstream buffer;
	buffer << t.rdbuf();

	sp_register->setAppName("testrest")
			.setHttpEndpoint("<none>:8080")
			.setRmrEndpoint("<none>:4560")
			.setAppInstanceName("testrest")
			.setAppVersion("1.0.0")
			.setConfigPath("")
			.setConfig(buffer.str());
	std::cout << sp_register->toJson().serialize() << std::endl;
	std::string path = "/register";
	client->doPost(path, sp_register->toJson().serialize()).then([=](pplx::task<std::string> response)
	{
		try
		{
			std::cout << "doPost successful" << "\n";
			std::cout << response.get() << std::endl;
		}
		catch (const std::exception& e) {
			std::cout << __func__ << " catch exception: " << e.what() << std::endl;
		}
	}).wait();
}

void unsubscribeXapp(std::string subId)
{
	auto client = std::make_shared<RestClient>(SUBMGR_HTTP);
	client->Unsubscribe(subId);
}

void handler(const HttpRequest& req, HttpResponse res)
{
    Async::Promise res_ = res.send(Http::Code::Ok, "Nhat Anh");
    return;
}
class NhatAnh
{
    public:
        void handler(const HttpRequest& req, HttpResponse res) {std::cout << "Testing..." << std::endl;};
};
int main( int argc, char** argv ) {
	Xapp* x;
	char*	port = (char *) "4560";
	int ai = 1;							// arg processing index
	int nthreads = 1;
	int httpport = 8080;
	//registerXapp();
	/*Start RMR server*/
	//x = new Xapp( port, true );
	//std::thread thread2(&Xapp::Run, x, nthreads);
	subscribeXapp();
	fprintf( stderr, "<XAPP> listening on port: %s\n", port );
	fprintf( stderr, "<XAPP> starting %d threads\n", nthreads );

	/*Start HTTP server*/
    NhatAnh nhatanh;
	std::string addr = "";
	std::shared_ptr<RestServer> xappserver = std::make_shared<RestServer>(addr, 8089);
    xappserver->addRoute("/ric/v1/health/alive", &handler, &nhatanh, HttpMethod::Get);
	std::thread thread1(&RestServer::Start, xappserver);

	thread1.join();
	//thread2.join();

	return 0;
}
