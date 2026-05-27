/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdlib.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 15:23:26 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/27 10:14:54 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STDLIB_H
# define STDLIB_H

# include <stddef.h>
# include <stdint.h>

# define UINT8_HEX_MAX_SIZE 2
# define UINT16_HEX_MAX_SIZE 4

int		toupper(int c);
int		tolower(int c);
int		atoi(const char *nptr);
long	strtol(char *s, char **end);
int		strtoi(char *s, char **end);
int		toabs(int n);
size_t	utoa_base(char out[UINT8_HEX_MAX_SIZE + 1], uint8_t n, char *base);
size_t	u16toa_base(char out[UINT16_HEX_MAX_SIZE + 1], uint16_t n, char *base);

#endif
