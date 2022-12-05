/*
# ==================================================================================
# Copyright (c) 2020 HCL Technologies Limited.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==================================================================================
*/
#include"RestClient.h"
namespace xapp
{
	void cpprestclient::SetbaseUrl(std::string url)
	{
		this->baseUrl = utility::conversions::to_string_t(url);
		this->Baseurl = url;
	}
	cpprestclient::cpprestclient(std::string base_url) {
		this->baseUrl = utility::conversions::to_string_t(base_url);
		this->Baseurl = base_url;
	}
	cpprestclient::cpprestclient(std::string base_url, std::function<void(web::http::http_request)>callback):listener{ utility::conversions::to_string_t(resp_url) }
	{
		this->baseUrl= utility::conversions::to_string_t(base_url);
		this->Baseurl = base_url;
		try
		{
			listener.support(web::http::methods::POST, [callback](web::http::http_request request) { callback(request); });
			ok_2_run = true;
                        /*
                        ok_2_run = true;
                        t = new std::thread*[1];
                        t[0] = new std::thread(&cpprestclient::response_listener, this);
                        t[0]->detach();
                        */
                        response_listener();

			
		}
		catch (std::exception const & e)
		{
			std::wcout << e.what() << std::endl;
		}

	}
	cpprestclient::cpprestclient(std::string base_url,std::string response_url, std::function<void(web::http::http_request)>callback):listener{ utility::conversions::to_string_t(response_url) }{
		
		this->baseUrl= utility::conversions::to_string_t(base_url);
                this->Baseurl = base_url;
                try
                {
                        listener.support(web::http::methods::POST, [callback](web::http::http_request request) { callback(request); });
                        /*
                        ok_2_run = true;
                        t = new std::thread*[1];
                        t[0] = new std::thread(&cpprestclient::response_listener, this);
                        t[0]->detach();
                        */
                        response_listener();



                }
                catch (std::exception const & e)
                {
                        std::wcout << e.what() << std::endl;
                }



	}

