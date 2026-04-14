/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_at_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/02 05:16:30 by joleksia          #+#    #+#             */
/*   Updated: 2026/05/02 05:16:30 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void ft_lstadd_at(t_list **lst, t_list *new, size_t index) {
    /* quicker execution: if index is 0 then we're just adding to the front... */
    if (index == 0) {
        return (ft_lstadd_front(lst, new));
    }
    /* safer execution: if index exceeds the size of the list, add to the back... */
    size_t lstsize = ft_lstsize(*lst);
    if (index >= lstsize) {
        return (ft_lstadd_back(lst, new));
    }

    /* iterate to the index... */
    t_list *iter = *lst;
    for (size_t i = 0; i < index; i++) {
        iter = iter->next;
    }

    /* insert new element into the list... */
    new->next = iter->next;
    iter->next = new;
}
