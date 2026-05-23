/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 12:59:09 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/23 13:02:37 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IO_H
# define IO_H

# include <stdint.h>

static inline uint8_t	inb(uint16_t port)
{
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void	outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

#endif
