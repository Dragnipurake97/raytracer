#include "model.h"

// Prototypes
void drawScene(ppm &image);
bool castRay(ray r, vec3<float> &normal, vec3<float> &world, material &surface, float &ray_dist);
colour Trace(ray r, float ray_distance);
bool castShadowRay(ray r);
colour gammaCorrection(colour c);

// Misc Settings
const int WIDTH = 1000;
const int HEIGHT = 1000;
const float ERROR_MARGIN = 0.1;
const float RAY_DEPTH = 1000;

int fov = 90;
float gamma = 1.0;
vec3<float> origin = vec3<float>(0.0, 0.0, 0.0);

// Placeholder Colours
colour black = { 0, 0, 0 };
colour white = { 255, 255, 255 };
colour red = { 255, 0, 0 };
colour green = { 0, 255, 0 };
colour blue = { 0, 0, 255 };
colour cyan = { 111, 175, 173 };

// Scene Information
std::vector<model> scene;
std::vector<light> lights;
colour BACKGROUND = black;

int main()
{
	// Cornell Box
	scene.push_back(load3dModel("Models/green_right"));
	scene.push_back(load3dModel("Models/red_left"));
	scene.push_back(load3dModel("Models/white_top"));
	scene.push_back(load3dModel("Models/white_back"));
	scene.push_back(load3dModel("Models/white_bottom"));

	scene.at(0).mat = material(green, 0.8, 0.2);
	scene.at(1).mat = material(red, 0.8, 0.2);
	scene.at(2).mat = material(white, 0.8, 0.2);
	scene.at(3).mat = material(white, 0.8, 0.2);
	scene.at(4).mat = material(white, 0.8, 0.2);

	// Lights
	lights.push_back(light(vec3<float>(0, 98, 100), Point)); // Top centre of box
	lights.at(0).brightness = 0.5;

	ppm test(WIDTH, HEIGHT); 
	drawScene(test);
	writeFile(test, "Renders/test.ppm");

	return 0;
}

void drawScene(ppm &image)
{
	vec3<float>* intersect;

	// Loop through each pixel and cast a ray
	for (int x = 0; x < WIDTH; x++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			// BUILD AND CAST RAY
			ray r;
			r.origin = origin;

			// Convert to Normalised Device Coords
			float NDC_x = (x + 0.5) / WIDTH;
			float NDC_y = (y + 0.5) / HEIGHT;

			// Clamp to range -1, 1
			NDC_x = 2 * NDC_x - 1;
			NDC_y = 2 * NDC_y - 1;

			// Apply FoV
			NDC_x = NDC_x * tan(fov / 2);
			NDC_y = NDC_y * tan(fov / 2);

			// Normalise (Camera looks at +Z axis)
			r.direction = vec3<float>(NDC_x, NDC_y, 1.0).normalise();

			colour c = Trace(r, 0.0);

			c = gammaCorrection(c);

			image.setPixel(x, y, c);
		}
	}
}

colour gammaCorrection(colour c)
{
	// Gamma Correction
	c.r = pow(255.0 * (c.r / 255.0), (1.0 / gamma));
	c.g = pow(255.0 * (c.g / 255.0), (1.0 / gamma));
	c.b = pow(255.0 * (c.b / 255.0), (1.0 / gamma));

	// Make sure colours don't overflow
	if (c.r > 255)
		c.r = 255;
	if (c.g > 255)
		c.g = 255;
	if (c.b > 255)
		c.b = 255;

	return c;
}


