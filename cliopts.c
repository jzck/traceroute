/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cliopts_get.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <jack@crans.org>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/03/14 20:04:04 by jhalford          #+#    #+#             */
/*   Updated: 2017/04/02 20:45:11 by jhalford         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
**	void *data must be a structure starting with `int flag`
**	to do polymorphism with t_data_template !
*/

#include "cliopts.h"

t_cliopts	*cliopts_getmap_long(t_cliopts opt_map[], char *arg)
{
	int		i;

	i = -1;
	while (opt_map[++i].c)
		if (!strcmp(opt_map[i].str, arg))
			return (&opt_map[i]);
	return (NULL);
}

t_cliopts	*cliopts_getmap_short(t_cliopts opt_map[], char arg)
{
	int		i;

	i = -1;
	while (opt_map[++i].c)
		if (opt_map[i].c == arg)
			return (&opt_map[i]);
	return (NULL);
}

static char			*check_required(char ***av, char *arg)
{
	char	*ret;

	if (!av || !*av)
		return (NULL);
	if (!arg || !*arg || !*(arg + 1))
		return (*++(*av));
	ret = arg + 1;
	return (ret);
}

static int			cliopts_parse_short(
		char ***av, t_cliopts opt_map[], void *data)
{
	t_cliopts	*map;
	char		*arg;
	int			i;
	char		*tmp;

	arg = **av + 1;
	i = -1;
	while (arg[++i] && !(tmp = NULL))
	{
		if (!(map = cliopts_getmap_short(opt_map, arg[i])))
			return (ERR_SET(E_CO_INV, arg[i]));
		((t_data_template*)data)->flag |= map->flag_on;
		((t_data_template*)data)->flag &= ~map->flag_off;
		if (map->get)
		{
			if (map->arg_required && !(tmp = check_required(av, arg + i)))
				return (ERR_SET(E_CO_MISS, *arg));
			tmp = tmp ? tmp : **av;
			if ((map->get)(tmp, data))
				return (ERR_SET(E_CO_MISS, *arg));
			if (map->arg_required)
				break ;
		}
	}
	return (++(*av) ? 0 : 0);
}

static int			cliopts_parse_long(
		char ***av, t_cliopts opt_map[], void *data)
{
	t_cliopts	*map;
	char		*arg;
	char		*tmp;

	arg = **av + 2;
	tmp = NULL;
	if (!(map = cliopts_getmap_long(opt_map, arg)))
		return (ERR_SET(E_CO_INVL, arg));
	((t_data_template*)data)->flag |= map->flag_on;
	((t_data_template*)data)->flag &= ~map->flag_off;
	if (map->get)
	{
		if (map->arg_required && !(tmp = check_required(av, NULL)))
			return (ERR_SET(E_CO_MISS, *arg));
		if ((map->get)(tmp, data))
			return (ERR_SET(E_CO_MISSL, arg));
	}
	++(*av);
	return (0);
}

int					cliopts_get(char **av, t_cliopts opt_map[], void *data)
{
	if (!av)
		return (1);
	av++;
	while (av && *av)
	{
		if (strcmp(*av, "-") == 0 || (strcmp(*av, "--") == 0 && av++))
			break ;
		else if ((*av)[0] == '-' && (*av)[1] == '-')
		{
			if (cliopts_parse_long(&av, opt_map, data))
				return (1);
		}
		else if ((*av)[0] == '-')
		{
			if (cliopts_parse_short(&av, opt_map, data))
				return (1);
		}
		else
			break ;
	}
	((t_data_template*)data)->av_data = av;
	return (0);
}
