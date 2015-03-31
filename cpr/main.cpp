#include <cpr.h>

#include <iostream>


int main() {
    auto response = cpr::Get(Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                             Parameters{{"anon", "true"}, {"key", "value"}});
    std::cout << "Status code: " << response.status_code << std::endl;
    std::cout << "Url: " << response.url << std::endl;
    std::cout << "Elapsed: " << response.elapsed << " seconds" << std::endl;
    std::cout << "Content-type: " << response.header["content-type"] << std::endl;
    std::cout << std::endl << "Response: " << std::endl << std::endl << response.text << std::endl;
}
