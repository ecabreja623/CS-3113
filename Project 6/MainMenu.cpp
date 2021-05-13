#include "MainMenu.h"

void MainMenu::Initialize(int carSelected) {
	state.nextScene = -1;
	glClearColor(0.3f, 0.0f, 0.0f, 1.0f);
}

void MainMenu::Update(float deltaTime) {}

void MainMenu::Render(ShaderProgram* program) {
	Util::DrawIcon(program, Util::LoadTexture("game_logo.png"), glm::vec3(0, 0, 0));

	Util::DrawText(program, Util::LoadTexture("font2.png"), "created by Elvis Cabreja", 0.15f, 0.0f, glm::vec3(-1.7, -0.75, 0));
	Util::DrawText(program, Util::LoadTexture("font2.png"), "PRESS ENTER TO START", 0.25f, 0.0f, glm::vec3(-2.3, -1.5, 0));

	Util::DrawText(program, Util::LoadTexture("font2.png"), "Controls: WASD to drive", 0.15f, 0.0f, glm::vec3(-1.63, -3.0, 0));
}