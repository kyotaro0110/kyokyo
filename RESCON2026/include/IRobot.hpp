#pragma once
#include "stdafx.h"
#include "DeviceConfig.h"

class IRobot {
public:
	virtual ~IRobot() = default;

	//共通で行うべき操作を準仮想関数で定義
	virtual bool initialize() = 0;
	virtual void update(size_t MaxSpeed) = 0;
	virtual bool isConnected() = 0;
	virtual void draw(const Vec2& pos) const {}
	virtual void analyzerUI(const RectF& region) const{}
};
