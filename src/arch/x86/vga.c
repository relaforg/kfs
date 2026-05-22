/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vga.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:08:21 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/22 16:22:37 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vga.h"

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return (fg | bg << 4);
}

uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return ((uint16_t) uc | (uint16_t) color << 8);
}
