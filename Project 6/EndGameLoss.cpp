#include "EndGameLoss.h"

void EndGameLoss::Initialize(int carSelected) {
	state.nextScene = -1;
	glClearColor(0.7f, 0.0f, 0.0f, 1.0f);
}

void EndGameLoss::Update(float deltaTime) {}

void EndGameLoss::Render(ShaderProgram* program) {
	Util::DrawText(program, Util::LoadTexture("font2.png"), "GAME OVER!", 1.0f, 0.0f, glm::vec3(-4.25, 1.5, 0));
	Util::DrawText(program, Util::LoadTexture("font2.png"), "YOU MACKED!", 1.0f, 0.0f, glm::vec3(-4.6, 0, 0));	// macked = crashed
	Util::DrawText(program, Util::LoadTexture("font2.png"), "PRESS SPACE TO TRY AGAIN FROM THIS LEVEL", 0.25f, 0.0f, glm::vec3(-4.75, -2.0, 0));
	Util::DrawText(program, Util::LoadTexture("font2.png"), "PRESS ENTER TO TRY AGAIN FROM THE START", 0.25f, 0.0f, glm::vec3(-4.6, -2.5, 0));
}