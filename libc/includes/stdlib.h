/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdlib.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 15:23:26 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/22 15:24:04 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STDLIB_H
# define STDLIB_H

int		ft_toupper(int c);
int		ft_tolower(int c);
int		ft_atoi(const char *nptr);
long	ft_strtol(char *s, char **end);
int		ft_strtoi(char *s, char **end);
int		ft_toabs(int n);

#endif
