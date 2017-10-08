/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lst_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/11/04 11:09:27 by jhalford          #+#    #+#             */
/*   Updated: 2017/05/16 21:00:09 by ariard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lst_print(t_list *list, void (*printer)())
{
	while (list)
	{
		ft_putstr("[");
		(*printer)(list->content);
		ft_putstr("]->");
		list = list->next;
	}
	ft_putendl("X\n");
}
