#pragma once
#include "stdafx.h"
#include "Controller.hpp"

class ButtonsDrawer : public Controller
{
private:
    //size_t playerIndex = 0;
    //const Array<String> buttonnames = { U"Y", U"B", U"A", U"X", U"l", U"r", U"L", U"R", U"N", U"N", U"N", U"N", U"N", U"N", U"N", U"U", U"M", U"S" };
	Font m_font{ FontMethod::MSDF, 30, Typeface::Bold };
	Font font{ FontMethod::MSDF, 20, Typeface::Bold };

	Vec2 getRightBottomPos(int dx,int dy) const {
		return Vec2{ dx, Scene::Height() - dy };
	}

public:
    explicit ButtonsDrawer() : Controller (){}

	void drawInput(size_t MaxSpeed) const
	{
		//char currentButton = Command().button;
		char currentButton = button();

		if (currentButton == 'z')
		{
			font(U"Controller is not connected").draw(getRightBottomPos(50, 10), Palette::Black);
			return;
		}

		if (currentButton != 'z')
		{
			m_font(U"Button").drawAt(getRightBottomPos(200,100), Palette::Red);
			m_font(currentButton).drawAt(getRightBottomPos(130,100), Palette::Red);
		}

		/*const auto gamepad = Gamepad(0);
		Print << U"ボタン数: " << gamepad.buttons.size();
		Print << U"軸数: " << gamepad.axes[2];*/

		char stickDirection = Command(MaxSpeed).direction;
		size_t stickMagnitude = Command(MaxSpeed).magnitude;
		m_font(U"Dir").drawAt(getRightBottomPos(200,70), Palette::Red);
		m_font(stickDirection).drawAt(getRightBottomPos(130,70), Palette::Red);
		m_font(U"Mag").drawAt(getRightBottomPos(200,40), Palette::Red);
		m_font(stickMagnitude).drawAt(getRightBottomPos(130,40), Palette::Red);

		//StickCondition(1);
	}

};
