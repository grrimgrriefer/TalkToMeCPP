#include "WebsocketPotato.h"

namespace Utility::AudioInput
{
	std::ostream& operator<< (std::ostream& out, connection_metadata const& data)
	{
		out << "> URI: " << data.m_uri << "\n"
			<< "> Status: " << data.m_status << "\n"
			<< "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
			<< "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);

		return out;
	}
}