/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 12:51:43 by relaforg          #+#    #+#             */
/*   Updated: 2026/05/23 13:07:45 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEYBOARD_H
# define KEYBOARD_H

# define KEYBOARD_DATA		0x60
# define KEYBOARD_STATUS	0x64

void	listen_keyboard(void);

#endif
