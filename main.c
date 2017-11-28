/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <jack@crans.org>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/10/08 17:18:05 by jhalford          #+#    #+#             */
/*   Updated: 2017/10/08 17:55:15 by jhalford         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "traceroute.h"

int			g_pid = -1;

struct addrinfo	*resolve_host(char *hostname, char ip[])
{
	struct addrinfo		*result;
	struct addrinfo		hints;
	struct sockaddr_in	*addr;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;
	if (getaddrinfo(hostname, NULL, &hints, &result) != 0)
	{
		perror("getaddrinfo");
		exit(1);
	}
	addr = (struct sockaddr_in*)result->ai_addr;
	inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);
	return (result);
}

int				main(int ac, char **av)
{
	struct addrinfo		*result;
	char				ip[INET_ADDRSTRLEN];

	if (ac != 2)
	{
		ft_usage("%s <addr>\n", av[0]);
		return (1);
	}
	result = resolve_host(av[1], ip);
	if (result->ai_next)
		printf("traceroute: Warning: %s has multiple addresses; using %s\n",
				result->ai_canonname, ip);
	printf("traceroute to %s (%s), %i hops max %i byte packets\n",
			result->ai_canonname, ip, 64, PACKETSIZE);
	traceroute(result->ai_addr);
	return (0);
}

