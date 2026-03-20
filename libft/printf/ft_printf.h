/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohurteb <yohurteb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 10:41:08 by yohurteb          #+#    #+#             */
/*   Updated: 2024/05/28 14:54:45 by yohurteb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H
# include <stdarg.h>
# include <unistd.h>

int	ft_printf(const char *format, ...)
	__attribute__((format(printf, 1, 2)));

int	nb_base_long(unsigned long nb, char *base, unsigned long size_b);

int	verif_nil(unsigned long nb, char *base, unsigned long size_b);

#endif