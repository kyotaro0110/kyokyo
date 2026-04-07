#pragma once
#include "stdafx.h"
#include "ButtonsDrawer.hpp"

enum class State {
	Menu,
	InputViewer
};

using App = SceneManager<State>;

class InputViewer : public App::Scene {
private:
	ButtonsDrawer m_drawer{ };

public:
	InputViewer(const InitData& init) : IScene{ init }{}

	void update() override {
		if (KeyTab.down()) {
			changeScene(State::Menu);
		}
	}
	
	
};
