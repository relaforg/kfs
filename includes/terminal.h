/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:10:01 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/25 11:48:58 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TERMINAL_H
# define TERMINAL_H

# include <stdint.h>
# include <stddef.h>
# include "vga.h"

typedef struct
{
	size_t		row;
	size_t		column;
	uint8_t		color;
	uint16_t	buffer[VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t)];
}	terminal_screen_t;

void	terminal_initialize(void);
void	terminal_setcolor(uint8_t color);
void	terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void	terminal_up_scroll();
void	terminal_putchar(char c);
void	terminal_write(const char* data, size_t size);
void	terminal_writestring(const char* data);
void	terminal_change_screen(uint8_t screen);

#endif