colour Trace(ray r, float ray_distance)
{
	// Return if ray has gone on too long
	if (ray_distance > RAY_DEPTH)
		return black;

	colour final_colour(0.0, 0.0, 0.0);

	vec3<float> normal, world;
	material surface;

	// Check if there is an intersection
	if (!castRay(r, normal, world, surface, ray_distance))
		return black;
	
	// TOTAL DIRECT LIGHT FROM LIGHT SOURCES
	for (int l = 0; l < lights.size(); l++)
	{
		// Incoming light vector
		vec3<float> lightVector(world - lights.at(l).position);

		// SHADOW RAY
		r.origin = world;
		r.direction = vec3<float>(-lightVector.x, -lightVector.y, -lightVector.z); // Reverse light vector

		lightVector = lightVector.normalise();

		if (castShadowRay(r)) // If ray to light source is intersected, no light (directly) recieved
			continue;

		// Add Diffuse Lighting
		float diffuse = normal.dot(lightVector);

		// Handle negative dot (for more than 90 degrees)
		if (diffuse < 0)
			diffuse = 0;

		final_colour = final_colour + (surface.col * diffuse) * lights.at(l).brightness;

		// Blinn-Phong specular lighting
		vec3<float> view = world - origin;
		// Get halfway vector
		vec3<float> h = vec3<float>(view + lightVector).normalise();

		float specular = normal.dot(h);
		if (specular < 0.0)
			specular = 0.0;

		specular = pow(specular, surface.shininess);
		final_colour = final_colour + (surface.col * specular) * lights.at(l).brightness;
	}

	// REFLECTION
	ray reflection_ray;

	// Find reflection ray
	reflection_ray.origin = world;
	reflection_ray.direction = r.direction - normal * 2 * r.direction.dot(normal);

	// Cast it
	final_colour += Trace(reflection_ray, ray_distance);

	// REFRACTION
	if (surface.type == Transparent) // if transparent then there is refraction 
	{

	}

	// Make sure colours don't overflow
	if (final_colour.r > 255)
		final_colour.r = 255;
	if (final_colour.g > 255)
		final_colour.g = 255;
	if (final_colour.b > 255)
		final_colour.b = 255;

	return final_colour;


	return final_colour;
}


bool castRay(ray r, vec3<float> &normal, vec3<float> &world, material &surface, float &ray_dist)
{
	float t_buf = RAY_DEPTH;
	bool doesIntersect = false;
	vec3<float> v[3];
	model mesh;

	vec3<float> world_intersect;

	// For each model
	for (int m = 0; m < scene.size(); m++)
	{
		mesh = scene.at(m);

		// For each triangle
		for (int face = 0; face < mesh.triangles.size(); face += 3)
		{
			// Get triangle vertices
			v[0] = mesh.vertices.at(mesh.triangles.at(face));
			v[1] = mesh.vertices.at(mesh.triangles.at(face + 1));
			v[2] = mesh.vertices.at(mesh.triangles.at(face + 2));

			// Build Plane
			vec3<float> p_u = v[1] - v[0];
			vec3<float> p_v = v[2] - v[0];
			vec3<float> p_normal = p_u.cross(p_v);

			// Build plane Coordinate vectors
			vec3<float> pcs_u = p_u.normalise();
			vec3<float> pcs_normal = pcs_u.cross(p_v);
			pcs_normal = pcs_normal.normalise();
			vec3<float> pcs_w = pcs_normal.cross(pcs_u);
			pcs_w = pcs_w.normalise();

			vec3<float> s = r.origin - v[0];
			vec3<float> pcs_s = { s.dot(pcs_u), s.dot(pcs_w),  s.dot(pcs_normal) };
			vec3<float> pcs_l = { r.direction.dot(pcs_u), r.direction.dot(pcs_w), r.direction.dot(pcs_normal) };

			// Make sure intersection is not behind ray
			float t = (-pcs_s.z / pcs_l.z);
			if (t < 0)
				continue;

			// Where ray intersects constructed plane p
			vec3<float> p_intersect = r.origin + r.direction * (-pcs_s.z / pcs_l.z);

			// Make sure no self-intersection
			if (abs(p_intersect.x - r.origin.x) <= ERROR_MARGIN && abs(p_intersect.y - r.origin.y) <= ERROR_MARGIN && abs(p_intersect.z - r.origin.z) <= ERROR_MARGIN)
				continue;

			// Check intersection point is in the triangle
			float alpha, beta, gamma;
			vec3<float> intersect_A = p_intersect - v[0];
			vec3<float> AB = v[1] - v[0];
			vec3<float> AC = v[2] - v[0];
			beta = intersect_A.dot(pcs_normal.cross(AC)) / AB.dot(pcs_normal.cross(AC));
			gamma = intersect_A.dot(pcs_normal.cross(AB)) / AC.dot(pcs_normal.cross(AB));
			alpha = 1 - beta - gamma;

			if (alpha >= 0 && beta >= 0 && gamma >= 0)
			{
				doesIntersect = true;
				if (t < t_buf)
				{
					//world_intersect = (v[0] * alpha) + (v[1] * beta) + (v[2] * gamma);
					world = (v[0] * alpha) + (v[1] * beta) + (v[2] * gamma);
					normal = (v[1] - v[0]).cross(v[2] - v[0]).normalise();
					surface = mesh.mat;
				}
			}
		}
	}

	if (doesIntersect)
		ray_dist += t_buf;

	return doesIntersect;
}


