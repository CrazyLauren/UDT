/*
 * socket_parser.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.03.2014
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef SOCKET_PARESER_H_
#define SOCKET_PARESER_H_
namespace NSHARE
{
SHARE_EXPORT std::vector<ISocket*> get_sockets(CConfig const& aChild);
}

#endif /* SOCKET_PARESER_H_ */
