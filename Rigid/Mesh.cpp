#include "Mesh.h"

Mesh::Mesh()
{

	const float ea = 50.0;
	float l = ea, b = ea, h = ea;
	
	vertices.push_back({ 0.0,0.0,0.0 });
	vertices.push_back({ l,0,0 });
	vertices.push_back({ 0,b,0 });
	vertices.push_back({ 0,0,h });
	vertices.push_back({ l,b,0 });
	vertices.push_back({ l,0,h });
	vertices.push_back({ 0,b,h });
	vertices.push_back({ l,b,h });

	faces.push_back({ 0,1,4,2 });
	faces.push_back({ 3,5,7,6 });
	faces.push_back({ 1,4,7,5 });
	faces.push_back({ 0,2,6,3 });
	faces.push_back({ 0,1,5,3 });
	faces.push_back({ 2,4,7,6 });

	edge.push_back({ 0,1 });
	edge.push_back({ 1,4 });
	edge.push_back({ 4,2 });
	edge.push_back({ 2,0 });
	edge.push_back({ 3,5 });
	edge.push_back({ 5,7 });
	edge.push_back({ 7,6 });
	edge.push_back({ 6,3 });
	edge.push_back({ 0,3 });
	edge.push_back({ 5,1 });
	edge.push_back({ 7,4 });
	edge.push_back({ 6,2 });

	X = { 0,0,0 };
	m = 1;
	p = { 0.0f ,20.0f, 0.0 };
	L = { 400.0,0,0.0 };
	V = { 0,0,0 };
	
	float I_arr[9] = {  m / 12 * (b*b + h*h), 0, 0,
						0, m / 12 * (l*l + h*h), 0,
						0, 0, m / 12 * (l*l + b*b) };

	I0 = glm::make_mat3(I_arr);
	I0_inv = glm::inverse(I0);

	q = glm::angleAxis((float)M_PI/3, glm::vec3(0,1,1));

	R = glm::toMat3(q);
};

Mesh Mesh::derivative(Mesh body)
{
	Mesh der;

	der.X = body.p/body.m;
	glm::quat w_q = glm::quat(0.0f, body.w);
	der.q = 0.5f * w_q *  body.q;
	der.p = { 0,0,0 };
	der.L = { 0,0,0 };

	return der;
}

Mesh Mesh::add(Mesh* p1, Mesh* p2)
{
	Mesh temp;
	temp.X = p1->X + p2->X;
	temp.p = p1->p + p2->p;
	temp.q = p1->q + p2->q;
	temp.L = p1->L + p2->L;
	temp.vertices = p1->vertices;
	temp.faces = p1->faces;
	return temp;
}

Mesh Mesh::multiply(Mesh* p1, float h)
{
	Mesh temp;
	temp.X = p1->X * h;
	temp.p = p1->p * h;
	temp.q = p1->q * h;
	temp.L = p1->L * h;
	temp.vertices = p1->vertices;
	temp.faces = p1->faces;
	return temp;
}

std::vector<glm::vec3> Mesh::vertices_o()
{
	std::vector<glm::vec3> vertices_o;
	for (int i = 0; i < vertices.size(); i++) {
		vertices_o.push_back(X + glm::toMat3(glm::normalize(q)) * vertices[i]);
	}
	return vertices_o;
}

glm::mat3 Mesh::w_star()
{
	float w_arr[9] = { 0, -w[2],w[1], w[2], 0, -w[0], -w[1], w[0], 0 };
	return(glm::make_mat3(w_arr));
}

void Mesh::update()
{
	q = glm::normalize(q);
	R = glm::toMat3(q);
	I = (R)* I0 * glm::transpose(R);
	w = glm::inverse(I) * L;
}

void Mesh::RK4step(float h)
{
	Mesh K1, K2, K3, K4;
	K1 = derivative((*this));
	K2 = derivative(add(this, &multiply(&K1, 0.5*h)));
	K3 = derivative(add(this, &multiply(&K2, 0.5*h)));
	K4 = derivative(add(this, &multiply(&K3, h)));
	*this = add(this, &multiply(&K1, h / 6));
	*this = add(this, &multiply(&K2, h / 3));
	*this = add(this, &multiply(&K3, h / 3));
	*this = add(this, &multiply(&K4, h / 6));
}

glm::vec3 Mesh::collision_check(std::vector<glm::vec3> Vert, std::vector<std::vector<int>> indices, float h)
{
	Mesh nextState = *this;

	bool collide;
	nextState.RK4step(h);

	std::vector<glm::vec3> V1 = vertices_o();
	std::vector<glm::vec3> V2 = nextState.vertices_o();
	glm::vec3 normal, delP, delL;

	glm::vec3 Vfinal;


	#pragma omp parallel for collapse(2)
	for (int i = 0; i < V1.size(); i++) {
		for (int j = 0; j < indices.size(); j++) {

			std::tie(collide, normal) = face_collision(V1[i], V2[i], Vert, indices[j]);

			if(collide){
				std::tie(delP,delL) = collision_response(V1[i]-X,normal);
				p += delP; L += delL;

				Vfinal = V1[i];
				
			}
		}
	}
	return Vfinal;
}

std::tuple<bool, glm::vec3> Mesh::face_collision(glm::vec3 oldPos, glm::vec3 newPos, std::vector<glm::vec3> Vert, std::vector<int> indices_)
{

	glm::vec3 p1 = Vert[indices_[0]]; glm::vec3 p2 = Vert[indices_[1]]; glm::vec3 p3 = Vert[indices_[2]];
	float a1 = p2.x - p1.x;
	float b1 = p2.y - p1.y;
	float c1 = p2.z - p1.z;
	float a2 = p3.x - p1.x;
	float b2 = p3.y - p1.y;
	float c2 = p3.z - p1.z;
	float a = b1 * c2 - b2 * c1;
	float b = a2 * c1 - a1 * c2;
	float c = a1 * b2 - b1 * a2;
	float d = (-a * p1.x - b * p1.y - c * p1.z);
	float norm = sqrt(a*a + b*b + c*c);
	a = a / norm; b = b / norm; c = c / norm; d = d / norm;
	float x = a*oldPos.x + b*oldPos.y + c*oldPos.z + d;
	float y = a*newPos.x + b*newPos.y + c*newPos.z + d;
	bool planeCollide = !((x < 0) == (y < 0));

	glm::vec3 normal(a, b, c);

	
	if (planeCollide) {

		Point polygon[10], point;
		for (int i = 0; i < indices_.size(); i++) {
			polygon[i] = Point(Vert[indices_[i]][0], Vert[indices_[i]][1], Vert[indices_[i]][2]);

		}

		point = Point(oldPos[0], oldPos[1], oldPos[2]);

		if (!(isInside_yz(polygon, indices_.size(), point) || isInside(polygon, indices_.size(), point) || isInside_xz(polygon, indices_.size(), point)))
		{
			planeCollide = false;
		}
	}

	return std::make_tuple(planeCollide, normal);
}

std::tuple<glm::vec3, glm::vec3> Mesh::collision_response(glm::vec3 ra, glm::vec3 normal)
{
	
	glm::vec3 p_dot_a = p / m + glm::cross(w, ra);
	float v_ = glm::dot(p_dot_a, normal);
	float j_imp = (-(1.0f + Cr)*v_) / (1.0f / m + glm::dot(normal, glm::cross(glm::inverse(I)*ra*normal, ra)));
	if (abs(j_imp) > 20) j_imp = 20 * j_imp / abs(j_imp);

	return std::make_tuple(j_imp * normal, j_imp * glm::cross(ra, normal));
}


Mesh::~Mesh()
{
}
