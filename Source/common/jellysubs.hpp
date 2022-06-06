#ifndef JELLYSUBS_HPP
#define JELLYSUBS_HPP

struct jelly
{
	float dt, friction, k, density, radius, spherespeed, spheremass;
	vec3 sphere;
	vec4 gravity;
};

void savejelly(float, float, float, float, vec3, float, float, float, vec4, int);
jelly loadjelly(int);

#endif
