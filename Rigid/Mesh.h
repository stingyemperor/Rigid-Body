#pragma once
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Plane.h"
#include <omp.h>
#include <tuple>
#include <vector>


# define M_PI 3.14159265358979323846

class Mesh
{

public:

	float m;
	glm::vec3 X, V, w, L, p;
	glm::mat3 I0, I0_inv, R, I;
	glm::quat q;

	std::vector <glm::vec3> vertices;
	std::vector<std::vector<int>> faces;
	std::vector<std::vector<int>> edge;
	float Cr = 0.9;

	Mesh();

	Mesh derivative(Mesh body);

	Mesh add(Mesh *p1, Mesh *p2);

	Mesh multiply(Mesh *p1, float h);

	std::vector<glm::vec3> vertices_o();

	glm::mat3 w_star();

	void update();

	void RK4step(float h);

	glm::vec3 collision_check(std::vector <glm::vec3> Vert, std::vector<std::vector<int>> indices, float h);

	std::tuple<bool,glm::vec3> face_collision(glm::vec3 oldPos, glm::vec3 newPos, std::vector<glm::vec3> Vert, std::vector<int> indices_);

	std::tuple<glm::vec3,glm::vec3> collision_response(glm::vec3 ra, glm::vec3 normal);

	~Mesh();
};

