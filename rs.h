/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rs.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <jack@crans.org>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/10/07 18:05:30 by jhalford          #+#    #+#             */
/*   Updated: 2017/10/08 15:59:58 by jhalford         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <strings.h>
# include <float.h>
# include <dlfcn.h>
# include <math.h>

typedef struct s_rs	t_rs;

struct				s_rs {
	int				count;
	double			min;
	double			max;
	double			avg;
	double			m;
	double			stdev;
	double			var;
};

void					rs_init(t_rs *rs);
void					rs_push(t_rs *rs, double n);
void					rs_final(t_rs *rs);
