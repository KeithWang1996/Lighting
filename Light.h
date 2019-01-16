#ifndef LIGHT_H
#define LIGHT_H

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <math.h>

class Light
{
public:
	//Material stuff
	GLint object_mode;
	glm::vec3 color_diff;
	glm::vec3 color_spec;
	glm::vec3 color_ambi;

	//Light stuff
	int light_mode;
	glm::vec3 light_color;
	glm::vec3 light_pos;
	glm::vec3 light_dir;
	float cutoff_angle;
	float exponent;

	GLuint VBO, VBO2, VAO, EBO;
	GLuint uProjection, uModelview, uModel;
	glm::mat4 toWorld;
	glm::mat4 temp;

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLuint> tvertices;

	std::vector<glm::vec3> tnormals;
	GLfloat vel;
	glm::vec3 axis;
	glm::vec3 curr;
	glm::vec3 prev;

	Light(int mode);
	void update();
	glm::mat4 init();
	glm::mat4 rotate();
	void draw(GLuint shaderProgram);
	void parse(const char* filepath);

	~Light();
};
#endif
