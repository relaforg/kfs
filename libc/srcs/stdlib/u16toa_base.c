/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   u16toa_base.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 10:08:39 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/27 10:13:30 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "stdlib.h"
#include "string.h"


size_t	u16toa_base(char out[UINT16_HEX_MAX_SIZE + 1], uint16_t n, char *base)
{
	size_t	size;
	size_t	base_len;
	char	tmp[UINT16_HEX_MAX_SIZE];

	if (!base)
		return (0);
	if (n == 0)
	{
		out[0] = '0';
		out[1] = '\0';
		return (1);
	}
	base_len = strlen(base);
	size = 0;
	while (n && size < UINT16_HEX_MAX_SIZE)
	{
		tmp[size++] = base[n % base_len];
		n /= base_len;
	}
	for (size_t i = 0; i < size; i++)
		out[i] = tmp[size - 1 - i];
	out[size] = '\0';
	return (size);
}
