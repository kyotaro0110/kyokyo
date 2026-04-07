#pragma once
#include "stdafx.h"

class ControllerModelDrawer
{
private:
	// 送信文字表示用のフォント
	Font m_font{ FontMethod::MSDF, 60, Typeface::Bold };
	size_t m_playerIndex;
	// 形状データ
	Vec2 m_center;
	double m_size;
	s3d::Polygon m_controllerShape;
	Array<Circle> m_bigCircles;
	Circle m_viewButton, m_menuButton;
	Array<Circle> m_abxyButtons;
	s3d::Polygon m_crossKey;
	RectF m_buttonL, m_buttonR;
	RectF m_triggerL, m_triggerR;
	double m_distButton;
	double m_rCirSmall;
	double m_topY;

	// 形状を計算するプライベート
	void calculateGeometry()
	{
		const double r_Hex = m_size;
		const double Dist_Cir_Big = r_Hex * Sqrt(3.0) / (1.0 + Sqrt(3.0));
		const double r_Cir_Big = r_Hex * Sqrt(3.0) / (2.0 * (1.0 + Sqrt(3.0)));
		const double Dist_Cir_Small = r_Hex * (2.0 * Sqrt(3.0) + 1.0 - 2.0 * Sqrt(1.0 + Sqrt(3.0))) / (2.0 * (1.0 + Sqrt(3.0)));
		m_rCirSmall = Dist_Cir_Small / Sqrt(3.0);
		const double r_Button = r_Cir_Big / (1.0 + Sqrt(2.0));
		m_distButton = r_Button * Sqrt(2.0);
		m_topY = m_center.y - r_Hex * Sqrt(3.0) / (2.0 * (1.0 + Sqrt(3.0)));
		const Vec2 Corner_L = { m_center.x - r_Hex * (1.0 + 2.0 * Sqrt(3.0)) / (2.0 * (1.0 + Sqrt(3.0))), m_topY };
		const Vec2 Corner_R = { m_center.x + r_Hex * (1.0 + 2.0 * Sqrt(3.0)) / (2.0 * (1.0 + Sqrt(3.0))), m_topY };

		m_controllerShape = s3d::Polygon{
			Corner_L, Corner_R,
			Vec2{m_center.x + r_Hex * cos(0_deg), m_center.y + r_Hex * sin(0_deg)},
			Vec2{m_center.x + r_Hex * cos(60_deg), m_center.y + r_Hex * sin(60_deg)},
			Vec2{m_center.x + r_Hex * cos(120_deg), m_center.y + r_Hex * sin(120_deg)},
			Vec2{m_center.x + r_Hex * cos(180_deg), m_center.y + r_Hex * sin(180_deg)}
		};

		m_bigCircles.clear();
		for (int i = 0; i < 4; i++) {
			m_bigCircles.emplace_back(m_center + Vec2{ Dist_Cir_Big, 0 }.rotated((i * 60_deg)), r_Cir_Big);
		}

		m_viewButton = Circle{ m_center.x - Dist_Cir_Small, m_center.y + m_rCirSmall, m_rCirSmall };
		m_menuButton = Circle{ m_center.x + Dist_Cir_Small, m_center.y + m_rCirSmall, m_rCirSmall };

		m_abxyButtons.clear();
		for (int i = 0; i < 4; i++) {
			m_abxyButtons.emplace_back(m_bigCircles[0].center + Vec2{ m_distButton, 0 }.rotated((i * 90_deg)), r_Button);
		}

		m_crossKey = Shape2D::Plus(r_Cir_Big - 10, 30, m_bigCircles[3].center, 0.0).asPolygon();
		m_buttonL = RectF{ Corner_L.x, Corner_L.y - m_rCirSmall, m_rCirSmall * 2, m_rCirSmall };
		m_buttonR = RectF{ Corner_R.x - m_rCirSmall * 2, Corner_R.y - m_rCirSmall, m_rCirSmall * 2, m_rCirSmall };
		m_triggerL = RectF{ Corner_L.x + m_rCirSmall * 2, Corner_L.y - m_rCirSmall * 2, m_rCirSmall * 2, m_rCirSmall * 2 };
		m_triggerR = RectF{ Corner_R.x - m_rCirSmall * 4, Corner_L.y - m_rCirSmall * 2, m_rCirSmall * 2, m_rCirSmall * 2 };
	}

public:
	explicit ControllerModelDrawer(size_t index, const Vec2& center = Scene::Center(), double size = 100.0)
		: m_playerIndex(index),
		  m_center(center),
		  m_size(size)
		
