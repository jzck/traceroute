/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <jack@crans.org>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/04/22 14:10:24 by jhalford          #+#    #+#             */
/*   Updated: 2017/10/08 17:52:17 by jhalford         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "error.h"
# include "cliopts.h"

# include <unistd.h>
# include <stdlib.h>

# include <sys/socket.h>
# include <sys/time.h>
# include <sys/types.h>

# include <fcntl.h>
# include <errno.h>
# include <resolv.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

#define PACKETSIZE	56

struct			s_packet
{
	struct icmp	hdr;
	char		msg[PACKETSIZE - sizeof(struct icmp)];
};

void			traceroute(struct sockaddr *addr);
unsigned short	cksum(void *b, int len);
int				host_format(struct sockaddr *addr);
void			ip_load_icmp(struct icmp *icmp, void *buf);
double			time_milli(void);
