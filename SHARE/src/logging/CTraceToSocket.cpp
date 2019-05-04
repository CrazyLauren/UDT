// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 *
 * CTraceToSocket.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.8.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifdef CPLUS_LOG
#include <deftype>
#include <share_socket.h>
#include <log4cplus/log4judpappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/streams.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/version.h>
#include <log4cplus/loggingmacros.h>
#include <iomanip>
#include <cstring>
#if defined (UNICODE)
#include <cwctype>
#else
#include <cctype>
#endif
#include <logging/CTraceToSocket.h>

namespace NSHARE
{
using namespace log4cplus;
namespace //see log4judpappender.cxx
{
static inline bool is_control(tchar ch)
{
#if defined (UNICODE)
	return !! std::iswcntrl (std::char_traits<tchar>::to_int_type (ch));
#else
	return !!std::iscntrl(std::char_traits<tchar>::to_int_type(ch));
#endif
}

//! Outputs str with reserved XML characters escaped.
static
void output_xml_escaped(tostream & os, tstring const & str)
{
	for (tstring::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		tchar const & ch = *it;
		switch (ch)
		{
		case LOG4CPLUS_TEXT ('<'):
			os << LOG4CPLUS_TEXT ("&lt;");
			break;

		case LOG4CPLUS_TEXT ('>'):
			os << LOG4CPLUS_TEXT ("&gt;");
			break;

		case LOG4CPLUS_TEXT ('&'):
			os << LOG4CPLUS_TEXT ("&amp;");
			break;

		case LOG4CPLUS_TEXT ('\''):
			os << LOG4CPLUS_TEXT ("&apos;");
			break;

		case LOG4CPLUS_TEXT ('"'):
			os << LOG4CPLUS_TEXT ("&quot;");
			break;

		default:
			if (is_control(ch))
			{
				tchar const prev_fill = os.fill();
				std::ios_base::fmtflags const prev_flags = os.flags();
				os.flags(std::ios_base::hex | std::ios_base::right);
				os.fill(LOG4CPLUS_TEXT ('0'));

				os << std::setw(0) << LOG4CPLUS_TEXT ("&#x") << std::setw(2)
						<< std::char_traits<tchar>::to_int_type(ch)
						<< std::setw(0) << LOG4CPLUS_TEXT (";");

				os.fill(prev_fill);
				os.flags(prev_flags);
			}
			else
				os.put(ch);
		}
	}
}
//! Helper manipulator like class for escaped XML output.
struct outputXMLEscaped
{
	outputXMLEscaped(tstring const & s) :
			str(s)
	{
	}

