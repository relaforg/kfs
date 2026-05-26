/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 10:55:18 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/26 14:09:46 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gdt.h"

#define GDT_ENTRIES 7

static uint64_t __attribute__ ((section (".gdt")))	gdt[GDT_ENTRIES];
static struct gdt_ptr gp;

extern void gdt_flush(uint32_t gdt_ptr_addr);

/* Version OSDev, mais qui RETOURNE le descripteur au lieu de l'afficher */
uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
    uint64_t descriptor;

    /* Construction des 32 bits hauts */
    descriptor  =  limit       & 0x000F0000;
    descriptor |= (flag <<  8) & 0x00F0FF00;
    descriptor |= (base >> 16) & 0x000000FF;
    descriptor |=  base        & 0xFF000000;

    descriptor <<= 32;

    /* Construction des 32 bits bas */
    descriptor |= base  << 16;
    descriptor |= limit & 0x0000FFFF;

    return descriptor;
}

void gdt_init(void)
{
    gdt[0] = 0;                                              /* Null */
    gdt[1] = create_descriptor(0, 0x000FFFFF, GDT_CODE_PL0); /* Kernel code  - 0x08 */
    gdt[2] = create_descriptor(0, 0x000FFFFF, GDT_DATA_PL0); /* Kernel data  - 0x10 */
    gdt[3] = create_descriptor(0, 0x000FFFFF, GDT_DATA_PL0); /* Kernel stack - 0x18 */
    gdt[4] = create_descriptor(0, 0x000FFFFF, GDT_CODE_PL3); /* User code    - 0x20 */
    gdt[5] = create_descriptor(0, 0x000FFFFF, GDT_DATA_PL3); /* User data    - 0x28 */
    gdt[6] = create_descriptor(0, 0x000FFFFF, GDT_DATA_PL3); /* User stack   - 0x30 */

    gp.limit = sizeof(gdt) - 1;
    gp.base  = (uint32_t)&gdt;

    gdt_flush((uint32_t)&gp);
}
