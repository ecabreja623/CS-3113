#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Entity.h"

struct GameState {
	Entity* player;
	Entity* objects;
	Entity* floor;
	Entity* AI;
	int nextScene;	// allows us to pick which scene should be switched to
	int carSelected = -1;	// for vehicle selection screen
};

class Scene {
public:
	GameState state;
	virtual void Initialize(int carSelected) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Render(ShaderProgram* program) = 0;
};