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
#ifndef _pistacheserver_h
#define _pistacheserver_h
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/http_headers.h>
#include<vector>
#include<string>
#include<memory>
#include<thread>
#include<vector>
#include<unordered_set>
#include<assert.h>
#include<functional>
#include<unordered_map>
namespace xapp
{
    using usr_callback=void(*)(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
    class pistacheserver
    {
        private:
            std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint;
            Pistache::Rest::Router router;
            void setupRoutes_get(int index);
            void setupRoutes_post(int index);
            void setupRoutes_del(int index);

            std::vector<std::string> method;
	    std::vector< bool> static_routing;
            std::vector<bool> dynamic_routing;
            std::unordered_map<int,std::string> route_static;
	    std::unordered_map<int,std::string> route_dynamic;
	    std::unordered_map<int,usr_callback> cb_static;
	    std::unordered_map<int,usr_callback> cb_dynamic;

	    std::string base = "/ric/v1";
            std::string dynamic_id="/:Id";
	    int stat_sz=0;
	    int dyn_sz=0;
	    int cal_stat_size();
	    int cal_dyn_size();
	    std::thread **t;
        public:
	    void thread_start();
            pistacheserver(Pistache::Address addr,std::vector<std::string> method,std::vector<bool> static_routing,std::vector<bool> dynamic_routing);
	    virtual ~pistacheserver(){};
	    virtual void default_handler(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response)
		 {
        		response.send(Pistache::Http::Code::Not_Found, "The requested method does not exist");
   		 }

            void init(size_t thr);
            void setup_base_url(std::string base);
            void setup_static_route(std::unordered_map<int,std::string> route);//routing path
	    void setup_dynamic_route(std::unordered_map<int,std::string> route);
	    void add_static_cb(std::unordered_map<int,usr_callback> cb);
	    void add_dynamic_cb(std::unordered_map<int,usr_callback> cb);
            void start();
            void shutdown();
    };
}
#endif

