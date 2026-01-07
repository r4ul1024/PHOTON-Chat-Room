#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

boost::asio::io_context io;
boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 1234);
boost::asio::ip::tcp::acceptor acceptor(io, endpoint);
std::mutex mtx;

std::unordered_map<std::shared_ptr<boost::asio::ip::tcp::socket>, std::string>
    users;

int id = 0;

void write_handler(std::shared_ptr<boost::asio::ip::tcp::socket> clientSocket,
                   std::string message) {
    auto first = message.find(" ");
    std::string username = message.substr(0, first);
    message.erase(0, first + 1);

    for (auto &user : users) {
        if (user.first == clientSocket) {
            message = user.second + " " + message + "\n";
        }
    }

    for (auto &user : users) {
        if (user.first != clientSocket) {
            if (user.second == username) {
                user.first->async_write_some(
                    boost::asio::buffer(message),
                    [user](const boost::system::error_code &ec, size_t) {
                        if (ec) {
                            std::cout << "Write error: " << ec.message()
                                      << "\n";
                        }
                    });
            }
        }
    }
}

void read_handler(std::shared_ptr<boost::asio::ip::tcp::socket> clientSocket,
                  std::shared_ptr<std::vector<char>> data, int id) {

    clientSocket->async_read_some(
        boost::asio::buffer(*data),
        [clientSocket, data, id](const boost::system::error_code &ec,
                                 size_t length) {
            if (!ec) {
                std::string buffer;
                buffer.append(data->data(), length);

                auto pos = buffer.find("\n");
                if (pos != std::string::npos) {
                    std::string message = buffer.substr(0, pos);
                    buffer.erase(0, pos + 1);

                    write_handler(clientSocket, message);
                }

                read_handler(clientSocket, data, id);
            } else {
                std::cerr << "Read error: " << ec.message() << "\n";
            }
        });
}

void client_accept() {
    auto clientSocket = std::make_shared<boost::asio::ip::tcp::socket>(io);
    acceptor.async_accept(
        *clientSocket, [clientSocket](const boost::system::error_code &ec) {
            if (!ec) {
                id++;
                auto data = std::make_shared<std::vector<char>>(1024);
                users.insert({clientSocket, std::to_string(id)});
                clientSocket->async_write_some(
                    boost::asio::buffer(std::to_string(id)),
                    [&](const boost::system::error_code &ec, size_t) {
                        if (ec) {
                            std::cout << "Write error: " << ec.message()
                                      << "\n";
                        }
                    });
                std::cout << "Client" << id << " connected\n";
                read_handler(clientSocket, data, id);
                client_accept();
            } else {
                std::cerr << "Accept error: " << ec.message() << "\n";
            }
        });
}

int main() {
    client_accept();

    io.run();
    return 0;
}
