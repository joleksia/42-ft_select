/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 11:12:36 by joleksia          #+#    #+#             */
/*   Updated: 2026/02/07 11:29:55 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

extern ssize_t ft_read(int fd, void *buf, size_t count) {
    ssize_t result;

    __asm__ volatile (
        "syscall"
        : "=a" (result)
        : "a" (0),
          "D" (fd),
          "S" (buf),
          "d" (count)
        : "rcx", "r11", "memory"
    );
    return (result);
}
