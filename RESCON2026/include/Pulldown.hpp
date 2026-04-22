// Pulldown.hpp
#pragma once
#include <Siv3D.hpp>

# define M_ITEMS_DEFALT_FACTOR U"Waiting for QR code..."

class Pulldown
{
private:
	static constexpr Size Padding{ 8, 2 };
	static constexpr int32 DownButtonSize = 16;
	static constexpr ColorF TextColor1{ 0.11 };
	static constexpr ColorF TextColor2{ 0.70 };

	ColorF TextColor = TextColor1;
	Font m_font;
	double m_fontSize = 12;

	Array<String>* p_m_items = nullptr;
	Array<String> m_items;
	int p_m_items_size = 0;
	size_t m_index = 0;
	double m_maxitemWidth = 0;
	RectF m_rect{ 0 };
	bool m_isOpen = false;

	ListBoxState m_listBoxState;

	[[nodiscard]]
	double getMaxItemWidth() const
	{
		double result = 0.0;
		for (const auto& item : m_items)
		{
			result = Max(result, (m_font(item).region(m_fontSize).w));
		}
		return result;
	}

	[[nodiscard]]
	RectF getRect(const Vec2& pos) const noexcept
	{
		const double fontHeight = (m_font.height() * (m_fontSize / m_font.fontSize()));
		return{
			pos,
			(m_maxitemWidth + (Padding.x * 3 + DownButtonSize)),
			(fontHeight + Padding.y * 2)
		};
	}

public:
	Pulldown() = default;

	explicit Pulldown(s3d::Array<s3d::String>* items, const Font& font, const double fontSize, const Vec2& pos)
		: m_font{ font }
		, m_fontSize{ fontSize }
		, p_m_items{ items }
	{
		updateSourceItems();
		m_rect = getRect(pos);
	}

	void setSourceArray(s3d::Array<s3d::String>* newItems)
	{
		p_m_items = newItems;
		m_index = 0;
		updateSourceItems();
		m_rect = getRect(m_rect.pos);
	}

	void updateSourceItems()
	{
		m_items.clear();

		if ((not p_m_items) || p_m_items->empty()) {
			m_items << M_ITEMS_DEFALT_FACTOR;
			TextColor = TextColor2;
			p_m_items_size = 0;
			m_listBoxState.items = m_items;
		}
		else {
			m_items.append(*p_m_items);
			TextColor = TextColor1;
			p_m_items_size = p_m_items->size();
			m_listBoxState.items = *p_m_items;
		}
		m_maxitemWidth = getMaxItemWidth();
		m_listBoxState.selectedItemIndex = m_index;
	}

	void update()
	{
		if (p_m_items_size != (p_m_items ? (int)p_m_items->size() : 0))
		{
			updateSourceItems();
			m_rect = getRect(m_rect.pos);
		}

		const bool isEnabled = (p_m_items && (not p_m_items->empty()));
		if (not isEnabled)
		{
			m_isOpen = false;
			return;
		}

		if (m_rect.leftClicked())
		{
			m_isOpen = (not m_isOpen);
		}

		const RectF listBoxArea{ m_rect.bl(), m_rect.w, Min((double)Scene::Height() - m_rect.bottomY() - 10, m_rect.h * 10) };

		if (m_isOpen && MouseL.down() && (not m_rect.mouseOver()) && (not listBoxArea.mouseOver()))
		{
			m_isOpen = false;
		}
	}

	void draw()
	{
		const bool isEnabled = (p_m_items && (not p_m_items->empty()));
		m_rect.draw(isEnabled ? ColorF{ 1.0 } : ColorF{ 0.9 });
		m_rect.drawFrame(1, 0, m_isOpen ? Palette::Orange : Palette::Gray);

		if (m_index < m_items.size())
		{
			m_font(m_items[m_index]).draw(m_fontSize, (m_rect.pos + Padding), TextColor);
		}

		Triangle{ (m_rect.rightX() - DownButtonSize / 2.0 - Padding.x), (m_rect.y + m_rect.h / 2.0),
			(double)(DownButtonSize * 0.5), 180_deg }.draw(isEnabled ? TextColor : TextColor2);

		if (not m_isOpen) return;

		const double availableHeight = Scene::Height() - m_rect.bottomY() - 10;
		if (SimpleGUI::ListBox(m_listBoxState, m_rect.bl(), m_rect.w, availableHeight))
		{
			m_index = m_listBoxState.selectedItemIndex.value_or(0);
			m_isOpen = false;
		}
	}

	size_t getIndex() const { return m_index; }
}; 
