#pragma once
#include "stdafx.h"

struct GamepadState {
	char direction;
	size_t magnitude;
};

struct ServoPoint {
	double x;
	double y;
};

class Controller {
private:
	
protected:
	size_t playerIndex = 0;
	Font m_drawfont{ FontMethod::MSDF, 60, Typeface::Bold };

	
public:
	explicit Controller() = default;
	virtual ~Controller() = default;

	char button() const {
		uint32_t state = 0;
		int count = 0;
		const auto gamepad = Gamepad(playerIndex);
		if (!gamepad.isConnected())
		{
			return 'z';
		}
		else {
			for (int i = 0; i < 10; ++i) {
				if (gamepad.buttons[i].pressed()) {
					state |= (1 << i);
					count++;
				}
			}

			if (count > 2) {
				return 'k';
			}

			switch (state) {
				case(1 << 0) | (1 << 1) :
					return 'w';
				case(1 << 0) | (1 << 2) :
					return 'l';
				case(1 << 0) | (1 << 3) :
					return 'm';
				case(1 << 0) | (1 << 4) :
					return 'n'; 
				case(1 << 0) | (1 << 5) :
					return 'o';
				case(1 << 0) | (1 << 6) :
					return 'p';
				case(1 << 0) | (1 << 7) :
					return 'q';
				case(1 << 0) :
					return 'a';
				case(1 << 1) :
					return 'b';
				case(1 << 2) :
					return 'c';
				case(1 << 3) :
					return 'd';
				case(1 << 4) :
					return 'e';
				case(1 << 5) :
					return 'f';
				case(1 << 6) :
					return 'g';
				case(1 << 7) :
					return 'h';
				case(1 << 8) :
					return 'i';
				case(1 << 9) :
					return 'j';
			}
			return 'k';
		}
	}

	void StickCondition(int index) const {
		const auto gamepad = Gamepad(index);
		if (gamepad.isConnected()) {
			double x = gamepad.axes[0];
			double y = gamepad.axes[1]*(- 1);

			if (Abs(x) < 0.005) x = 0.0;
			if (Abs(y) < 0.005) y = 0.0;

			m_drawfont(U"Stick X: {:.2f}"_fmt(x)).draw(800, 550, Palette::Black);
			m_drawfont(U"Stick Y: {:.2f}"_fmt(y)).draw(800, 620, Palette::Black);
		}
	}

	GamepadState Command(size_t MaxSpeed) const {
		const auto gamepad = Gamepad(playerIndex);
		if (!gamepad.isConnected()) {
			return { 'z', 0};
		}
		if (gamepad.isConnected()) {
			double x = gamepad.axes[0];
			double y = gamepad.axes[1] * -1;

			double mag = s3d::Vec2{ x, y }.length();
			mag = s3d::Min(mag, 1.0);

			uint8 magInt = static_cast<uint8>(mag * MaxSpeed);

			if (mag < 0.01) return { 'k', 0};

			const double threshold = 0.15;

			char dir = 'k';

			if (y > threshold) {
				if (x > threshold) dir = 'b';
				else if (x < -threshold) dir = 'd';
				else dir = 'c';
			}
			else if (y < -threshold) {
				if (x > threshold) dir = 'h';
				else if (x < -threshold) dir = 'f';
				else dir = 'g';
			}
			else {
				if (x > threshold) dir = 'a';
				else if (x < -threshold) dir = 'e';
			}
			
			return { dir, magInt };
		}
	}
};

class Controller_2th : public Controller {
public:
	//using Controller::Controller;
	explicit Controller_2th() : Controller() {}

protected:
	ServoPoint ServoCondition() const {
		const auto gamepad = Gamepad(1);
		ServoPoint sp{ 0,0 };
		if (gamepad.isConnected()) {
			sp.x = gamepad.axes[0]+1.0;
			sp.y = gamepad.axes[1] * (-1)+1.0;

			sp.x *= 135.0;
			sp.y *= 135.0;

			if (Abs(sp.x) < 1.005) sp.x = 0.0;
			if (Abs(sp.y) < 1.005) sp.y = 0.0;

			/*m_drawfont(U"Stick X: {:.2f}"_fmt(sp.x)).draw(800, 550, Palette::Black);
			m_drawfont(U"Stick Y: {:.2f}"_fmt(sp.y)).draw(800, 620, Palette::Black);*/

		}

		return sp;
	}
	
};
