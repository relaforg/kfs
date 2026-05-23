/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtol.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 10:26:38 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/22 15:28:46 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ctype.h"

long	strtol(char *s, char **end)
{
	long	out;
	int		sign;

	out = 0;
	sign = 1;
	while (isspace(*s))
		s++;
	if (*s == '-' || *s == '+')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}
	*end = s;
	while (isdigit(*s))
	{
		out = out * 10 + (*s - '0');
		s++;
		*end = s;
	}
	return (out * sign);
}
