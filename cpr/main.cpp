#include <cpr.h>

#include <iostream>


int main() {
    auto response = cpr::Post(Url{"https://posttestserver.com/post.php"},
                              Payload{{"foo", "bar"}, {"foz", "baz"}});
    std::cout << "Status code: " << response.status_code << std::endl;
    std::cout << "Url: " << response.url << std::endl;
    std::cout << "Elapsed: " << response.elapsed << " seconds" << std::endl;
    std::cout << "Content-type: " << response.header["content-type"] << std::endl;
    std::cout << std::endl << "Response: " << std::endl << std::endl << response.text << std::endl;
}
