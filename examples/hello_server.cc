/*
   Mathieu Stefani, 13 février 2016

   Example of an hello world server
*/


#include "pistache/endpoint.h"

using namespace Pistache;

class HelloHandler : public Http::Handler {
public:

    HTTP_PROTOTYPE(HelloHandler)

    void onRequest(const Http::Request& request, Http::ResponseWriter response) override{
        UNUSED(request);
        response.send(Pistache::Http::Code::Ok, "Hello World from Pistache!\n");
    }
};

int main(int argc, char *argv[]) {

    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));

    std::cout << "Pistache webserver is listening on " << addr.toString() << std::endl;
    std::cout << " - Issue 'curl http://" << addr.toString() << "'" << std::endl << std::endl;
    std::cout << std::endl;

    auto opts = Pistache::Http::Endpoint::options()
        .threads(1);

    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(Http::make_handler<HelloHandler>());
    server.serve();

    server.shutdown();

    UNUSED(argc);
    UNUSED(argv);

    return 0;
}
