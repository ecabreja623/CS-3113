#include "Scene.h"

class EndGameLoss : public Scene {
public:
	void Initialize() override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};