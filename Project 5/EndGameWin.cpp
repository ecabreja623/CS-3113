#include "EndGameWin.h"

void EndGameWin::Initialize() {
	state.nextScene = -1;
	glClearColor(0.0f, 0.2f, 0.8f, 1.0f);
}

void EndGameWin::Update(float deltaTime) {}

void EndGameWin::Render(ShaderProgram* program) {
	Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "GAME OVER!", 1.0f, 0.0f, glm::vec3(-4.25, 1, 0));
	Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "YOU WIN!", 1.0f, 0.0f, glm::vec3(-4, -0.5, 0));
	Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "PRESS ENTER TO PLAY AGAIN", 0.25f, 0.0f, glm::vec3(-2.35, -3.0, 0));
}