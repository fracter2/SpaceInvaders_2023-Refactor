#include "something_went_wrong_scene.h"

#include "common.h"
#include <format>
#include <string>


namespace {
	std::string GetWhat(std::exception_ptr eptr) noexcept {
		try {
			std::rethrow_exception(eptr);
		}
		catch (const std::exception& e) {
			return e.what();
		}
	}
}

SomethingWentWrongScene::SomethingWentWrongScene(const std::function<void(SceneId)>& transitionFunc, std::exception_ptr exception) noexcept
	: transitionTo(transitionFunc)
	, exceptionMessage(GetWhat(exception))
{
}

void SomethingWentWrongScene::Update() noexcept {
	if (IsKeyReleased(KEY_ENTER)) {
		transitionTo(SceneId::MainMenu);
	}
}

void SomethingWentWrongScene::Render() const noexcept {
	static constexpr int fontSize = 40;

	{	// Prompt
		// NOTE The cpp guidenline ES.10 states to declare only one var/const per line. 
		// However i have opted to allow cases like this. It is only two, inherently 
		// simple x / y that MUST be initialized (constexpr). It also aligns well with 
		// the following line. 
		static constexpr int x = 600, y = 200;
		DrawText("PRESS ENTER TO CONTINUE", x, y, fontSize, YELLOW);
	}

	{	// Tittle
		static constexpr int x = 50, y = 100;
		DrawText("OOPS! SOMETHING WENT WRONG!", x, y, fontSize, YELLOW);
	}

	{	// Error text
		static constexpr int x = 50, y = 300;
		DrawText(std::format("Exception: {}", exceptionMessage).c_str(), x, y, fontSize, YELLOW);
	}
}
