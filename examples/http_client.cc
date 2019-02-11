/*
   Mathieu Stefani, 07 février 2016

 * Http client example
*/

#include <atomic>

#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/client.h>

using namespace Pistache;
using namespace Pistache::Http;

int main(int argc, char *argv[]) {

    std::string server_addr("127.0.0.1:9080");
    int reqs_count = 1;

    // Accepted parameters:
    //  - argv[1] -> server address
    //  - argv[2] -> requests to be made

    if (argc >= 2) {
        server_addr = std::string(argv[1]);
    }

    if (argc >= 3) {
        reqs_count = std::stoi(argv[2]);
    }

    std::cout << "Pistache webserver client is running:" << std::endl;
    std::cout << " - Server:        " << server_addr << std::endl;
    std::cout << " - Request count: " << reqs_count << std::endl;
    std::cout << std::endl;

    Http::Client client;

    auto opts = Http::Client::options()
        .threads(1)
        .maxConnectionsPerHost(8);
    client.init(opts);

    std::vector<Async::Promise<Http::Response>> responses;

    std::atomic<size_t> completedRequests(0);
    std::atomic<size_t> failedRequests(0);

    auto start = std::chrono::system_clock::now();

    for (int i = 0; i < reqs_count; ++i) {
        std::cout << "Sending cookies request # " << i + 1 << std::endl;
        auto resp = client.get(server_addr).cookie(Http::Cookie("FOO", "bar")).send();
        resp.then([&](Http::Response response) {
                ++completedRequests;
            std::cout << "  Response code = " << response.code() << std::endl;
            auto body = response.body();
            if (!body.empty())
               std::cout << "  Response body = " << body << std::endl;
        }, Async::IgnoreException);
        responses.push_back(std::move(resp));
    }

    auto sync = Async::whenAll(responses.begin(), responses.end());
    Async::Barrier<std::vector<Http::Response>> barrier(sync);

    barrier.wait_for(std::chrono::seconds(5));

    auto end = std::chrono::system_clock::now();
    std::cout << std::endl
              << "Summary of execution" << std::endl
              << "Total number of requests sent     : " << reqs_count << std::endl
              << "Total number of responses received: " << completedRequests.load() << std::endl
              << "Total number of requests failed   : " << failedRequests.load() << std::endl
              << "Total time of execution           : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    client.shutdown();
    return 0;
}
