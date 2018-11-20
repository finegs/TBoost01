//
// sender.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <SDKDDKVer.h>

#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
//#include <boost/bind.hpp>
#include <thread>

constexpr short multicast_port = 30001;
constexpr int max_message_count = 1000000;
using boost::asio::ip::udp;

/*
class sender
{
public:
	sender(boost::asio::io_context& io_context,
		const boost::asio::ip::address& multicast_address)
		: endpoint_(multicast_address, multicast_port),
		socket_(io_context, endpoint_.protocol()),
		timer_(io_context),
		message_count_(0)
	{
		do_send();
	}

private:
	void do_send()
	{

		while (true) {

			std::ostringstream os;
			os << "SND : " << std::this_thread::get_id << " , Message " << message_count_++;
			message_ = std::move(os.str());
			os.clear();

			// added by S.G.K 20181120
			std::cout << message_ << std::endl;

			socket_.async_send_to(boost::asio::buffer(message_), endpoint_, [this](boost::system::error_code ec, std::size_t length) {
				if (ec) {
					std::cout << "Error : " << ec << std::endl;
				}
				//if (!ec && message_count_ < max_message_count)
				//	do_timeout();
			});

			if (message_count_ < max_message_count)
				do_timeout();
			else
				break;

		}
		//socket_.async_send_to(
		//	boost::asio::buffer(message_),
		//	endpoint_,
		//	boost::bind(&sender::handleAsyncSendResult, this, message_,
		//		boost::asio::placeholders::error,
		//		boost::asio::placeholders::bytes_transferred));
		//socket_.async_send_to(
		//	boost::asio::buffer(message_),
		//	endpoint_,
		//	[this](boost::system::error_code ec, std::size_t length) {
		//	if (!ec && message_count_ < max_message_count)
		//		do_timeout();
		//});
	}

	//void handleAsyncSendResult(boost::system::error_code ec, std::size_t length) {
	//	if (!ec && message_count_ < max_message_count)
	//		do_timeout();
	//}

	void do_timeout()
	{
		timer_.expires_after(std::chrono::milliseconds(100));
		timer_.async_wait(
			[this](boost::system::error_code ec)
		{
			if (ec) {
				std::cout << "TimeOut Error : " << ec << std::endl;
			}
			//if (!ec)
			//	do_send();
		});
	}

private:
	boost::asio::ip::udp::endpoint endpoint_;
	boost::asio::ip::udp::socket socket_;
	boost::asio::steady_timer timer_;
	int message_count_;
	std::string message_;
};

*/

/*
using boost::asio::ip::udp;

class multicast_sender : public std::enable_shared_from_this<multicast_sender> {
	udp::socket socket_;
	udp::endpoint endpoint_;
	boost::asio::streambuf buffer_;
public:
	multicast_sender(boost::asio::io_service& io_service, short port,
		udp::endpoint const& remote) :
		socket_(io_service, udp::endpoint(udp::v4(), port)),
		endpoint_(remote)
	{
	}
	void
		send(char const* data, size_t size)
	{
		std::ostreambuf_iterator<char> out(&buffer_);
		std::copy(data, data + size, out);

		socket_.async_send_to(buffer_, endpoint_,
			std::bind(&multicast_sender,
				shared_from_this(), std::placeholders::_1));
	}

	void
		handle_send(boost::system::error_code const& ec)
	{
	}
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: sender <multicast_address>\n";
			std::cerr << "  For IPv4, try:\n";
			std::cerr << "    sender 239.255.0.1\n";
			std::cerr << "  For IPv6, try:\n";
			std::cerr << "    sender ff31::8000:1234\n";
			return 1;
		}

		//boost::asio::io_service io_service;
		//multicast_sender s(io_service, boost::asio::ip::make_address(argv[1]));
		//io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}

*/


#include <string>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

class multicast_sender
{
public:

	multicast_sender(
		const std::string& address,
		const std::string& multicast_address,
		const unsigned short multicast_port)
		: work_(io_service_),
		multicast_endpoint_(
			boost::asio::ip::address::from_string(multicast_address),
			multicast_port),
		socket_(io_service_, boost::asio::ip::udp::endpoint(
			boost::asio::ip::address::from_string(address),
			0 /* any port */))
	{
		// Start running the io_service.  The work_ object will keep
		// io_service::run() from returning even if there is no real work
		// queued into the io_service.
		auto self = this;
		work_thread_ = std::thread([self]()
		{
			self->io_service_.run();
		});
	}

	~multicast_sender()
	{
		// Explicitly stop the io_service.  Queued handlers will not be ran.
		io_service_.stop();

		// Synchronize with the work thread.
		work_thread_.join();
	}

	void send(const char* data, const size_t size)
	{
		// Caller may delete before the async operation finishes, so copy the
		// buffer and associate it to the completion handler's lifetime.  Note
		// that the completion may not run in the event the io_servie is
		// destroyed, but the the completion handler will be, so managing via
		// a RAII object (std::shared_ptr) is ideal.
		auto buffer = std::make_shared<std::string>(data, size);
		socket_.async_send_to(boost::asio::buffer(*buffer), multicast_endpoint_,
			[buffer](
				const boost::system::error_code& error,
				std::size_t bytes_transferred)
		{
			std::cout << "SND : " << bytes_transferred << " bytes with " <<
				error.message() << std::endl;
		});
	}

private:
	boost::asio::io_service io_service_;
	boost::asio::io_service::work work_;
	boost::asio::ip::udp::endpoint multicast_endpoint_;
	boost::asio::ip::udp::socket socket_;
	std::thread work_thread_;

};

const int SND_BUF_SIZE = 8192;

int main(int argc, char* argv[])
{

	if (argc != 3)
	{
		std::cerr << "Usage: sender <listen_address> <multicast_address>\n";
		std::cerr << "  For IPv4, try:\n";
		std::cerr << "    receiver 0.0.0.0 239.255.0.1\n";
		std::cerr << "  For IPv6, try:\n";
		std::cerr << "    receiver 0::0 ff31::8000:1234\n";
		return 1;
	}

	multicast_sender sender(argv[1], argv[2], multicast_port);
	char* data = (char*)malloc(SND_BUF_SIZE);
	std::memset(data, 0, SND_BUF_SIZE);
	//sender.send(data, SIZE);

	// Give some time to allow for the async operation to complete 
	// before shutting down the io_service.
	long cnt = 0;
	std::string message;
	for (cnt = 0;cnt < max_message_count; cnt++) {

		//std::ostringstream os;
		//os << "SND : " << std::this_thread::get_id << " , Message " << cnt;

		std::memset(data, '\0', SND_BUF_SIZE);

		//memcpy(data, os.str(), )

		sprintf_s(data, SND_BUF_SIZE, "SND : %#08X, Message=%ld", std::this_thread::get_id(), cnt);

		sender.send(data, strlen(data));

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	free(data);

	return EXIT_SUCCESS;
}
