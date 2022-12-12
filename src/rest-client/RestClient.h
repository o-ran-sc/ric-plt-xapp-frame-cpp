#ifndef _CPPREST_SDK_WRAPPER_H
#define _CPPREST_SDK_WRAPPER_H

#include<string>
#include<cpprest/http_client.h>
#include<cpprest/filestream.h>
#include<cpprest/uri.h>
#include<cpprest/json.h>
#include<sstream>
#include <cpprest/http_listener.h>
#include"nlohmann/json.hpp"
#include<functional>
#include"RestModel.h"
#include<thread>
#include <chrono>
#include <pthread.h>


namespace xapp
{
	typedef struct resp
	{
		long status_code;
		nlohmann::json body;
		//utility::string_t body;
	}response_t;

	typedef struct oran_resp
	{
		long status_code;
		std::string SubscriptionId;
	}oresponse_t;

	class cpprestclient
	{
	protected:
		utility::string_t baseUrl;
		std::string Baseurl;
		//utility::string_t resp_url=U("http://0.0.0.0:8080/ric/v1/subscriptions/response");
		std::string resp_url = "http://0.0.0.0:8080/ric/v1/subscriptions/response";//default response url
		web::http::experimental::listener::http_listener listener;
		std::thread** t;
		void response_listener();
		bool ok_2_run;
	public:
		void SetbaseUrl(std::string);
		std::string getBaseUrl();
		//O-RAN specific api calls
		oresponse_t post_subscrtiptions(const nlohmann::json & json, std::string path);
		int delete_subscrtiptions(std::string Id, std::string path);
		response_t get_subscriptions(std::string path);
		response_t get_config(std::string path);

		//general api calls
		virtual response_t do_get(std::string path);
		virtual response_t do_post(const nlohmann::json & json, std::string path);
		virtual response_t do_del(std::string Id, std::string path);
		//virtual response_t do_post(const web::json::value & json, std::string path);
		cpprestclient(std::string base_url, std::function<void(web::http::http_request)>callback);
		cpprestclient(std::string base_url,std::string response_url, std::function<void(web::http::http_request)>callback);
		cpprestclient(std::string base_url);
		virtual ~cpprestclient(){}
		//void start_response_listener();
		void stop_response_listener();
	};

}





#endif
