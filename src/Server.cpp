#include <boost/asio.hpp>
#include <iostream>

boost::asio::io_service io_service;
boost::asio::ip::tcp::resolver resolver(io_service);
boost::asio::ip::tcp::acceptor acceptor(io_service);
boost::asio::ip::tcp::socket tcp_socket(io_service);

void onAccept(const boost::system::error_code &ec) {
	if (ec)
	{
		std::cout << "Failed to accept with message " << ec.message() << std::endl;
	} else
	{
		std::stringstream res;
//		res << R"("{"greeting":"Hello World"}")";
		res << "Hello World!\n";
		tcp_socket.send(boost::asio::buffer(res.str()));
		std::cout << res.str();
	}
}

int main() {
	boost::asio::ip::tcp::resolver::query query("127.0.0.1", "8888");
	boost::asio::ip::tcp::endpoint endpoint = *(resolver.resolve(query));
	acceptor.open(endpoint.protocol());
	acceptor.bind(endpoint);
	acceptor.listen();

	acceptor.async_accept(tcp_socket, onAccept);

	io_service.run();

	return 0;
}
