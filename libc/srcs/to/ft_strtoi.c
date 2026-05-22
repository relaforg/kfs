/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtoi.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:31:35 by relaforg          #+#    #+#             */
/*   Updated: 2025/11/26 11:42:42 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strtoi(char *s, char **end)
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
		if ((sign == 1 && out > INT_MAX) || (sign == -1 && - out < INT_MIN))
			return (0);
		s++;
		*end = s;
	}
	return (out * sign);
}
