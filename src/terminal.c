/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:13:06 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/25 14:35:43 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "terminal.h"
#include "vga.h"

#define WORKSPACE_NBR 10

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

uint8_t					workspace_idx = 0;
terminal_workspace_t	workspaces[WORKSPACE_NBR];


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

	for (int i = 0 ; i < WORKSPACE_NBR ; i++)
	{
		workspaces[i].row = 1;
		workspaces[i].column = 0;
		workspaces[i].color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t index = y * VGA_WIDTH + x;
				if (x == 40 && y == 0)
					workspaces[i].buffer[index] = vga_entry('0' + i, workspaces[i].color);
				else
					workspaces[i].buffer[index] = vga_entry(' ', workspaces[i].color);
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

void	terminal_change_workspace(uint8_t workspace)
{
	if (workspace >= WORKSPACE_NBR || workspace == workspace_idx)
		return ;

	// Save the 
	workspaces[workspace_idx].row = terminal_row;
	workspaces[workspace_idx].column = terminal_column;
	workspaces[workspace_idx].color = terminal_color;
	memcpy(
		workspaces[workspace_idx].buffer,
		terminal_buffer,
		VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t)
	);

	workspace_idx = workspace;

	terminal_row = workspaces[workspace_idx].row;
	terminal_column = workspaces[workspace_idx].column;
	terminal_color = workspaces[workspace_idx].color;
	memcpy(
		terminal_buffer,
		workspaces[workspace_idx].buffer,
		VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t)
	);
}
