#include "Scene.h"

class EndGameWin : public Scene {
public:
	void Initialize() override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};