	{
		calculateGeometry();
	}

	void setPos(const Vec2& center)
	{
		m_center = center;
		calculateGeometry();
	}

	void draw() const
	{
		const auto gamepad = Gamepad(m_playerIndex);
		if (gamepad.isConnected()) {
			m_controllerShape.draw(Palette::Steelblue).drawFrame(4, Palette::Black);
			for (const auto& circle : m_bigCircles) { circle.draw(Palette::Lightgray).drawFrame(2, Palette::Black); }

			m_crossKey.draw(Palette::White);
			const Vec2 dpad_dir{ (double)gamepad.povRight.pressed() - gamepad.povLeft.pressed(), (double)gamepad.povDown.pressed() - gamepad.povUp.pressed() };
			if (!dpad_dir.isZero()) {
				Circle{ m_bigCircles[3].center + dpad_dir.withLength(m_distButton), m_rCirSmall }.draw(Palette::Crimson);
			}

			m_abxyButtons[0].draw(gamepad.buttons[0].pressed() ? Palette::Crimson : Palette::Snow).drawFrame(3, Palette::Black); // B on Xbox
			m_abxyButtons[1].draw(gamepad.buttons[1].pressed() ? Palette::Crimson : Palette::Snow).drawFrame(3, Palette::Black); // A on Xbox
			m_abxyButtons[2].draw(gamepad.buttons[2].pressed() ? Palette::Crimson : Palette::Snow).drawFrame(3, Palette::Black); // Y on Xbox
			m_abxyButtons[3].draw(gamepad.buttons[3].pressed() ? Palette::Crimson : Palette::Snow).drawFrame(3, Palette::Black); // X on Xbox

			Circle{ m_bigCircles[2].center + m_distButton * Vec2{gamepad.axes[0], gamepad.axes[1]}, m_rCirSmall }.draw(gamepad.buttons[8].pressed() ? Palette::Crimson : Palette::White).drawFrame(3, Palette::Black);
			Circle{ m_bigCircles[1].center + m_distButton * Vec2{gamepad.axes[3], gamepad.axes[4]}, m_rCirSmall }.draw(gamepad.buttons[9].pressed() ? Palette::Crimson : Palette::White).drawFrame(3, Palette::Black);

			m_viewButton.draw(gamepad.buttons[6].pressed() ? Palette::Crimson : Palette::Snow).drawFrame(3, Palette::Black);
			m_menuButton.draw(gamepad.buttons[7].pressed() ? Palette::Crimson : Palette::Snow).drawFrame(3, Palette::Black);

			m_buttonL.rounded(10, 10, 0, 0).draw(gamepad.buttons[4].pressed() ? Palette::Crimson : Palette::Snow).drawFrame(3, Palette::Black);
			m_buttonR.rounded(10, 10, 0, 0).draw(gamepad.buttons[5].pressed() ? Palette::Crimson : Palette::Snow).drawFrame(3, Palette::Black);
			//m_triggerL.rounded(10, 10, 0, 0).draw(gamepad.buttons[12].pressed() ? Palette::Crimson : Palette::Snow).drawFrame(3, Palette::Black);
			//m_triggerR.rounded(10, 10, 0, 0).draw(gamepad.buttons[13].pressed() ? Palette::Crimson : Palette::Snow).drawFrame(3, Palette::Black);

			m_triggerL.rounded(10, 10, 0, 0).draw(Palette::White).drawFrame(3, Palette::Black);
			//m_triggerL.stretched((gamepad.axes[2] - 1.0) * m_triggerL.h, 0, 0, 0).rounded(10, 10, 0, 0).draw(Palette::Crimson).drawFrame(3, Palette::Black);
			m_triggerR.rounded(10, 10, 0, 0).draw(Palette::White).drawFrame(3, Palette::Black);
			//m_triggerR.stretched((-gamepad.axes[2] - 1.0) * m_triggerR.h, 0, 0, 0).rounded(10, 10, 0, 0).draw(Palette::Crimson).drawFrame(3, Palette::Black);
		}
	}
};
