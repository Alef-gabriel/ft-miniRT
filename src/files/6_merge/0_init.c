#include "minirt.h"

double	*reflect(double *v, double *n)
{
	double	*var;
	double	*res;

	var = vector_multipli_scalar(2.0*vector_abs(v,n),n);
	res = vector_subtraction(v, var);
	free(var);
	return(res);
}

double	**matrix_identify(double x, double y, double z, double w)
{
	double	**res;
	int		i;

	i = 0;
	res = (double **)malloc(sizeof(double *) * 4);
	while (i < 4)
	{
		res[i] = (double *)malloc(sizeof(double) * 4);
		i++;
	}
	res[0][0] = (double)x;res[0][1] = 0.0;res[0][2] = 0.0;res[0][3] = 0.0;
	res[1][0] = 0.0;res[1][1] = (double)y;res[1][2] = 0.0;res[1][3] = 0.0;
	res[2][0] = 0.0;res[2][1] = 0.0;res[2][2] = (double)z;res[2][3] = 0.0;
	res[3][0] = 0.0;res[3][1] = 0.0;res[3][2] = 0.0;res[3][3] = (double)w;
	return (res);
}

t_ray_d	*ray_to_object_space(t_ray_d *ray, double **local)
{
	t_ray_d	*res;
	double	**inverse;
	double	**aux;

	res = (t_ray_d *)malloc(sizeof(t_ray_d));
	aux = (double **)malloc(sizeof(double *));

	inverse = matrix_inverter(local, matrix_determinant(local));
	aux[0] = ray->direction;
	aux = matrix_multiply(aux, inverse, 1);
	res->direction = aux[0];
	aux[0] = ray->origin;
	aux = matrix_multiply(aux, inverse, 1);
	res->origin = aux[0];
	return (res);
}

double	*make_point(double x, double y, double z)
{
	double	*res;

	res = (double *)malloc(sizeof(double));
	res[0] = x;
	res[1] = y;
	res[2] = z;
	return (res);
}

t_intersec	*plane_intersection(t_ray_d *ray)
{
	t_intersec	*intersectionPoints = NULL;
	if (ray->direction[1] < 0.001)
		return intersectionPoints;
	intersectionPoints = (t_intersec *)malloc(sizeof(t_intersec));
	intersectionPoints->t1 = -ray->origin[1] / ray->direction[1];
	intersectionPoints->cont = 2;
	return intersectionPoints;
}


t_intersec	*sphere_intersection(t_ray_d *rayy, t_obj_d *obj)
{
	t_intersec	*intersectionPoints = NULL;
	t_ray_d *ray = ray_to_object_space(rayy, obj->matrix);
	//position
	double *oc = vector_subtraction(ray->origin, obj->position);
	double	a = vector_abs(ray->direction, ray->direction);
	double	b = 2.0 * vector_abs(oc, ray->direction);
	double	c = vector_abs(oc, oc) - (obj->radius * obj->radius);
	double	discriminant = (b*b)-(4*a*c);
	free(oc);
	if (discriminant < 0)
	{
		return intersectionPoints;
	}
	intersectionPoints = (t_intersec *)malloc(sizeof(t_intersec));
	double t1 = (-b - sqrt(discriminant)) / (2.0 * a);
	double t2 = (-b + sqrt(discriminant)) / (2.0 * a);
	intersectionPoints->t1 = t1;
	intersectionPoints->t2 = t2;
	intersectionPoints->cont = 3;
	return intersectionPoints;
}


t_intersec	*cylinder_intersection(t_ray_d *ray, t_obj_d *obj)
{
	t_intersec	*intersectionPoints = NULL;

	double *oc = vector_subtraction(ray->origin, obj->position);
	double	a = ray->direction[0] * ray->direction[0] + ray->direction[2] * ray->direction[2];
	double	b = 2.0 * oc[0] * ray->direction[0] + 2.0 * oc[2] * ray->direction[2];

	double	c = oc[0] * oc[0] + oc[2] * oc[2] - obj->radius;
	double	discriminant = (b*b)-(4*a*c);
	if (discriminant < 0)
		return intersectionPoints;
	intersectionPoints = (t_intersec *)malloc(sizeof(t_intersec));
	intersectionPoints->cont = 0;
	double t0 = (-b - sqrt(discriminant)) / (2.0 * a);
	double t1 = (-b + sqrt(discriminant)) / (2.0 * a);
	if (t0 > t1)
	{
		double temp = t0;
		t0 = t1;
		t1 = temp;
	}
	double	y0 = oc[1] + t0 * ray->direction[1];

	double max =  (obj->radius * obj->radius) / 2.0;
	double min = -1.0 * max;
	if (min < y0 && y0 < max)
	{
		intersectionPoints->cont = 2;
		intersectionPoints->t1 = t0;
	}
	double	y1 = oc[1] + t1 * ray->direction[1];
	if (min < y1 && y1 < max)
	{
		intersectionPoints->cont++;
		intersectionPoints->t2 = t1;
	}
	return intersectionPoints;
}

t_hit_d	*hiter_point(t_intersec *intersections)
{
	t_hit_d	*first = NULL;
	if(intersections == NULL)
		return (NULL);
	else
	{
		if ((intersections->cont == 2 || intersections->cont == 3) && !(intersections->t1 < 0.0))
		{
			first = (t_hit_d *)malloc(sizeof(t_hit_d));
			first->t = intersections->t1;
			return (first);
		}
		if ((intersections->cont == 1 || intersections->cont == 3) && !(intersections->t2 < 0.0))
		{
			first = (t_hit_d *)malloc(sizeof(t_hit_d));
			first->t = intersections->t2;
			return (first);
		}
	}
	return (first);
}

