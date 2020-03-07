#include "Input.hpp"
#include <FAN/Settings.hpp>
#include <string>

using namespace Settings;

namespace CursorNamespace {
	_vec2<int> cursor_position;
}

namespace WindowNamespace {
	_vec2<int> window_size;
}

namespace Input {
	bool key[1024];
	std::string characters;
	bool action[348];
	bool released[1024];
	bool cursor_inside_window;
}

void GlfwErrorCallback(int id, const char* error) {
	printf("GLFW Error %d : %s\n", id, error);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (!action) {
		if (key < 0) {
			return;
		}
		Input::key[key % 1024] = false;
		return;
	}

	Input::key[key] = true;

	for (int i = 32; i < 162; i++) {
		if (key == i && action == 1) {
			Input::action[key] = true;
		}
	}

	for (int i = 256; i < 384; i++) {
		if (key == i && action == 1) {
			Input::action[key] = true;
		}
	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if ((button == GLFW_MOUSE_BUTTON_LEFT ||
		button == GLFW_MOUSE_BUTTON_RIGHT ||
		button == GLFW_MOUSE_BUTTON_MIDDLE)
		&& action == GLFW_PRESS) {
		Input::key[button] = true;
	}
	else {
		Input::key[button] = false;
	}
	for (int i = 0; i < GLFW_MOUSE_BUTTON_8; i++) {
		if (i == button && action == 1) {
			Input::action[button] = true;
		}
	}
}

void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
	CursorNamespace::cursor_position = vec2(xpos, ypos);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	if (yoffset == 1) {
		Input::action[GLFW_MOUSE_SCROLL_UP] = true;
	}
	else if (yoffset == -1) {
		Input::action[GLFW_MOUSE_SCROLL_DOWN] = true;
	}
}

//bool render_one = true;
void CharacterCallback(GLFWwindow* window, unsigned int key) {
	Input::characters.push_back(key);
	//render_one = true;
}

void FrameSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	WindowNamespace::window_size = vec2(width, height);
}

void CursorEnterCallback(GLFWwindow* window, int entered) {
	Input::cursor_inside_window = entered;
}

void WindowInit() {
	using namespace WindowNamespace;
	window_size = WINDOWSIZE;
	glfwWindowHint(GLFW_DECORATED, false);
	glfwWindowHint(GLFW_RESIZABLE, true);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//window_size = vec2(glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
	window = glfwCreateWindow(window_size.x, window_size.y, "Server", fullScreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (!window) {
		printf("Window ded\n");
		glfwTerminate();
		exit(EXIT_SUCCESS);
	}
	glfwMakeContextCurrent(window);
	glewInit();
	glViewport(0, 0, window_size.x, window_size.y);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

std::string& textInput() {
	return Input::characters;
}

void OnKeyPress(int key, std::function<void()> lambda, bool once) {
	if (once) {
		if (KeyPressA(key)) {
			lambda();
		}
	}
	else {
		if (KeyPress(key)) {
			lambda();
		}
	}
}

bool cursor_inside_window() {
	return Input::cursor_inside_window;
}

bool KeyPress(int key) {
	if (Input::key[key % 1024]) {
		return true;
	}
	return false;
}

bool KeyPressA(int key) {
	if (Input::action[key % 348]) {
		return true;
	}
	return false;
}

void KeysReset() {
	for (int i = 0; i < 348; i++) {
		Input::action[i] = false;
	}
}