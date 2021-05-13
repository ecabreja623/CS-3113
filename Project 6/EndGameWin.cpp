#include "EndGameWin.h"

void EndGameWin::Initialize(int carSelected) {
	state.nextScene = -1;
	glClearColor(0.8f, 0.6f, 0.9f, 1.0f);
}

void EndGameWin::Update(float deltaTime) {}

void EndGameWin::Render(ShaderProgram* program) {
	Util::DrawText(program, Util::LoadTexture("font2.png"), "YOU WIN!", 1.0f, 0.0f, glm::vec3(-3.25, 1, 0));
	Util::DrawText(program, Util::LoadTexture("font2.png"), "YOU WERE CUTTING LIKE A KNIFE!", 0.35f, 0.0f, glm::vec3(-5, -0.75, 0));

	Util::DrawText(program, Util::LoadTexture("font2.png"), "PRESS ENTER TO PLAY AGAIN", 0.25f, 0.0f, glm::vec3(-2.85, -2, 0));
}