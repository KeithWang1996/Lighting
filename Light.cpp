#include "Light.h"
#include "Window.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>
using namespace std;
Light::Light(int mode)
{
	if (mode == 1) {
		light_mode = 1;
		light_dir = glm::vec3(1.0f, 0.0f, 0.0f);
	}
	else if (mode == 2) {
		parse("sphere.obj");
		light_mode = 2;
		light_pos = glm::vec3(0.0f, 0.0f, 6.0f);
	}
	else if (mode == 3) {
		parse("cone.obj");
		light_mode = 3;
		light_pos = glm::vec3(0.0f, 0.0f, 8.0f);
		light_dir = glm::vec3(0.0f, 0.0f, -1.0f);
		cutoff_angle = 0.999;
		exponent = 0.6;
	}
	
	
	if (mode != 1) {
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
	}

	curr = glm::vec3(0, 0, 0);
	prev = glm::vec3(0, 0, 0);
	axis = glm::vec3(1, 0, 0);
	temp = glm::mat4(1.0f);
	toWorld = glm::mat4(1.0f);

	object_mode = 1;
	color_diff = glm::vec3(1.0f, 1.0f, 1.0f);
	color_spec = glm::vec3(1.0f, 1.0f, 1.0f);
	color_ambi = glm::vec3(0.7f, 0.6f, 0.5f);
	light_color = glm::vec3(1.0f, 1.0f, 1.0f);
}

void Light::parse(const char* filepath) {
	FILE *fp;
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLuint t1, t2, t3;//indices of triangle corner in vertices
	GLuint n1, n2, n3;//indices of triangle corner in vertex normals
	int c1, c2;
	fp = fopen(filepath, "rb");
	if (fp == NULL) {
		cerr << "error loading file" << endl;
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
			tvertices.push_back(t1 - 1);
			tvertices.push_back(t2 - 1);
			tvertices.push_back(t3 - 1);
			//tnormals.push_back(glm::vec3(n1, n2, n3));
			//cout << t1<<endl << t2 << endl << t3 << endl;
		}
		else {
			fscanf(fp, "\n");
		}
	}
	for (int i = 0; i < 3; i++) {
		tvertices.pop_back();
	}
	fclose(fp);
}

void Light::draw(GLuint shaderProgram) {
	
	if (light_mode != 1) {
		glm::mat4 modelview = Window::V * toWorld;

		uProjection = glGetUniformLocation(shaderProgram, "projection");
		uModelview = glGetUniformLocation(shaderProgram, "modelview");
		uModel = glGetUniformLocation(shaderProgram, "model");

		glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
		glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
		glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, (GLsizei)tvertices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
		glUniform1i(glGetUniformLocation(shaderProgram, "light.light_mode"), light_mode);
		glUniform3f(glGetUniformLocation(shaderProgram, "light.light_color"), light_color.x, light_color.y, light_color.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "light.light_pos"), light_pos.x, light_pos.y, light_pos.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "light.light_dir"), light_dir.x, light_dir.y, light_dir.z);
		glUniform1f(glGetUniformLocation(shaderProgram, "light.cutoff_angle"), cutoff_angle);
		glUniform1f(glGetUniformLocation(shaderProgram, "light.exponent"), exponent);

		glUniform1i(glGetUniformLocation(shaderProgram, "material.object_mode"), object_mode);
		glUniform3f(glGetUniformLocation(shaderProgram, "material.color_diff"), color_diff.x, color_diff.y, color_diff.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "material.color_spec"), color_spec.x, color_spec.y, color_spec.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "material.color_ambi"), color_ambi.x, color_ambi.y, color_ambi.z);
}

void Light::update() {
	toWorld = rotate()*temp*init();

}

glm::mat4 Light::init() {
	if (light_mode == 2) {
		return glm::translate(glm::mat4(1.0f), this->light_pos)*glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	}
	else if (light_mode == 3) {
		return glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2))*glm::translate(glm::mat4(1.0f), this->light_pos);
			//*glm::rotate(glm::mat4(1.0f), 90.0f/180.0f * glm::pi<float>(), glm::vec3(1, 0, 0));
	}
}

glm::mat4 Light::rotate() {
	return glm::rotate(glm::mat4(1.0f), vel / 180.0f * glm::pi<float>(), axis);
}

Light::~Light()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO2);
}
