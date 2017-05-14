/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <jack@crans.org>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/04/22 14:10:24 by jhalford          #+#    #+#             */
/*   Updated: 2017/04/23 18:18:41 by jhalford         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "traceroute.h"

int 	g_pid=-1;

unsigned short checksum(void *b, int len)
{
	unsigned short	*buf = b;
	unsigned int	sum=0;
	unsigned short	result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return (result);
}

int		wait_for_reply(struct sockaddr_in *addr, unsigned char buf[1024])
{
	static int sd = 0;

	if (sd == 0)
	{
		if ((sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0)
		{
			perror("socket");
			exit(0);
		}
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));
	}
	bzero(buf, 1024);

	socklen_t	len = sizeof(*addr);
	return (recvfrom(sd, buf, 1024, 0, (struct sockaddr*)addr, &len));
}

struct icmp		*get_icmp(void *buf)
{
	struct ip	*ip = buf;
	struct icmp *icmp;
	struct s_packet	*pkt;

	int hlen = ip->ip_hl << 2;
	pkt = (struct s_packet*)(buf + hlen);
	icmp = &pkt->hdr;
	return (icmp);
}

int		traceroute(struct sockaddr_in *dest)
{
	int		ttl;
	int				sd;
	char	addrstr[INET_ADDRSTRLEN];
	unsigned char buf[1024];
	struct s_packet	pkt;
	struct sockaddr_in addr;
	struct timeval	t1, t2, trip;
	double diff;

	if ((sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0)
	{
		perror("socket");
		return (1);
	}
	if (fcntl(sd, F_SETFL, O_NONBLOCK) != 0)
		perror("Request non blocking IO");
	for (ttl = 1; ttl < 255; ttl++)
	{
		if (setsockopt(sd, 0, IP_TTL, &ttl, sizeof(ttl)) != 0)
			perror("set TTL option");
		printf("%2d ", ttl);
		fflush(stdout);
		int addrshow = 0;
		for (int probe = 0; probe < 3; probe++)
		{
			bzero(&pkt, sizeof(pkt));
			pkt.hdr.icmp_type = ICMP_ECHO;
			pkt.hdr.icmp_id = g_pid;
			pkt.hdr.icmp_seq = (ttl - 1) * 3 + probe;
			pkt.hdr.icmp_cksum = checksum(&pkt, sizeof(pkt));
			if (sendto(sd, &pkt, sizeof(pkt), 0, (struct sockaddr*)dest, sizeof(*dest)) <= 0)
				perror("sendto");

			if (gettimeofday(&t1, NULL) != 0)
				return (1);

			int bytes = wait_for_reply(&addr, buf);

			if (gettimeofday(&t2, NULL) != 0)
				return (1);

			if (bytes < 0)
			{
				printf(" *");
				fflush(stdout);
				continue ;
			}
			timersub(&t2, &t1, &trip);
			diff = (trip.tv_sec + trip.tv_usec / 1000000.0) * 1000.0;
			if (!addrshow)
			{
				char	hostname[1024];
				char	servname[20];

				if (getnameinfo((struct sockaddr*)&addr, sizeof(addr), hostname, sizeof(hostname), servname, sizeof(servname), 0))
				{
					perror("getnameinfo");
					exit(1);
				}
				printf(" %s (%s)", hostname, inet_ntop(AF_INET, &(addr.sin_addr), addrstr, INET_ADDRSTRLEN)); addrshow = 1;
			}
			printf("  %.3f ms", diff);
		}
		printf("\n");
		struct icmp *icmp = get_icmp(buf);
		if (icmp->icmp_type == ICMP_ECHOREPLY)
			break ;
	}
	exit(0);
}

int		main(int ac, char **av)
{
	struct sockaddr_in	*addr;
	struct addrinfo		*result, hints;
	char				addrstr[INET_ADDRSTRLEN];

	if (ac != 2)
	{
		printf("usage: %s <addr>\n", av[0]);
		exit(1);
	}
	memset (&hints, 0, sizeof (hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;
	if (getaddrinfo(av[1], NULL, &hints, &result) != 0)
	{
		perror("getaddrinfo");
		exit(1);
	}
	addr = (struct sockaddr_in*)result->ai_addr;
	inet_ntop(AF_INET, &(addr->sin_addr), addrstr, INET_ADDRSTRLEN);
	g_pid = getpid();
	if (result->ai_next)
		printf("traceroute: Warning: %s has multiple addresses; using %s\n", result->ai_canonname, addrstr);
	printf("traceroute to %s (%s), %i hops max %i byte packets\n",
			FT_TRY(result->ai_canonname, addrstr), addrstr, 64, PACKETSIZE);
	exit(traceroute(addr));
}
