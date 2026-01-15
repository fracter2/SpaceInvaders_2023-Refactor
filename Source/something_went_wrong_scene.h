#pragma once				// TODO Consider making into #ifdef... type include guard (portability, recomended by cpp guidelines)
#include <functional>
#include <exception>
#include <string>

#include "scene.h"

class SomethingWentWrongScene : public Scene {
public:
	explicit SomethingWentWrongScene(const std::function<void(SceneId)>& transitionFunc, std::exception_ptr exception) noexcept;
	void Update() noexcept override;
	void Render() const noexcept override;

private:
	std::function<void(SceneId)> transitionTo;
	std::string exceptionMessage;
};