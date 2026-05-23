/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <relaforg@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 12:56:59 by relaforg          #+#    #+#             */
/*   Updated: 2025/11/03 13:14:13 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

char	*strchr(const char *s, int c)
{
	while (*s != (char) c)
	{
		if (!*s)
			return (NULL);
		s++;
	}
	return ((char *) s);
}
