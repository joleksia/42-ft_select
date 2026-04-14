/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_close.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 11:28:08 by joleksia          #+#    #+#             */
/*   Updated: 2026/02/07 11:30:55 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

extern int ft_close(int fd) {
    int result;

    __asm__ volatile (
        "syscall"
        : "=a" (result)
        : "a" (3),
          "D" (fd)
        : "rcx", "r11", "memory"
    );
    return (result);
}
