#include "MainMenu.h"

void MainMenu::Initialize(int carSelected) {
	state.nextScene = -1;
	glClearColor(0.3f, 0.0f, 0.0f, 1.0f);
}

void MainMenu::Update(float deltaTime) {}

void MainMenu::Render(ShaderProgram* program) {
	Util::DrawIcon(program, Util::LoadTexture("game_logo_TL.png"), glm::vec3(-1, 1, 0));	// put together parts of logo to make it bigger
	Util::DrawIcon(program, Util::LoadTexture("game_logo_TM.png"), glm::vec3(0, 1, 0));
	Util::DrawIcon(program, Util::LoadTexture("game_logo_TR.png"), glm::vec3(1, 1, 0));
	Util::DrawIcon(program, Util::LoadTexture("game_logo_BL.png"), glm::vec3(-1, 0, 0));
	Util::DrawIcon(program, Util::LoadTexture("game_logo_BM.png"), glm::vec3(0, 0, 0));
	Util::DrawIcon(program, Util::LoadTexture("game_logo_BR.png"), glm::vec3(1, 0, 0));

	Util::DrawText(program, Util::LoadTexture("font2.png"), "created by Elvis Cabreja", 0.15f, 0.0f, glm::vec3(-1.7, -0.75, 0));
	Util::DrawText(program, Util::LoadTexture("font2.png"), "PRESS ENTER TO START", 0.25f, 0.0f, glm::vec3(-2.3, -1.5, 0));

	Util::DrawText(program, Util::LoadTexture("font2.png"), "Controls: WASD to drive", 0.15f, 0.0f, glm::vec3(-1.63, -3.0, 0));
}