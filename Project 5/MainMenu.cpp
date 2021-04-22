#include "MainMenu.h"

void MainMenu::Initialize() {
	state.nextScene = -1;
	glClearColor(0.3f, 0.0f, 0.6f, 1.0f);
}

void MainMenu::Update(float deltaTime) {}

void MainMenu::Render(ShaderProgram* program) {
	Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "SLATT", 1.0f, 0.0f, glm::vec3(-4, 1, 0));
	Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "PRESS ENTER TO START", 0.25f, 0.0f, glm::vec3(-2.35, -1.0, 0));

	Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "Controls: Arrow keys to move, SPACE to jump, Left Mouse Click to shoot", 0.13f, 0.0f, glm::vec3(-4.5, -3.0, 0));
}