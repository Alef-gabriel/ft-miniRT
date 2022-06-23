/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   1_ray.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anhigo-s <anhigo-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 23:22:42 by anhigo-s          #+#    #+#             */
/*   Updated: 2022/06/23 00:23:37 by anhigo-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_ray	*ray_direction(t_mini *data, int x, int y)
{
	t_ray	*ray = (t_ray *)malloc(sizeof(t_ray));
	double	*camera;

	camera = vector_to_double(data->cam->view_point);
	ray->direction = find_direction(data->wall, data->wall_size, camera, x, y);
	ray->origin = camera;
	return (ray);
}