double	*position(t_ray_d *ray, double t)
{
	double	*aux = vector_multipli_scalar(t, ray->direction);
	double	*position = vector_addition(ray->origin, aux);
	free(aux);
	return (position);
}

double	*slighting(double *position, t_light_d *light, double *eye, t_material_d *material, double *normal)
{
	double	*temp = material->color;
	double	*effectivecolor = vector_multipli(temp, light->intensity);
	double	*lightvec = vector_normalize_double(vector_subtraction(light->posi, position));
	double	*ambienteColor = vector_multipli_scalar(material->ambient, temp);
	double	*difusecolor;
	double	*specularcolor;

	double	lDOtn = vector_abs(lightvec, normal);
	if (lDOtn <= 0.0)
	{
		difusecolor = make_point(0.0, 0.0, 0.0);
		specularcolor = make_point(0.0, 0.0, 0.0);
	}
	else
	{
		difusecolor = vector_multipli_scalar(lDOtn, vector_multipli_scalar(material->diffuse, effectivecolor));
		double	*refle = reflect(vector_multipli_scalar(-1.0, lightvec), normal);
		double	rDote = vector_abs(refle, eye);
		if (rDote <= 0.0)
			specularcolor = make_point(0.0, 0.0, 0.0);
		else
		{
			//Shinniness ou Brilho e depois do rDote
			double	factor = pow(rDote, 200.0);
			specularcolor = vector_multipli_scalar(material->specular * factor, light->intensity);
		}
	}
	return vector_addition(vector_addition(ambienteColor, difusecolor), specularcolor);
}

t_hit_d	*hiter_scene_object(t_ray_d *ray, t_scene *scene)
{
	t_intersec	*intersection;
	t_hit_d		*hiter;
	int			i;

	i = 0;
	hiter = NULL;
	while (i < scene->cont)
	{
		if (scene->object[i]->type == 'C')
		{
			intersection = cylinder_intersection(ray, scene->object[i]);
			hiter = hiter_point(intersection);
			if (hiter)
				return(hiter);
		}
		if(scene->object[i]->type == 'S')
		{
			intersection = sphere_intersection(ray, scene->object[i]);
			hiter = hiter_point(intersection);
			if (hiter)
				return(hiter);
		}
		if(scene->object[i]->type == 'P')
		{
			intersection = plane_intersection(ray);
			hiter = hiter_point(intersection);
			if (hiter)
				return(hiter);
		}
		i++;
	}
	return (hiter);
}

void	render(t_scene *scene, t_image *img, int resolution)
{
	double	*camera = make_point(0, 0, -5);
	double	*wall = make_point(0,0,7.0);
	double	wallsize = 7.0;
	t_ray_d	*ray = (t_ray_d *)malloc(sizeof(t_ray_d));
	int	x = 0;
	while (x < resolution)
	{
		int	y = 0;
		while (y < resolution)
		{
			double	increment = wallsize / resolution;
			double	*current_wall_pixel = vector_subtraction(wall, create_vector((wallsize * 0.5) - (x * increment), (wallsize * 0.5) - (y * increment), wall[2]));
			double	*point = vector_subtraction(current_wall_pixel, camera);
			double	*direction = vector_normalize_double(point);
			ray->direction = direction;
			ray->origin = camera;
			t_hit_d	*hit = hiter_scene_object(ray, scene);
			if(hit != NULL)
			{
				double	*hitposition = position(ray, hit->t);
				double	*lighting = slighting(hitposition, scene->light[0], ray->direction, scene->object[0]->material, vector_normalize_double(hitposition));
				int color = ((int)(255.99 * lighting[0])<<16) + ((int)(255.99 * lighting[1])<<8) + ((int)(255.99 * lighting[2]));
				my_mlx_pixel_put(img, x, y, color);
			}
			y++;
		}
		x++;
	}
}

t_scene	*init_scene(void)
{
	t_scene	*scene = (t_scene *)malloc(sizeof(t_scene));
	scene->cont = 1;
	scene->object = (t_obj_d **)malloc(sizeof(t_obj_d *) * 2);
	scene->object[0] = (t_obj_d*)malloc(sizeof(t_obj_d));
	scene->object[0]->radius = 2.0;
	scene->object[0]->type = 'C';
	scene->object[0]->position = make_point(0.0, 0.0, 0.0);
	scene->object[0]->matrix = matrix_identify(1,1,1,1);
	scene->object[0]->material = (t_material_d *)malloc(sizeof(t_material_d));
	scene->object[0]->material->color = create_vector(1, 0.2, 1.0);
	scene->object[0]->material->ambient = 0.1;
	scene->object[0]->material->diffuse = 0.9;
	scene->object[0]->material->specular = 0.9;
	scene->object[1] = (t_obj_d*)malloc(sizeof(t_obj_d));
	scene->object[1]->type = 'P';
	scene->light = (t_light_d **)malloc(sizeof(t_light_d *));
	scene->light[0] = (t_light_d *)malloc(sizeof(t_light_d));
	scene->light[0]->posi = make_point(-10, 10, -10);
	scene->light[0]->intensity = create_vector(1.0, 1.0, 1.0);
	return (scene);
}
