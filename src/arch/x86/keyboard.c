/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 12:55:33 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/25 11:22:31 by relaforg         ###   ########.fr       */
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

static const char g_shift_scancode_map[128] = {
	0,		0,		'!',	'@',	'#',	'$',	'%',	'^',	// 0x00 - 0x07
	'&',	'*',	'(',	')',	'_',	'+',	0,		0,		// 0x08 - 0x0F
	'Q',	'W',	'E',	'R',	'T',	'Y',	'U',	'I',	// 0x10 - 0x17
	'O',	'P',	'{',	'}',	'\n',	0,		'A',	'S',	// 0x18 - 0x1F
	'D',	'F',	'G',	'H',	'J',	'K',	'L',	':',	// 0x20 - 0x27
	'"',	'~',	0,		'|',	'Z',	'X',	'C',	'V',	// 0x28 - 0x2F
	'B',	'N',	'M',	'<',	'>',	'?',	0,		'*',	// 0x30 - 0x37
	0,		' ',	0,		0,		0,		0,		0,		0,		// 0x38 - 0x3F
	0,		0,		0,		0,		0,		0,		0,		0,		// 0x40 - 0x47
	0,		0,		'-',	0,		0,		0,		'+',	0,		// 0x48 - 0x4F
	0,		0,		0,		0,		0,		0,		0,		0,		// 0x50 - 0x57
	0,		0,		0,		0,		0,		0,		0,		0,		// 0x58 - 0x5F
	0,		0,		0,		0,		0,		0,		0,		0,		// 0x60 - 0x67
	0,		0,		0,		0,		0,		0,		0,		0,		// 0x68 - 0x6F
	0,		0,		0,		0,		0,		0,		0,		0,		// 0x70 - 0x77
	0,		0,		0,		0,		0,		0,		0,		0		// 0x78 - 0x7F
};

static const uint8_t	left_shift_pressed = 0x2A;
static const uint8_t	left_shift_realeased = 0xAA;
static const uint8_t	right_shift_pressed = 0x36;
static const uint8_t	right_shift_realeased = 0xB6;
static const uint8_t	shift_pos = 0;

static const uint8_t	left_alt_pressed = 0x38;
static const uint8_t	left_alt_realeased = 0xB8;
static const uint8_t	alt_pos = 1;

static uint8_t	modifiers = 0;

void	listen_keyboard(void)
{
	uint8_t		status;
	char		tmp;
	const char	*keyboard_map = g_scancode_map;

	while (1)
	{
		status = inb(KEYBOARD_STATUS);
		if (!(status & 0x1))
			continue ;

		// Handle key pressed
		status = inb(KEYBOARD_DATA);
		if (status <= 127 && keyboard_map[status] != 0)
		{
			tmp = keyboard_map[status];
			if (modifiers >> alt_pos & 1)
				printf("\ntest");
			else
				printf("%c", tmp);
		}

		// Update modifiers
		switch (status)
		{
			case left_shift_pressed:
			case right_shift_pressed:
				modifiers |= 1 << shift_pos;
				keyboard_map = g_shift_scancode_map;
				break ;
			case left_shift_realeased:
			case right_shift_realeased:
				modifiers &= 0 << shift_pos;
				keyboard_map = g_scancode_map;
				break ;
			case left_alt_pressed:
				modifiers |= 1 << alt_pos;
				break ;
			case left_alt_realeased:
				modifiers &= 0 << alt_pos;
				break ;
		}
	}
}
