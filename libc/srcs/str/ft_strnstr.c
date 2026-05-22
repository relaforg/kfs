/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <relaforg@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 15:28:43 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/22 15:27:55 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "string.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	little_len;
	size_t	i;

	if (!*little)
		return ((char *) big);
	little_len = 0;
	while (little[little_len])
		little_len++;
	if (!len)
		return (NULL);
	i = 0;
	while (i <= len - little_len && big[i] && len >= little_len)
	{
		if (!ft_strncmp(&big[i], little, little_len))
			return ((char *) &big[i]);
		i++;
	}
	return (NULL);
}
