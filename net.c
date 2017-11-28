#include "traceroute.h"

int		host_format(struct sockaddr *addr)
{
	char	dn[1024];
	char	sv[20];
	char	ip[INET_ADDRSTRLEN];

	if (getnameinfo(addr, sizeof(*addr), dn, sizeof(dn),
				sv, sizeof(sv), 0))
	{
		perror("getnameinfo");
		return (1);
	}
	printf(" %s (%s)", dn,
			inet_ntop(AF_INET, &(((struct sockaddr_in*)addr)->sin_addr),
				ip, INET_ADDRSTRLEN));
	return (0);
}

void	ip_load_icmp(struct icmp *icmp, void *buf)
{
	struct ip	*ip;
	int hlen;

	ip = buf;
   	hlen = ip->ip_hl << 2;
	memcpy(icmp, buf + hlen, sizeof(struct icmp));
}