	/*
	void cpprestclient::start_response_listener() {
		try {
			t[0] = new std::thread(&cpprestclient::response_listener, this);
			t[0]->detach();
		}
		catch (std::exception const & e)
		{
			std::wcout << e.what() << std::endl;
		}
	}
	*/
	std::string cpprestclient::getBaseUrl()
	{
		return Baseurl;
	}
	void cpprestclient::response_listener() {
		try
		{
			this->listener.open().wait();
			//std::cout<<"kumar "<<"\n";
			//std::this_thread::sleep_for(std::chrono::seconds(20));
                       /*
			while (ok_2_run) {
				//std::cout<<"sandeeep ";

			}
                        */

		}
		catch (std::exception const & e)
		{
			std::wcout << e.what() << std::endl;
		}
	}
	void cpprestclient::stop_response_listener() {
		try
		{
			ok_2_run = false;
			listener.close();
			//delete[] t;
			std::cout << "\nclosed1\n";
		}
		catch (std::exception const & e)
		{
			std::wcout << e.what() << std::endl;
		}
	}
	oresponse_t cpprestclient::post_subscriptions(const nlohmann::json & json, std::string path) {
		oresponse_t res;
		res.status_code = 0;
		res.SubscriptionId = "";
		auto postJson = pplx::create_task([&]() {
			utility::stringstream_t s;
			s << json.dump().c_str();
			web::json::value sdk_json = web::json::value::parse(s);
			web::uri_builder uri(this->baseUrl + utility::conversions::to_string_t(path));
			auto addr = uri.to_uri().to_string();
			web::http::client::http_client client(addr);
			return client.request(web::http::methods::POST, U("/"), sdk_json.serialize(), U("application/json"));
		})
			.then([&](web::http::http_response response) {

			res.status_code = response.status_code();
			if (response.status_code() != 201) {
				throw std::runtime_error("Returned " + std::to_string(response.status_code()));
			}
			ucout << response.to_string() << "\n";
			response.headers().set_content_type(U("application/json"));
			return response.extract_json();
		})

			.then([&](web::json::value jsonObject) {
			res.SubscriptionId = utility::conversions::to_utf8string(jsonObject[U("SubscriptionId")].as_string());

			//validating subscription response against its schema
			std::string s = utility::conversions::to_utf8string(jsonObject.serialize());
			nlohmann::json  j= nlohmann::json::parse(s);
			xapp::model::SubscriptionResponse SResp;
			xapp::model::from_json(j, SResp);
			
			//sanity check
			nlohmann::json ans;
			xapp::model::to_json(ans, SResp);
			std::cout << ans.dump(4) << "\n";

		});
		try {
			postJson.wait();
		}
		catch (const std::exception& e) {
			
			printf("Error exception:%s\n", e.what());
		}
		return res;

	}
	int  cpprestclient::delete_subscriptions(std::string Id, std::string path)
	{
		int status_code=0;
		auto delJson = pplx::create_task([&]() {
			web::uri_builder uri(this->baseUrl + utility::conversions::to_string_t(path));
			auto addr = uri.to_uri().to_string();
			web::http::client::http_client client(addr);
			addr.append(utility::conversions::to_string_t(Id));
			//ucout << utility::string_t(U("making requests at: ")) << addr << std::endl;
			return client.request(web::http::methods::DEL);
		})


			.then([&](web::http::http_response response) {
			status_code = response.status_code();
			if (response.status_code() != 204) {
				throw std::runtime_error("Returned " + std::to_string(response.status_code()));
			}
		});

		try {
			delJson.wait();
		}
		catch (const std::exception& e) {
			printf("Error exception:%s\n", e.what());
		}

		return status_code;
	}
	response_t  cpprestclient::get_config(std::string path)
	{
		response_t res;
		res.status_code = 0;
		res.body = utility::conversions::to_string_t("");
		auto postJson = pplx::create_task([&]() {


			web::http::client::http_client client(this->baseUrl);


			return client.request(web::http::methods::GET, web::uri_builder(U("/")).append_path(utility::conversions::to_string_t(path)).to_string());
		})
			.then([&](web::http::http_response response) {

			res.status_code = response.status_code();
			if (response.status_code() != 200) {
				throw std::runtime_error("Returned " + std::to_string(response.status_code()));
			}
			ucout << response.to_string() << "\n";
			response.headers().set_content_type(U("application/json"));
			return response.extract_json();
		})

			.then([&](web::json::value jsonObject) {
			//validating configlist against its schema
			std::string s = utility::conversions::to_utf8string(jsonObject.serialize());
			res.body = nlohmann::json::parse(s);
			std::vector<xapp::model::XAppConfig> cfg;
			xapp::model::from_json(res.body, cfg);
			
			//sanity check
			nlohmann::json ans;
			xapp::model::to_json(ans, cfg);
			std::cout << ans.dump(4) << "\n";
		});
		try {
			postJson.wait();
		}
		catch (const std::exception& e) {
			printf("Error exception:%s\n", e.what());
		}
		return res;
	}
	response_t  cpprestclient::get_subscriptions(std::string path)
	{
		response_t res;
		res.status_code = 0;
		res.body = utility::conversions::to_string_t("");
		auto postJson = pplx::create_task([&]() {


			web::http::client::http_client client(this->baseUrl);


			return client.request(web::http::methods::GET, web::uri_builder(U("/")).append_path(utility::conversions::to_string_t(path)).to_string());
		})
			.then([&](web::http::http_response response) {

			res.status_code = response.status_code();
			if (response.status_code() != 200) {
				throw std::runtime_error("Returned " + std::to_string(response.status_code()));
			}
			ucout << response.to_string() << "\n";
			response.headers().set_content_type(U("application/json"));
			return response.extract_json();
		})

			.then([&](web::json::value jsonObject) {

			//validating subscription list  against its schema
			std::string s = utility::conversions::to_utf8string(jsonObject.serialize());
			res.body = nlohmann::json::parse(s);
			std::vector<xapp::model::SubscriptionData> subList;
			xapp::model::from_json(res.body, subList);

			//sanity check
			nlohmann::json ans;
			xapp::model::to_json(ans, subList);
			std::cout << ans.dump(4) << "\n";

		});
		try {
			postJson.wait();
		}
		catch (const std::exception& e) {
			printf("Error exception:%s\n", e.what());
		}
		return res;
	}
	response_t cpprestclient::do_post(const nlohmann::json & json, std::string path)
	{
		response_t res;
		res.status_code = 0;
		res.body = utility::conversions::to_string_t("");
		auto postJson = pplx::create_task([&]() {
			//conversion from nlhomann json to cpprestsdk json
			utility::stringstream_t s;
			s << json.dump().c_str();
			web::json::value sdk_json = web::json::value::parse(s);
			web::uri_builder uri(this->baseUrl + utility::conversions::to_string_t(path));
			auto addr = uri.to_uri().to_string();
			web::http::client::http_client client(addr);
			return client.request(web::http::methods::POST, U("/"), sdk_json.serialize(), U("application/json"));
		})
				.then([&](web::http::http_response response) {

					res.status_code = response.status_code();
					if (response.status_code() != 201) {
					throw std::runtime_error("Returned " + std::to_string(response.status_code()));
					}
					ucout << response.to_string() << "\n";
					response.headers().set_content_type(U("application/json"));
					return response.extract_json();
			})
				
				.then([&](web::json::value jsonObject) {
					//std::cout << "\nRecieved REST subscription response\n";
					//std::wcout << jsonObject.serialize().c_str() << "\n";
					//resp.body = jsonObject.serialize();

				//conversion from cpprestsdk json to nlhomann json
				std::string s = utility::conversions::to_utf8string(jsonObject.serialize());
				res.body = nlohmann::json::parse(s);
			});
			try {
				postJson.wait();
			}
			catch (const std::exception& e) {
				printf("Error exception:%s\n", e.what());
			}
			return res;
	}
	response_t cpprestclient::do_del(std::string Id, std::string path)
	{
		response_t res;
		res.status_code = 0;
		res.body = utility::conversions::to_string_t("");
		auto delJson = pplx::create_task([&]() {
			web::uri_builder uri(this->baseUrl + utility::conversions::to_string_t(path));
			auto addr = uri.to_uri().to_string();
			web::http::client::http_client client(addr);
			addr.append(utility::conversions::to_string_t(Id));
			//ucout << utility::string_t(U("making requests at: ")) << addr << std::endl;
			return client.request(web::http::methods::DEL);
		})

			
			.then([&](web::http::http_response response) {
				res.status_code = response.status_code();
				if (response.status_code() != 204) {
					throw std::runtime_error("Returned " + std::to_string(response.status_code()));
				}
				ucout << response.to_string() << "\n";
				response.headers().set_content_type(U("application/json"));
				return response.extract_json();
				//std::wcout << "Deleted: " << std::boolalpha << (response.status_code() == 204) << std::endl;
		})
			.then([&](web::json::value jsonObject) {

				//resp.body = jsonObject.serialize();
			std::string s = utility::conversions::to_utf8string(jsonObject.serialize());
			res.body = nlohmann::json::parse(s);
		});

		try {
			delJson.wait();
		}
		catch (const std::exception& e) {
			printf("Error exception:%s\n", e.what());
		}

		return res;

	}
	 response_t cpprestclient::do_get(std::string path)
	{
		response_t res;
		res.status_code = 0;
		res.body = utility::conversions::to_string_t("");
		auto postJson = pplx::create_task([&]() {


			web::http::client::http_client client(this->baseUrl);

			
			return client.request(web::http::methods::GET, web::uri_builder(U("/")).append_path(utility::conversions::to_string_t(path)).to_string());
		})
			.then([&](web::http::http_response response) {

			res.status_code = response.status_code();
			if (response.status_code() != 200) {
				throw std::runtime_error("Returned " + std::to_string(response.status_code()));
			}
			ucout << response.to_string() << "\n";
			response.headers().set_content_type(U("application/json"));
			return response.extract_json();
		})

			.then([&](web::json::value jsonObject) {
	
			//resp.body = jsonObject.serialize();
			std::string s = utility::conversions::to_utf8string(jsonObject.serialize());
			res.body = nlohmann::json::parse(s);
		});
		try {
			postJson.wait();
		}
		catch (const std::exception& e) {
			printf("Error exception:%s\n", e.what());
		}
		return res;
	}
}


