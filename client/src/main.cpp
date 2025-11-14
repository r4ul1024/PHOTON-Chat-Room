#include <boost/asio.hpp>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

boost::asio::io_context io;
boost::asio::ip::tcp::socket serverSocket(io);

char data[1024];
std::string ip;
std::string username;
std::string message;

void logo();
void connect() {
    std::cout << "\033[32mEnter IP: \033[0m";
    std::getline(std::cin, ip);

    serverSocket.connect(boost::asio::ip::tcp::endpoint(
        boost::asio::ip::make_address(ip), 1234));

    std::cout << "\033[2J\033[H";
    logo();
    size_t length = serverSocket.read_some(boost::asio::buffer(data));
    username = std::string(data, length);
}

void logo() {
    std::cout << "\033[32m" << R"(
 ____  _   _  ___ _____ ___  _   _ 
|  _ \| | | |/ _ \_   _/ _ \| \ | |
| |_) | |_| | | | || || | | |  \| |
|  __/|  _  | |_| || || |_| | |\  |
|_|   |_| |_|\___/ |_| \___/|_| \_|
)";
    std::cout << "\033[0m\n\n";
}

void write_handler() {
    while (true) {
        std::cout << "Message: ";
        std::getline(std::cin, message);
        try {
            boost::asio::write(serverSocket, boost::asio::buffer(message));
        } catch (const std::exception &e) {
            std::cout << "\033[31mERROR: \033[0m" << e.what() << "\n";
            break;
        }
    }
}

int main() {
    std::cout << "\033[2J\033[H";
    while (true) {
        logo();
        try {
            connect();
            std::cout << "\033[2J\033[H";
            logo();
            std::cout
                << "\n\033[32mINFO: \033[0m"
                << "Successfully connected to the server, your username is: "
                   "\033[32m"
                << "User" << username << "\033[0m\n\n";
            break;
        } catch (std::exception &e) {
            std::cout << "\033[2J\033[H";
            std::cout << "\033[31mError: \033[0m" << e.what() << "\n";
        }
    }

    std::thread(write_handler).detach();

    while (true) {
        try {
            size_t length = serverSocket.read_some(boost::asio::buffer(data));
            std::cout << std::string(data, length) << "\n";

        } catch (const std::exception &e) {
            std::cout << "\033[31mERROR: \033[0m" << e.what() << "\n";
            break;
        }
    }
    return 0;
}