	tstring const & str;
};

//! Overload stream insertion for outputXMLEscaped.
inline tostream &
operator <<(tostream & os, outputXMLEscaped const & x)
{
	output_xml_escaped(os, x.str);
	return os;
}
}
CTraceToSocket::CTraceToSocket(NSHARE::CConfig const& aConf)
{
	if (aConf.MIsEmpty())
		helpers::getLogLog().error(
				LOG4CPLUS_TEXT("No settings for socket. It cannot be opened."));
	layout.reset(new PatternLayout(LOG4CPLUS_TEXT ("%m")));
	MOpen(aConf);
}

CTraceToSocket::~CTraceToSocket()
{
	destructorImpl();
}
void CTraceToSocket::close()
{
	helpers::getLogLog().debug(
			LOG4CPLUS_TEXT("Entering Log4jUdpAppender::close()..."));
	if (FSocket)
		FSocket->MClose();
	closed = true;
}
void CTraceToSocket::MOpen(NSHARE::CConfig const& aConf)
{
	if (!FSocket)
	{
		helpers::getLogLog().debug(aConf.MToJSON(true).c_str());
		std::vector<NSHARE::ISocket*> _rval = get_sockets(aConf);
		if (_rval.empty())
		{
			helpers::getLogLog().error(
					LOG4CPLUS_TEXT("Cannot open socket by configure:"));
			helpers::getLogLog().error(aConf.MToJSON(true).c_str());
		}
		else
		{
			FSocket = SHARED_PTR<ISocket>(_rval.back());
			_rval.pop_back();
			std::vector<NSHARE::ISocket*>::iterator _it=_rval.begin(),_it_end(_rval.end());

			for(;_it!=_it_end;++_it)
			{
				std::stringstream _buf;
				(*_it)->MPrint(_buf);
				helpers::getLogLog().error(LOG4CPLUS_TEXT("The next socket will be ignored..."));
				helpers::getLogLog().error(_buf.str());
				delete (*_it);
				*_it=NULL;
			}

			std::stringstream _buf;
			FSocket->MPrint(_buf);
			helpers::getLogLog().debug(LOG4CPLUS_TEXT("Opened "));
			helpers::getLogLog().debug(_buf.str());
		}
		}
	}

static tostringstream g_buffer;
void CTraceToSocket::append(const spi::InternalLoggingEvent& event)
{
	if (!FSocket)
	{
		helpers::getLogLog().error(LOG4CPLUS_TEXT("CTraceToSocket::append()")
		LOG4CPLUS_TEXT("- Socket is exist"));

		return;
	}
	if (!FSocket->MIsOpen() && !FSocket->MReOpen())
	{
		helpers::getLogLog().error(LOG4CPLUS_TEXT("CTraceToSocket::append()")
		LOG4CPLUS_TEXT("- Cannot open the socket"));
		std::stringstream _buf;
		FSocket->MPrint(_buf);
		helpers::getLogLog().error(_buf.str());
		return;
	}
	{//convert to log4j protocol(xmpl)
		log4cplus::detail::clear_tostringstream (g_buffer);
		layout->formatAndAppend(g_buffer, event);
		tstring str;
		g_buffer.str().swap(str);

		log4cplus::detail::clear_tostringstream(g_buffer);
		g_buffer << LOG4CPLUS_TEXT("<log4j:event logger=\"")
				<< outputXMLEscaped(event.getLoggerName())
				<< LOG4CPLUS_TEXT("\" level=\"")
				// TODO: Some escaping of special characters is needed here.
				<< outputXMLEscaped(
						getLogLevelManager().toString(event.getLogLevel()))
				<< LOG4CPLUS_TEXT("\" timestamp=\"")
#if	LOG4CPLUS_VERSION>=LOG4CPLUS_MAKE_VERSION(2,0,0)
				<< log4cplus::helpers::getFormattedTime(LOG4CPLUS_TEXT("%s%q"),
						event.getTimestamp())
#else
				<< event.getTimestamp().getFormattedTime(LOG4CPLUS_TEXT("%s%q"))
#endif
				<< LOG4CPLUS_TEXT("\" thread=\"") << event.getThread()
				<< LOG4CPLUS_TEXT("\">")

				<< LOG4CPLUS_TEXT("<log4j:message>")
				// TODO: Some escaping of special characters is needed here.
				<< outputXMLEscaped(str) << LOG4CPLUS_TEXT("</log4j:message>")

				<< LOG4CPLUS_TEXT("<log4j:NDC>")
				// TODO: Some escaping of special characters is needed here.
				<< outputXMLEscaped(event.getNDC())
				<< LOG4CPLUS_TEXT("</log4j:NDC>")

				<< LOG4CPLUS_TEXT("<log4j:locationInfo class=\"\" file=\"")
				// TODO: Some escaping of special characters is needed here.
				<< outputXMLEscaped(event.getFile())
				<< LOG4CPLUS_TEXT("\" method=\"")
				<< outputXMLEscaped(event.getFunction())
				<< LOG4CPLUS_TEXT("\" line=\"") << event.getLine()
				<< LOG4CPLUS_TEXT("\"/>") << LOG4CPLUS_TEXT("</log4j:event>");
	}
	tstring _str;
	g_buffer.str().swap(_str);
	bool ret = FSocket->MSend(_str.c_str(), _str.length() + 1/*\0*/).MIs();//todo buffering
	if (!ret)
	{
		helpers::getLogLog().error(LOG4CPLUS_TEXT(
				"Log4jUdpAppender::append()- Cannot write to server"));
	}
}
} /* namespace NSHARE */
#endif
