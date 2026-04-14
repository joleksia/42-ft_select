/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ioctl.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 11:35:44 by joleksia          #+#    #+#             */
/*   Updated: 2026/02/07 12:21:47 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

extern int ft_ioctl(int fd, unsigned long request, void *arg) {
    int result;

    __asm__ volatile (
        "syscall"
        : "=a" (result)
        : "a" (16),
          "D" (fd),
          "S" (request),
          "d" (arg)
        : "rcx", "r11", "memory"
    );
    return (result);
}
