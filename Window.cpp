#include "window.h"
const char* window_title = "GLFW Starter Project";
Cube * cube;
Light * light;
Light * dirLight;
Light * pointLight;
Light * spotLight;
OBJObject * obj;
OBJObject * bunny;
OBJObject * bear;
OBJObject * dragon;
OBJObject * head;

GLint shaderProgram;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
int Window::hold = 0;
int Window::mode = 0;
int Window::light_mode;
int Window::width;
int Window::height;
GLdouble Window::prevx;
GLdouble Window::prevy;
GLdouble Window::currx;
GLdouble Window::curry;
glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	
	
	dirLight = new Light(1);
	pointLight = new Light(2);
	spotLight = new Light(3);
   
	
	bunny = new OBJObject("bunny.obj");
	dragon = new OBJObject("dragon.obj");
	bear = new OBJObject("bear.obj");
	obj = bunny;
	light = dirLight;
	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	light_mode = 0;
	//Material init
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(cube);
	delete(bunny);
	delete(dragon);
	delete(bear);
	delete(dirLight);
	delete(pointLight);
	delete(spotLight);
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);
	
	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
	//cube->update();

	obj->update();
	light->update();
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);
	
	// Render the cube
	
	obj->draw(shaderProgram);
	light->draw(shaderProgram);
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_F1) {
			obj = bunny;
			dirLight->object_mode = 1;
			pointLight->object_mode = 1;
			spotLight->object_mode = 1;
		}
		if (key == GLFW_KEY_F2) {
			obj = dragon;
			dirLight->object_mode = 2;
			pointLight->object_mode = 2;
			spotLight->object_mode = 2;
		}
		if (key == GLFW_KEY_F3) {
			obj = bear;
			dirLight->object_mode = 3;
			pointLight->object_mode = 3;
			spotLight->object_mode = 3;
		}

		if (key == GLFW_KEY_S) {
			if (mods & GLFW_MOD_SHIFT)
				obj->scales = obj->scales * 1.1f;
			else
				obj->scales = obj->scales / 1.1f;
		}
		if (key == GLFW_KEY_W) {
			if (mode == 3) {
				if (mods & GLFW_MOD_SHIFT)
					light->cutoff_angle = light->cutoff_angle - 0.0001;
				else
					light->cutoff_angle = light->cutoff_angle + 0.0001;
			}
		}
		if (key == GLFW_KEY_E) {
			if (mode == 3) {
				if (mods & GLFW_MOD_SHIFT)
					light->exponent = light->exponent * 2;
				else
					light->exponent = light->exponent / 2;
			}
		}
		if (key == GLFW_KEY_1) {
			light = dirLight;
			mode = 1;
		}
		if (key == GLFW_KEY_2) {
			light = pointLight;
			mode = 2;
		}
		if (key == GLFW_KEY_3) {
			light = spotLight;
			mode = 3;
		}
		if (key == GLFW_KEY_0) {
			mode = 0;
		}
		if (key == GLFW_KEY_N) {
			if (light_mode == 0) {
				light_mode = 1;
				light = dirLight;
			}
			else {
				light_mode = 0;
				if (mode == 2) light = pointLight;
				if (mode == 3) light = spotLight;

			}
		}
	
	}
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			if (mode == 0) {
				hold = 1;
				glfwGetCursorPos(window, &prevx, &prevy);
				obj->prev = trackBallMapping(prevx, prevy);
			}
			else if (mode == 1) {
				hold = 5;
				glfwGetCursorPos(window, &prevx, &prevy);
				light->prev = trackBallMapping(prevx, prevy);
			}
			else if (mode == 2) {
				hold = 6;
				glfwGetCursorPos(window, &prevx, &prevy);
				light->prev = trackBallMapping(prevx, prevy);
			}
			else if (mode == 3) {
				hold = 7;
				glfwGetCursorPos(window, &prevx, &prevy);
				light->prev = trackBallMapping(prevx, prevy);
			}
		}
		else if (action == GLFW_RELEASE) {
			//prevx = 0;
			//prevy = 0;
			obj->temp = obj->rotate()*obj->move()*obj->temp;
			obj->vel = 0;
			light->temp = light->rotate()*light->temp;
		    light->vel = 0;
			hold = 0;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			glfwGetCursorPos(window, &prevx, &prevy);
			hold = 3;
		}
		if (action == GLFW_RELEASE) {
			obj->temp = obj->rotate()*obj->move()*obj->temp;
			obj->position = glm::vec3(0);
			hold = 4;
		}
	}
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (mode == 0) {
		obj->position.z = obj->position.z + yoffset / 4;
		obj->temp = obj->rotate()*obj->move()*obj->temp;
		obj->position = glm::vec3(0);
	}
	else if (mode == 2) {
		if (yoffset == 1) {
		    light->light_pos = glm::vec3(light->light_pos.x * 1.1,
			    light->light_pos.y * 1.1,
			    light->light_pos.z * 1.1);
	    }
		else if (yoffset == -1) {
			light->light_pos = glm::vec3(light->light_pos.x / 1.1,
				light->light_pos.y / 1.1,
				light->light_pos.z / 1.1);
		}
	}
	else if (mode == 3) {
		if (yoffset == 1) {
			light->light_pos = glm::vec3(light->light_pos.x * 1.1,
				light->light_pos.y * 1.1,
				light->light_pos.z * 1.1);
		}
		else if (yoffset == -1) {
			light->light_pos = glm::vec3(light->light_pos.x / 1.1,
				light->light_pos.y / 1.1,
				light->light_pos.z / 1.1);
		}
		light->light_dir = glm::normalize(-light->light_pos);
	}
}

