/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_init_file.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anhigo-s <anhigo-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 22:33:43 by anhigo-s          #+#    #+#             */
/*   Updated: 2022/06/23 23:19:31 by anhigo-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static void get_element_scene(t_mini *data, char **split);

void	get_line_scene(t_mini *data, char *line)
{
	char	**split;

	split = ft_split(line, '+');
	free(line);
	if (matrix_len(split) == 1)
	{
		matrix_free(split);
		return ;
	}
	if (matrix_len(split) < 3)
	{
		printf("%s\n", split[0]);
		print_error("miniRT: Invalid Line");
		exit(0); // free memory
	}
	get_element_scene(data, split);
	matrix_free(split);
}

static void get_element_scene(t_mini *data, char **split)
{
	if (find_ambient(data, split))
		data->light_a = init_ambient(split);
	else if (find_camera(data, split))
		data->cam = init_camera(split);
	else if (find_light(data, split))
		data->light = init_light(split);
	else if (find_sphere(split))
		lst_new_sphere(data, split);
	else if (find_plane(split))
		lst_new_plane(data, split);
	else if (find_cylinder(split))
		lst_new_cylinder(data, split);
	return ;
}