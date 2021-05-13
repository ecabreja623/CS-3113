#include "Scene.h"

class EndGameLoss : public Scene {
public:
	void Initialize(int carSelected) override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};