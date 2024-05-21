// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioCaptureDevice.h"
#include <SDKDDKVer.h>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace Utility::AudioInput
{
	class MicrophoneWebSocket
	{
	public:
		std::unique_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> ws;

		void OpenSocket()
		{
			std::string host = "localhost";
			boost::asio::io_context ioc;
			boost::asio::ip::tcp::resolver resolver{ ioc };
			ws = std::make_unique<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>>(ioc);

			auto const results = resolver.resolve(host, "5384");
			auto ep = boost::asio::connect(ws->next_layer(), results);
			host += ':' + std::to_string(ep.port());

			ws->set_option(boost::beast::websocket::stream_base::decorator(
				[] (boost::beast::websocket::request_type& req)
				{
					req.set(boost::beast::http::field::user_agent,
					std::string(BOOST_BEAST_VERSION_STRING) +
					" websocket-client-coro");
				}));

			ws->handshake(host, "/ws/audio/input/stream");
			ws->write(boost::asio::buffer(std::string("{\"contentType\":\"audio / wav\", \"sampleRate\" : 16000, \"channels\" : 1, \"bitsPerSample\" : 16, \"bufferMilliseconds\" : 30}")));
		}

		void CloseSocket()
		{
			// The make_printable() function helps print a ConstBufferSequence
			std::cout << "socket closing" << std::endl;

			// Close the WebSocket connection
			ws->close(boost::beast::websocket::close_code::normal);
		}

		void StartStreaming(AudioCaptureDevice& audioDevice)
		{
			audioDevice.startStream();

			// Continuously capture audio data and send it over the WebSocket
		}

		void StopStreaming()
		{
		}
	};
}