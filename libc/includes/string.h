/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 15:20:31 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/22 16:14:50 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_H
# define STRING_H

# include <stddef.h>

void	bzero(void *s, size_t n);
void	*memmove(void *dest, const void *src, size_t n);
size_t	strlen(const char *s);
size_t	strlcpy(char *dst, const char *src, size_t dsize);
size_t	strlcat(char *dst, const char *src, size_t dsize);
char	*strchr(const char *s, int c);
char	*strrchr(const char *s, int c);
int		strncmp(const char *s1, const char *s2, size_t n);
char	*strnstr(const char *big, const char *little, size_t len);
void	striteri(char *s, void (*f)(unsigned int, char*));

#endif
