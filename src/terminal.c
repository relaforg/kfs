/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:13:06 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/25 14:31:39 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "terminal.h"
#include "vga.h"

#define SCREEN_NBR 10

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

uint8_t				screen_idx = 0;
terminal_screen_t	screens[SCREEN_NBR];


void terminal_initialize(void)
{
	terminal_row = 1;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			if (x == 40 && y == 0)
				terminal_buffer[index] = vga_entry('0', terminal_color);
			else
				terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}

	for (int i = 0 ; i < SCREEN_NBR ; i++)
	{
		screens[i].row = 1;
		screens[i].column = 0;
		screens[i].color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t index = y * VGA_WIDTH + x;
				if (x == 40 && y == 0)
					screens[i].buffer[index] = vga_entry('0' + i, screens[i].color);
				else
					screens[i].buffer[index] = vga_entry(' ', screens[i].color);
			}
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void	terminal_up_scroll()
{
	memmove((char *) (VGA_MEMORY + VGA_WIDTH * sizeof(uint16_t)),
		 (char *) (VGA_MEMORY + (VGA_WIDTH * sizeof(uint16_t)) * 2),
		 (VGA_HEIGHT - 1) * VGA_WIDTH * sizeof(uint16_t)
		 );
	bzero(
		(void *)VGA_MEMORY + (VGA_HEIGHT - 1) * VGA_WIDTH * sizeof(uint16_t),
		VGA_WIDTH * sizeof(uint16_t)
	   );
}

void terminal_putchar(char c) 
{
	if (c == '\n')
	{
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
		{
			terminal_up_scroll();
			terminal_row = VGA_HEIGHT - 1;
		}
		return ;
	}
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row >= VGA_HEIGHT)
		{
			terminal_up_scroll();
			terminal_row = VGA_HEIGHT - 1;
		}
	}
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void	terminal_change_screen(uint8_t screen)
{
	if (screen >= SCREEN_NBR || screen == screen_idx)
		return ;

	screens[screen_idx].row = terminal_row;
	screens[screen_idx].column = terminal_column;
	screens[screen_idx].color = terminal_color;
	memcpy(
		screens[screen_idx].buffer,
		terminal_buffer,
		VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t)
	);

	screen_idx = screen;

	terminal_row = screens[screen_idx].row;
	terminal_column = screens[screen_idx].column;
	terminal_color = screens[screen_idx].color;
	memcpy(
		terminal_buffer,
		screens[screen_idx].buffer,
		VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t)
	);
}
