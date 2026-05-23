/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtoi.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:31:35 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/22 15:28:31 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include "ctype.h"

int	strtoi(char *s, char **end)
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
		if ((sign == 1 && out > INT_MAX) || (sign == -1 && - out < INT_MIN))
			return (0);
		s++;
		*end = s;
	}
	return (out * sign);
}
