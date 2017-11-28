/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <jack@crans.org>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/04/22 14:10:24 by jhalford          #+#    #+#             */
/*   Updated: 2017/10/08 17:55:05 by jhalford         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "traceroute.h"

int			g_pid;

static int	wait_for_reply(struct sockaddr *addr, unsigned char buf[1024])
{
	static int sd;;

	sd = 0;
	if (sd == 0)
	{
		if ((sd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
		{
			perror("socket");
			exit(1);
		}
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,
				(const char*)&tv, sizeof(struct timeval));
	}
	bzero(buf, 1024);
	return (recvfrom(sd, buf, 1024, 0, addr, NULL));
}

static int	probe_once(int sd, struct sockaddr *addr,
		struct s_packet *pkt, struct icmp *icmp)
{
	int				bytes;
	unsigned char	buf[1024];
	double			time;

	if (sendto(sd, &pkt, sizeof(pkt), 0,
				addr, sizeof(*addr)) <= 0)
	{
		perror("sendto");
		exit(1);
	}
	time = time_milli();
	bytes = wait_for_reply(addr, buf);
	time = time_milli() - time;
	if (bytes < 0)
		printf(" *");
	else
	{
		ip_load_icmp(icmp, buf);
		printf("  %.3f ms", time);
	}
	return (0);
}

static int	probe_depth(int sd, struct sockaddr *addr,
		struct s_packet pkt)
{
	struct icmp		icmp;
	int				probe;

	probe = -1;
	host_format(addr);
	while (++probe < 3)
	{
		pkt.hdr.icmp_seq++;
		pkt.hdr.icmp_cksum = cksum(&pkt, sizeof(pkt));
		probe_once(sd, addr, &pkt, &icmp);
	}
	printf("\n");
	return (icmp.icmp_type == ICMP_ECHOREPLY);
}

void		traceroute(struct sockaddr *addr)
{
	int				sd;
	int				ttl;
	struct s_packet	pkt;

	if ((sd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("socket");
		exit(1);
	}
	g_pid = getpid();
	bzero(&pkt, sizeof(pkt));
	pkt.hdr.icmp_type = ICMP_ECHO;
	pkt.hdr.icmp_id = g_pid;
	ttl = 0;
	while (++ttl < 255)
	{
		printf("%2d ", ttl);
		if (setsockopt(sd, 0, IP_TTL, &ttl, sizeof(ttl)) != 0)
		{
			perror("set TTL option");
			exit(1);
		}
		pkt.hdr.icmp_seq = (ttl - 1) * 3;
		if (probe_depth(sd, addr, pkt))
			break ;
	}
}
