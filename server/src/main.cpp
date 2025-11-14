#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

boost::asio::io_context io;
boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 1234);
boost::asio::ip::tcp::acceptor acceptor(io, endpoint);
std::mutex mtx;

std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> users;
std::string message;

void read_handler(std::shared_ptr<boost::asio::ip::tcp::socket> clientSocket,
                  std::shared_ptr<std::vector<char>> data, size_t length,
                  const boost::system::error_code &ec, int id) {
    if (!ec) {
        std::cout << "User" << id << " " << std::string(data->data(), length)
                  << "\n";

        message = "User" + std::to_string(id) + " " +
                  std::string(data->data(), length);
        for (auto &user : users) {
            if (user != clientSocket) {
                user->async_write_some(
                    boost::asio::buffer(message),
                    [user](const boost::system::error_code &ec, size_t) {
                        if (ec) {
                            std::cout << "Write error: " << ec.message()
                                      << "\n";
                        }
                    });
            }
        }
        clientSocket->async_read_some(
            boost::asio::buffer(*data),
            [clientSocket, data, id](const boost::system::error_code &ec,
                                     size_t length) {
                read_handler(clientSocket, data, length, ec, id);
            });

    } else {
        std::cout << "Read error: " << ec.message() << "\n";
    }
}

void client_handler(std::shared_ptr<boost::asio::ip::tcp::socket> clientSocket,
                    const boost::system::error_code &ec, int id) {
    if (!ec) {
        std::lock_guard<std::mutex> lock(mtx);
        {
            id++;
            users.push_back(clientSocket);
        }
        std::cout << "User" << id << " connected\n";

        clientSocket->async_write_some(
            boost::asio::buffer(std::to_string(id)),
            [id](const boost::system::error_code &ec, size_t) {
                if (ec) {
                    std::cout << "Write error: " << ec.message() << "\n";
                }
            });

        auto data = std::make_shared<std::vector<char>>(1024);
        clientSocket->async_read_some(
            boost::asio::buffer(*data),
            [clientSocket, data, id](const boost::system::error_code &ec,
                                     size_t length) {
                read_handler(clientSocket, data, length, ec, id);
            });

        auto newClientSocket =
            std::make_shared<boost::asio::ip::tcp::socket>(io);
        acceptor.async_accept(
            *newClientSocket,
            [newClientSocket, id](const boost::system::error_code &ec) {
                client_handler(newClientSocket, ec, id);
            });
    } else {
        std::cout << "Accept error: " << ec.message() << "\n";
    }
}

int main() {
    int id = 0;
    std::cout << "Server started\n";

    auto clientSocket = std::make_shared<boost::asio::ip::tcp::socket>(io);
    acceptor.async_accept(
        *clientSocket, [clientSocket, id](const boost::system::error_code &ec) {
            client_handler(clientSocket, ec, id);
        });

    io.run();
    return 0;
}
