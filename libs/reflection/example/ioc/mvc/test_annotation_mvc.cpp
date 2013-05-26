//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include "ioc.hpp"
#include "mvc_dispatcher_interface.hpp"

int main(int argc, char* argv[])
{
    try {
        std::list<std::string> xmls;
        for(int i = 0; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-xml") {
                xmls.push_back(argv[++i]);
            }
        }

        ApplicationContext* ctx = new ApplicationContext(xmls);
        DispatcherServlet* dispatcher = ctx->getBean<DispatcherServlet*>("dispatcherServlet");
        std::map<std::string, std::string> params;
        params.insert(std::map<std::string, std::string>::value_type("id", "1-2-3"));
        ServletRequest* req = new ServletRequest("/foo/petQuery", params);
        ServletResponse* res = new ServletResponse;
        dispatcher->handle(req, res);
    } catch(std::exception& exc) {
        std::cout << exc.what() << std::endl;
        throw;
    }
    return 0;
}