glm::vec3 Window::trackBallMapping(GLfloat x, GLfloat y) {
	glm::vec3 v;
	GLfloat d;
	v.x = (2.0f*x - width) / width;
	v.y = (height - 2.0f*y) / height;
	v.z = 0.0f;
	d = sqrt(pow(v.x, 2) + pow(v.y, 2));
	d = (d<1.0f) ? d : 1.0f;
	v.z = sqrt(1.001f - d*d);
	glm::normalize(v);
	return v;
}


void Window::cursor_position_callback(GLFWwindow* window, GLdouble xpos, GLdouble ypos){
	if (hold == 1) {
		glfwGetCursorPos(window, &currx, &curry);
		obj->curr = trackBallMapping(currx, curry);
		float temp = -100*glm::acos(glm::dot(obj->curr, obj->prev));
		if (temp > 0.0001f || temp < 0.0001f) obj->vel = temp;
		obj->axis = glm::normalize(glm::cross(obj->curr, obj->prev));
		prevx = currx;
		prevy = curry;
	}
	if (hold == 3) {
		glfwGetCursorPos(window, &currx, &curry);
		obj->position.x = obj->position.x + (currx - prevx)/80;
		obj->position.y = obj->position.y - (curry - prevy)/80;
		prevx = currx;
		prevy = curry;
	}
	if (hold == 5) {
		glfwGetCursorPos(window, &currx, &curry);
		light->curr = trackBallMapping(currx, curry);
		light->light_dir = glm::normalize(light->light_dir + (light->prev - light->curr));
		prevx = currx;
		prevy = curry;
	}
	if (hold == 6) {
		glfwGetCursorPos(window, &currx, &curry);
		light->curr = trackBallMapping(currx, curry);
		float temp = -100 * glm::acos(glm::dot(light->curr, light->prev));
		if (temp > 0.0001f) light->vel = -temp/10;
		else if (temp < 0.0001f) light->vel = temp/10;
		light->axis = glm::normalize(glm::cross(light->curr, light->prev));
		light->light_pos = glm::vec3(light->rotate()* glm::vec4(light->light_pos, 0));
		prevx = currx;
		prevy = curry;
	}
	if (hold == 7) {
		glfwGetCursorPos(window, &currx, &curry);
		light->curr = trackBallMapping(currx, curry);
		float temp = -100 * glm::acos(glm::dot(light->curr, light->prev));
		if (temp > 0.0001f || temp < 0.0001f) light->vel = temp/10;
		light->axis = glm::normalize(glm::cross(light->curr, light->prev));
		light->light_pos = glm::vec3(light->rotate()* glm::vec4(light->light_pos, 0));
		light->light_dir = glm::normalize(-light->light_pos);
		prevx = currx;
		prevy = curry;
	}
}
