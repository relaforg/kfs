/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 12:55:33 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/23 16:30:26 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "keyboard.h"
#include "io.h"

static const char g_scancode_map[128] = {
	0,		0,		'1',	'2',	'3',	'4',	'5',	'6',	// 0x00 - 0x07
	'7',	'8',	'9',	'0',	'-',	'=',	0,		0,		// 0x08 - 0x0F
	'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',	// 0x10 - 0x17
	'o',	'p',	'[',	']',	'\n',	0,		'a',	's',	// 0x18 - 0x1F
	'd',	'f',	'g',	'h',	'j',	'k',	'l',	';',	// 0x20 - 0x27
	'\'',	'`',	0,		'\\',	'z',	'x',	'c',	'v',	// 0x28 - 0x2F
	'b',	'n',	'm',	',',	'.',	'/',	0,		'*',	// 0x30 - 0x37
	0,		' ',	0,		0,		0,		0,		0,		0,		// 0x38 - 0x3F
	0,		0,		0,		0,		0,		0,		0,		'7',	// 0x40 - 0x47
	'8',	'9',	'-',	'4',	'5',	'6',	'+',	'1',	// 0x48 - 0x4F
	'2',	'3',	'0',	'.',	0,		0,		0,		0,		// 0x50 - 0x57
	0,		0,		0,		0,		0,		0,		0,		0,		// 0x58 - 0x5F
	0,		0,		0,		0,		0,		0,		0,		0,		// 0x60 - 0x67
	0,		0,		0,		0,		0,		0,		0,		0,		// 0x68 - 0x6F
	0,		0,		0,		0,		0,		0,		0,		0,		// 0x70 - 0x77
	0,		0,		0,		0,		0,		0,		0,		0		// 0x78 - 0x7F
};

const uint8_t	left_shift_pressed = 0x2A;
const uint8_t	left_shift_realeased = 0xAA;
const uint8_t	shift_pos = 0;

uint8_t	modifiers = 0;

void	listen_keyboard(void)
{
	uint8_t	status;
	char	tmp;

	while (1)
	{
		status = inb(KEYBOARD_STATUS);
		if ((status & 0x1) == 1)
		{
			status = inb(KEYBOARD_DATA);
			if (status <= 127)
			{
				tmp = g_scancode_map[status];
				if (tmp != 0)
				{
					printf("%c", tmp);
					continue ;
				}
			}
			switch (status)
			{
				case left_shift_pressed:
					modifiers |= 1 << shift_pos;
					break ;
				case left_shift_realeased:
					modifiers &= 0 << shift_pos;
					printf("\n%d\n", modifiers);
					break ;
			}
		}
	}
}
