#include "OBJObject.h"
#include "Window.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>
using namespace std;

OBJObject::OBJObject(const char *filepath) 
{
	parse(filepath);
	GLfloat maxX = -INFINITY;
	GLfloat maxY = -INFINITY;
	GLfloat maxZ = -INFINITY;
	GLfloat minX = INFINITY;
	GLfloat minY = INFINITY;
	GLfloat minZ = INFINITY;

	for (unsigned int i = 0; i < vertices.size(); i += 3) {
		if (vertices[i] > maxX) maxX = vertices[i];
		if (vertices[i] < minX) minX = vertices[i];
	}
	for (unsigned int i = 1; i < vertices.size(); i += 3) {
		if (vertices[i] > maxY) maxY = vertices[i];
		if (vertices[i] < minY) minY = vertices[i];
	}
	for (unsigned int i = 2; i < vertices.size(); i += 3) {
		if (vertices[i] > maxZ) maxZ = vertices[i];
		if (vertices[i] < minZ) minZ = vertices[i];
	}

	GLfloat max = maxX - minX;
	if (max < (maxY - minY)) max = maxY - minY;
	if (max < (maxZ - minZ)) max = maxZ - minZ;
    scales = (4.0f / max);
	avgX = (maxX + minX) / 2;
	avgY = (maxY + minY) / 2;
	avgZ = (maxZ + minZ) / 2;

	toWorld = glm::mat4(1.0f);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(vertices.size()), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(normals.size()), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*tvertices.size(), &tvertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	OBJObject::size = 1;
	OBJObject::size2 = 1;
	OBJObject::position= glm::vec3(0.0f, 0.0f, 0.0f);
	//OBJObject::scales = 1;
	OBJObject::rangle = 0;
	OBJObject::curr = glm::vec3(0, 0, 0);
	OBJObject::prev = glm::vec3(0, 0, 0);
	OBJObject::axis = glm::vec3(1, 0, 0);
	OBJObject::temp = glm::mat4(1.0f);
	
}

OBJObject::~OBJObject() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO2);
}

void OBJObject::parse(const char *filepath) 
{
	FILE *fp;
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLuint t1, t2, t3;//indices of triangle corner in vertices
	GLuint n1, n2, n3;//indices of triangle corner in vertex normals
	int c1, c2;
	fp = fopen(filepath, "rb");
	if (fp == NULL) {
		cerr<< "error loading file" <<endl; 
		exit(-1); 
	}
	
	while (!feof(fp)) {
		c1 = fgetc(fp);
		c2 = fgetc(fp);
		if ((c1 == 'v') && (c2 == ' ')) {
			fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
		else if ((c1 == 'v') && (c2 == 'n')) {
			fscanf(fp, "%f %f %f", &x, &y, &z);
			//normals.push_back(glm::vec3(x, y, z));
			normals.push_back(x);
			normals.push_back(y);
			normals.push_back(z);
		}
		else if ((c1 == 'f') && (c2 == ' ')) {
			fscanf(fp, "%u//%u %u//%u %u//%u", &t1, &n1, &t2, &n2, &t3, &n3);
			tvertices.push_back(t1-1);
			tvertices.push_back(t2-1);
			tvertices.push_back(t3-1);
			//tnormals.push_back(glm::vec3(n1, n2, n3));
			//cout << t1<<endl << t2 << endl << t3 << endl;
		}
		else{
			fscanf(fp, "/n");
		}
	}
	for (int i = 0; i < 3; i++) {
		tvertices.pop_back();
	}
	fclose(fp);
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
}

void OBJObject::draw(GLuint shaderProgram)
{
	glm::mat4 modelview = Window::V * toWorld;
	
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	uModel = glGetUniformLocation(shaderProgram, "model");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, (GLsizei)tvertices.size(), GL_UNSIGNED_INT, 0);

	glUniform1i(glGetUniformLocation(shaderProgram, "rendermode"), Window::light_mode);
	
	glBindVertexArray(0);

}

void OBJObject::update() {
	//glPointSize(size);
		toWorld = rotate()*move()*temp*init();

}
/*
glm::mat4 OBJObject::spin(GLfloat deg)
{
	//toWorld = toWorld * glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	
	this->angle += deg;
	if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	// This creates the matrix to rotate the obj
	return (glm::rotate(glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)));
	
}
*/

glm::mat4 OBJObject::move(){
	return glm::translate(glm::mat4(1.0f),this->position);
}

glm::mat4 OBJObject::scale() {
	return glm::scale(glm::mat4(1.0f), glm::vec3(scales, scales, scales));
}

glm::mat4 OBJObject::rotate() {
	return glm::rotate(glm::mat4(1.0f), vel / 180.0f * glm::pi<float>(), axis);
}

glm::mat4 OBJObject::init() {
	return glm::scale(glm::mat4(1.0f), glm::vec3(scales, scales, scales))*
		glm::translate(glm::mat4(1.0f), glm::vec3(-avgX, -avgY, -avgZ));
}