bool castShadowRay(ray r)
{
	bool doesIntersect = false;
	vec3<float> v[3];
	model model;

	// For each model
	for (int m = 0; m < scene.size(); m++)
	{
		model = scene.at(m);

		// For each triangle
		for (int face = 0; face < model.triangles.size(); face += 3)
		{
			// Get triangle vertices
			v[0] = model.vertices.at(model.triangles.at(face));
			v[1] = model.vertices.at(model.triangles.at(face + 1));
			v[2] = model.vertices.at(model.triangles.at(face + 2));

			// Build Plane
			vec3<float> p_u = v[1] - v[0];
			vec3<float> p_v = v[2] - v[0];
			vec3<float> p_normal = p_u.cross(p_v);

			// Build plane Coordinate vectors
			vec3<float> pcs_u = p_u.normalise();
			vec3<float> pcs_normal = pcs_u.cross(p_v);
			pcs_normal = pcs_normal.normalise();
			vec3<float> pcs_w = pcs_normal.cross(pcs_u);
			pcs_w = pcs_w.normalise();

			vec3<float> s = r.origin - v[0];
			vec3<float> pcs_s = { s.dot(pcs_u), s.dot(pcs_w),  s.dot(pcs_normal) };
			vec3<float> pcs_l = { r.direction.dot(pcs_u), r.direction.dot(pcs_w), r.direction.dot(pcs_normal) };

			// Make sure intersection is not behind ray or light is in front of intersection
			float t = (-pcs_s.z / pcs_l.z);
			if (t < 0 || t > 1) // If t is less than zero its behind ray, greater than one and it's after the light
				continue;

			// Where ray intersects constructed plane p
			vec3<float> p_intersect = r.origin + r.direction * (-pcs_s.z / pcs_l.z);

			// Make sure no self-intersection
			if (abs(p_intersect.x - r.origin.x) <= ERROR_MARGIN && abs(p_intersect.y - r.origin.y) <= ERROR_MARGIN && abs(p_intersect.z - r.origin.z) <= ERROR_MARGIN)
				continue;

			// Check intersection point is in the triangle
			float alpha, beta, gamma;
			vec3<float> intersect_A = p_intersect - v[0];
			vec3<float> AB = v[1] - v[0];
			vec3<float> AC = v[2] - v[0];
			beta = intersect_A.dot(pcs_normal.cross(AC)) / AB.dot(pcs_normal.cross(AC));
			gamma = intersect_A.dot(pcs_normal.cross(AB)) / AC.dot(pcs_normal.cross(AB));
			alpha = 1 - beta - gamma;

			if (alpha >= 0 && beta >= 0 && gamma >= 0)
			{
				doesIntersect = true;
				break;
			}
		}
	}

	return doesIntersect;
}