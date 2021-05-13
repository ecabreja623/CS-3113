#include "Scene.h"

class EndGameWin : public Scene {
public:
	void Initialize(int carSelected) override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};