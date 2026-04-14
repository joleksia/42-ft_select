/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_open.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 11:10:32 by joleksia          #+#    #+#             */
/*   Updated: 2026/02/07 11:23:30 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

extern int ft_open(const char *pathname, int flags, mode_t mode) {
    int fd;

    __asm__ volatile (
        "syscall"
        : "=a" (fd)
        : "a" (2),
          "D" (pathname),
          "S" (flags),
          "d" (mode)
        : "rcx", "r11", "memory"
    );
    return (fd);
}
