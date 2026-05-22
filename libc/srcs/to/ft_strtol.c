/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtol.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 10:26:38 by relaforg          #+#    #+#             */
/*   Updated: 2025/11/24 14:37:33 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/libft.h"

long	ft_strtol(char *s, char **end)
{
	long	out;
	int		sign;

	out = 0;
	sign = 1;
	while (ft_isspace(*s))
		s++;
	if (*s == '-' || *s == '+')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}
	*end = s;
	while (ft_isdigit(*s))
	{
		out = out * 10 + (*s - '0');
		s++;
		*end = s;
	}
	return (out * sign);
}
