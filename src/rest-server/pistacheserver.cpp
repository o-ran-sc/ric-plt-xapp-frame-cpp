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
#include"pistacheserver.h"
namespace xapp
{
 
	pistacheserver::pistacheserver(Pistache::Address addr,std::vector<std::string> method,std::vector<bool> static_routing,std::vector<bool> dynamic_routing)
        : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr))
    { 
	this->t=new std::thread*[1];
        this->method=method;
	this->static_routing=static_routing;
	this->dynamic_routing=dynamic_routing;
	assert(static_routing.size()==method.size() && dynamic_routing.size()==method.size());
	stat_sz=cal_stat_size();
	dyn_sz=cal_dyn_size();
    }
   int pistacheserver::cal_stat_size()//caclualting no of true in static_routing
	{
		int size=0;
		for(int i=0;i<static_routing.size();i++)
		{
			if (static_routing[i])
				size++;
		}
		return size;
	}

	 int pistacheserver::cal_dyn_size() //calaculating no true in dynamic_routing
        {
                int size=0;
                for(int i=0;i<dynamic_routing.size();i++)
                {
                        if (dynamic_routing[i])
                                size++;
                }
                return size;
        }

    void pistacheserver::setup_base_url(std::string base)
    {
        this->base=base;
    }
    void pistacheserver::setup_static_route(std::unordered_map<int,std::string> route)//routing path
    {
        this->route_static=route;
    }
 void pistacheserver::setup_dynamic_route(std::unordered_map<int,std::string> route)//routing path
    {
        this->route_dynamic=route;
    }
void pistacheserver::add_static_cb(std::unordered_map<int,usr_callback> cb)
{
	this->cb_static=cb;
}
void pistacheserver::add_dynamic_cb(std::unordered_map<int,usr_callback> cb) 
{
        this->cb_dynamic=cb;
}

    void pistacheserver::init(size_t thr = 2) 
    {
       		 auto opts = Pistache::Http::Endpoint::options().threads(thr).flags(Pistache::Tcp::Options::ReuseAddr)/*.flags(Pistache::Tcp::Options::ReusePort)*/; // how many threads for the server
       		 httpEndpoint->init(opts);
		assert(route_static.size()==stat_sz && route_dynamic.size()==dyn_sz); //no of true in satatic_routing and dynamic_routig should match with size of route_static and  route_dynamic respectively. 

		assert(cb_static.size()==stat_sz && cb_dynamic.size()==dyn_sz); //no of true in satatic_routing and dynamic_routig should match with size of cb_static and cb_dynamic respectively.
		for (int i=0;i<method.size();i++)
	{
        	if (method[i]=="get")
        	{
            		setupRoutes_get(i+1);
        	}
        	else if (method[i]=="post" )
        	{
            		setupRoutes_post(i+1);
        	}

        	else if (method[i]=="del")
        	{
            		setupRoutes_del(i+1);
        	}
	
       		else
        	{
            		std::cout<<"Wrong Method called \n";
        	}
    	}


    }


    void pistacheserver::start()
    {
	httpEndpoint->setHandler(router.handler());
        t[0]=new std::thread(&pistacheserver::thread_start,this);
	t[0]->detach();
    }
    void pistacheserver::thread_start()
    {
       this->httpEndpoint->serve();

    }

    void pistacheserver::shutdown()
    {
        httpEndpoint->shutdown();
    }

    void pistacheserver::setupRoutes_get(int index)
    {
        using namespace Pistache::Rest;
	auto search_s=route_static.find(index);
        auto search_d=route_dynamic.find(index);
        auto cbs_search=cb_static.find(index);
        auto cbd_search=cb_dynamic.find(index);
        assert (search_s!=route_static.end() || search_d!=route_dynamic.end()); 
        if (static_routing[index-1])
                {
		std::cout<<"static routing get"<<std::endl;
                assert (search_s!=route_static.end() && cbs_search!=cb_static.end());
                Routes::Get(router, base + search_s->second, Routes::bind(cbs_search->second));
                }
        if (dynamic_routing[index-1])
                {
		std::cout<<"dynamic routing get"<<std::endl;
                assert (search_d!=route_dynamic.end() && cbd_search!=cb_dynamic.end());
                Routes::Get(router, base + search_d->second+ dynamic_id, Routes::bind(cbd_search->second));
                }

	// Default handler, called when a route is not found
        router.addCustomHandler(Routes::bind(&pistacheserver::default_handler, this));
    }
    void pistacheserver::setupRoutes_post(int index)
    {
        using namespace Pistache::Rest;
	auto search_s=route_static.find(index);
	auto search_d=route_dynamic.find(index);
	auto cbs_search=cb_static.find(index);
	auto cbd_search=cb_dynamic.find(index);
	assert (search_s!=route_static.end() || search_d!=route_dynamic.end()); 
	if (static_routing[index-1])
		{
		std::cout<<"static routing post"<<std::endl;
		assert (search_s!=route_static.end() && cbs_search!=cb_static.end());
        	Routes::Post(router, base + search_s->second, Routes::bind(cbs_search->second));
		}
	if (dynamic_routing[index-1])
		{
		std::cout<<"dynamic routing post"<<std::endl;
		assert (search_d!=route_dynamic.end() && cbd_search!=cb_dynamic.end());
		Routes::Post(router, base + search_d->second+ dynamic_id, Routes::bind(cbd_search->second));
		}
        // Default handler, called when a route is not found
        router.addCustomHandler(Routes::bind(&pistacheserver::default_handler, this));
    }
    void pistacheserver::setupRoutes_del(int index)
    {
        using namespace Pistache::Rest;
        auto search_s=route_static.find(index);
        auto search_d=route_dynamic.find(index);
        auto cbs_search=cb_static.find(index);
        auto cbd_search=cb_dynamic.find(index);
        assert (search_s!=route_static.end() || search_d!=route_dynamic.end()); 
        if (static_routing[index-1])
                {
		std::cout<<"static routing delete"<<std::endl;
                assert (search_s!=route_static.end() && cbs_search!=cb_static.end());
                Routes::Delete(router, base + search_s->second, Routes::bind(cbs_search->second));
                }
        if (dynamic_routing[index-1])
                {
		std::cout<<"dynamic routing delete"<<std::endl;
                assert (search_d!=route_dynamic.end() && cbd_search!=cb_dynamic.end());
                Routes::Delete(router, base + search_d->second+ dynamic_id, Routes::bind(cbd_search->second));
		}
	// Default handler, called when a route is not found
        router.addCustomHandler(Routes::bind(&pistacheserver::default_handler, this));
    }

}



