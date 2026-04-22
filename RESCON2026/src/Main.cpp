# include "stdafx.h"
# include "DeviceConfig.h"
# include "ControllerSender.hpp"
# include "ButtonsDrawer.hpp"
# include "ControllerModelDrawer.hpp"
# include "menu.hpp"
# include "PiRobot.hpp"
# include "SoundAnalyzer.hpp"
# include "Pulldown.hpp"

enum class AppState {
	Home,
	InputViewer
};

void SetupDeviceInstances(
	const s3d::Array<DeviceInfo>& devices,
	//s3d::Array<std::unique_ptr<IVideoReceiver>>& videoReceivers,
	//s3d::Array<std::unique_ptr<ISoundReceiver>>& soundReceivers,
	s3d::Array<std::unique_ptr<Controller>>& controllers)
{
	//videoReceivers.clear();
	//soundReceivers.clear();
	controllers.clear();

	//複数のデバイスを同時に初期化
	//for (const auto& device : devices)
	//{
	//	if (device.type == DeviceType::RaspberryPi)
	//	{
	//		//videoReceivers.push_back(std::make_unique<Video_rsp>(device.ipAddress, device.videoPort));
	//		//soundReceivers.push_back(std::make_unique<Sound_rsp>(device.ipAddress, device.soundPort));
	//	}
	//	else if (device.type == DeviceType::TPIP4)
	//	{
	//		//videoReceivers.push_back(std::make_unique<Video_rsp>(device.ipAddress, device.videoPort));
	//		//soundReceivers.push_back(std::make_unique<Sound_rsp>(device.ipAddress, device.soundPort));
	//	}
	// controllers.push_back(std::make_unique<Controller>(device.ipAddress, device.commandPort));
	//}
}

void Main()
{
	//double bitx = Scene::Size().x / 12.0;
	//double bity = Scene::Size().y / 12.0;

	// 1. フォントの準備 (MSDF形式は拡大しても綺麗です)
	const double cm = 37.8;
	//const double mm = 3.78;
	bool isFull = false;
	static size_t modeIndex = 0;
	Window::Resize(1280, 720);
	
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };
	const Font iconFont{ FontMethod::MSDF, 40, Typeface::Icon_Awesome_Solid };
	const Rect buttonArea = RectF{ Arg::center(400, 400), 200, 60 }.asRect();
	const RectF leftArea{ 300,130, 80, 30 };
	const RectF rightArea{ leftArea.rightX() + leftArea.w + 20 ,leftArea.topY(), leftArea.w, leftArea.h};
	const RectF Frame{ leftArea.leftX() ,9 * cm,24 * cm,2 * cm};
	const RectF SidebarArea{ 0, 0, 120, 1000 };
	const RectF MenuHome{ 0, 100, 120, 60 };
	const RectF MenuController{ 0, 160, 120, 60 };
	const RectF MenuSettings{ 0, 220, 120, 60 };
	const RectF CameraCapture{ 350,75,700,700 };
	const RectF SubCameraCapture{ 1080,75,420,420 };
	const RectF QR{ 1120, 550, 300, 300 };
	const RectF SoundPattern{ 1120, 550, 400, 100 };
	Pulldown freqSelector(&AudioConfig::frequencyNames, Font{ 20 }, 20, Vec2{ 1300, SoundPattern.topY() - 50});
	
	
	//size_t index = 0;
	size_t MaxSpeed = 40;
	int ActiveRobot = -1 , PrevRobot = -1;
	const size_t maxHistory = 120; // 0.1秒ごとの更新なら12秒分
	double m_spectrumScale = 0.5; //mutableはconstの状況でも変更を許可する

	//操作ロボット選択ボタンの位置設定
	Array<RectF> selectButtons;
	for (int i = 0; i < 4; ++i) {
		selectButtons << RectF{ leftArea.leftX() + (i * 6 * cm), 6 * cm, 6 * cm, 3 * cm };
	}

	//画面切り替えスライドバーの位置設定
	const RectF AppMain{ 10,150,240,80 };
	const RectF AppControllerDrawer{ 10, AppMain.bottomY()+20,240,80};
	const auto& deviceList = DeviceConfig_RRC23::g_devices_RRC23;
	const auto& deviceList_eth0 = DeviceConfig_RRC23::g_devices_eth;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		// 失敗した場合はメッセージを出して終了
		System::MessageBoxOK(U"ネットワークの初期化に失敗しました。");
		return;
	}

	//デバイス各々にポインターを配列により作成
	Array<std::unique_ptr<IRobot>> robots;

	//有線無線の選択インデックス
	int32 LANindex = 0; //0なら無線、１なら有線
	int32 prevLANindex = 1;

	//動かすロボットと動かさないロボットをBool変数で管理
	Array<bool> robotSelection = { false, false, false, false };
	//Array<bool> activeRobot = { false, false, false, false };
	Window::SetStyle(WindowStyle::Sizable);
	Profiler::EnableAssetCreationWarning(false);

	//デフォルトの画面をHOMEに設定
	AppState AppState = AppState::Home;

	//コントローラーの入力状況の可視化のための変数設定
	ButtonsDrawer buttonsdrawer{};
	ControllerModelDrawer controllermodeldrawer{ 0 };
	ControllerModelDrawer controllermodeldrawer1{ 1 };

	//Array<std::unique_ptr<ControllerModelDrawer>> controllermodeldrawers;
	//Array<std::unique_ptr<ButtonsDrawer>> buttonsdrawers;

	//for(auto[i, controller])

	//アクティブカラーとインアクティブカラーの設定
	const Color OperatedColor = Palette::Red;
	const Color ActiveColor = Palette::Orange;
	const Color InactiveColor = Palette::Gray;

	while (System::Update())
	{
		//背景を白

		Scene::SetBackground(Palette::White);

		//F11で画面最大化
		if (KeyF11.down())
		{
			isFull = !isFull;
			Window::SetFullscreen(isFull);
		}
		// 2. タイトルの描画 (画面中央に配置)
		//font(U"SURF(静岡大学ロボットファクトリー)").drawAt(30, Vec2{ 400,50 }, Palette::Black);

		RectF{ 0,0,60,1000 }.draw(ColorF{ 0.2, 0.2, 0.2 }).drawFrame(2, Palette::Skyblue);
		iconFont(char32(0xf0c9)).drawAt(30, 30, 40, Palette::White);
		//HomeとControllerの画面切り替えにより、色が変わる(HomeとControllerの文字の色)

		if (prevLANindex != LANindex)
		{
			robots.clear();
			robotSelection.fill(false);
			ActiveRobot = -1;
			PrevRobot = -1;

			const auto& currentDeviceList = (LANindex == 0) ? deviceList : deviceList_eth0;
			for (const auto& info : currentDeviceList)
			{
				try
				{
					std::unique_ptr<IRobot> robot;
					if (info.type == DeviceType::RaspberryPi) {
						robot = std::make_unique<PiRobot>(info);
					}

					// ここで接続を試みる
					if (robot && robot->initialize()) {
						robots.push_back(std::move(robot));
					}
					else {
						// 接続に失敗したことを画面に出す（後述のPrintなどで）
						System::MessageBoxOK(U"接続エラー: {}"_fmt(info.name));
						robots.push_back(nullptr);
					}
				}
				catch (const std::exception& e)
				{
					// 予期せぬエラー（ソケット作成失敗など）が起きても、
					// アプリを落とさずにコンソールへエラーを表示
					Console << U"Critical Error ({}): "_fmt(info.name) << Unicode::FromUTF8(e.what());
					robots.push_back(nullptr);
				}
			}
		}

		prevLANindex = LANindex;

		if (SidebarArea.mouseOver()) {
			SidebarArea.draw(ColorF{ 0.2, 0.2, 0.2 }); // 暗めのグレーで高級感を出す
			iconFont(char32(0xf0c9)).drawAt(30, 30, 40, Palette::White);
			// 2. メニューボタンの描画
				MenuHome.draw(ColorF{ 0.3 }); // ホバー時
				if (MenuHome.leftClicked()) {
					AppState = AppState::Home;
					ClearPrint();
				}
			
			iconFont(char32(0xf015)).drawAt(25, MenuHome.center(), (AppState == AppState::Home) ? Palette::Orange : Palette::White);

				//操作画面へ切り替えのプログラム
				MenuController.draw(ColorF{ 0.3 });
				if (MenuController.leftClicked()) {
					AppState = AppState::InputViewer;
					ClearPrint();
				}

			//アイコンの描画
			iconFont(char32(0xf11b)).drawAt(25, MenuController.center(), (AppState == AppState::InputViewer) ? Palette::Orange : Palette::White);


			// 3. メイン画面の描画（サイドバーの右側から開始）
			const RectF MainContentArea{ 240, 0, 1040, 720 };
		}
		//Shift押しながらJまたは枠クリックで無線接続に変更


		//初期化フェーズ
		
		//ショートカットキーの作成（使うかわからん、ただの遊び心）
		if (KeyShift.pressed() && KeyJ.down() || leftArea.mouseOver())
		{
			if (KeyShift.pressed() && KeyJ.down())LANindex = 0;     // クリックで「無線」選択
			else
				if (MouseL.down()) LANindex = 0;

		}

		if (KeyTab.down() && ActiveRobot != -1 && PrevRobot != -1) {
			std::swap(ActiveRobot, PrevRobot);
		}

		//Shift押しながらLまたは枠クリックで有線接続に変更

		if (KeyShift.pressed() && KeyL.down() || rightArea.mouseOver())
		{
			if (KeyShift.pressed() && KeyL.down())
				LANindex = 1;     // クリックで「有線」選択
			else
				if (MouseL.down()) LANindex = 1;
		}

		//選択したロボットのみ、操作可能状態にする

		for (int i = 0; i < 4; ++i) {
			if (robotSelection[i] && i < robots.size() && robots[i] != nullptr && ActiveRobot == i)
				robots[i]->update(MaxSpeed);
		}

		//枠内クリックで画面切り替え

		if (AppState == AppState::Home) {
			leftArea.drawFrame(2, Palette::Black);
			rightArea.drawFrame(2, Palette::Black);
			Frame.drawFrame(2, Palette::Black);
			const Vec2 center = Scene::Center();

			//有線無線の選択ボタンの追加
			font(U"無線").drawAt(30, leftArea.center(), (LANindex == 0) ? ActiveColor : InactiveColor);
			font(U"有線").drawAt(30, rightArea.center(), (LANindex == 1) ? ActiveColor : InactiveColor);

			//1～4号機の選択ボタンの追加
			for (auto [i, button] : Indexed(selectButtons))
			{
				if (button.leftClicked())
				{
					// 接続状態を反転
					if(std::count(robotSelection.begin(), robotSelection.end(), true) < 2)
					robotSelection[i] = !robotSelection[i];

					if (robotSelection[i]) {
						// 新しく接続した時、空いているスロットがあれば入れる
						if (ActiveRobot == -1) {
							ActiveRobot = (int)i;
						}
						else if (PrevRobot == -1) {
							PrevRobot = (int)i;
						}
					}
					else {
						// 接続解除した時、スロットから外す
						if (ActiveRobot == (int)i) ActiveRobot = -1;
						if (PrevRobot == (int)i) PrevRobot = -1;
					}
				}

				button.drawFrame(2, Palette::Black);

				// 三項演算子で色を決定（操作中 > 待機中 > その他）
				const Color btnColor = (ActiveRobot == (int)i) ? OperatedColor :
					(PrevRobot == (int)i) ? ActiveColor :
					(robotSelection[i]) ? Palette::Lightgray : InactiveColor;

				font(U"{}号機"_fmt(i + 1)).drawAt(25, button.center(), btnColor);
			}
			font(U"全ての接続をリセット").drawAt(25, Frame.center(), Palette::Gray);
			if (Frame.leftClicked()) {
				robots.clear();
				robotSelection.fill(false);
				ActiveRobot = -1;
				PrevRobot = -1;
				prevLANindex = -1;
				ClearPrint();
				System::MessageBoxOK(U"全ての接続をリセットしました。");
			}
		}

		else if (AppState == AppState::InputViewer)
		{
			//コントローラー状態の可視化
			font(U"操作用").draw(25, 150, Scene::Height() - 300, Palette::Black);
			controllermodeldrawer.setPos(Vec2{ 200, Scene::Height() - 200 });
			controllermodeldrawer.draw();
			buttonsdrawer.drawInput(MaxSpeed);
			CameraCapture.drawFrame(2, Palette::Black);
			SubCameraCapture.drawFrame(2, Palette::Black);
			if (modeIndex == 0) {
				QR.drawFrame(2, Palette::Black);
			}
			else {
				SoundPattern.drawFrame(2, Palette::Black);
			}
			font(U"{}号機操作中"_fmt(ActiveRobot+1)).draw(25, 620, 25, Palette::Red);
			font(U"{}号機待機中"_fmt(PrevRobot+1)).draw(25, 1250, 25, Palette::Orange);
			//font(U"解析").draw(15, 1250, SoundPatternAnarayze.topY() - 50, Palette::Black);
			SimpleGUI::HorizontalRadioButtons(modeIndex, { U"QR", U"聴音" }, Vec2{ 1120,SoundPattern.topY() - 50});
			
			
			//2号機を操作中のみサブコントローラーの入力状況の描画
			if (robotSelection[1]) {
				controllermodeldrawer1.setPos(Vec2{ 200, Scene::Height() - 500 });
				controllermodeldrawer1.draw();
			}

			//オーディオの処理（仮）

			freqSelector.update();
			//Scene::SetBackground(Palette::White);
			size_t freq_index = freqSelector.getIndex();
			//size_t selectedFreq = 

			//今操作しているロボット固有の情報の描画
			/*if (activeRobotIndex < robots.size()) {
				robots[activeRobotIndex]->draw();
			}*/

			//全ロボットの情報の表示
			
			for (int i = 0; i < 4; ++i)
			{
				if (robotSelection[i] && i < robots.size() && robots[i] != nullptr && i == ActiveRobot) {
					robots[i]->analyzerUI(CameraCapture, ActiveRobot, i);
				}
				if (robotSelection[i] && i < robots.size() && robots[i] != nullptr && i == ActiveRobot && modeIndex == 1) {
					robots[i]->drawSignalHistory(SoundPattern, ActiveRobot, i, freq_index);
				}
				if(modeIndex == 1)
					freqSelector.draw();
				const auto& currentList = (LANindex == 0) ? DeviceConfig_RRC23::g_devices_RRC23
					: DeviceConfig_RRC23::g_devices_eth;

				const auto& info = currentList[i];
				double x = 200;
				double y = 20 + i * 30;
				font(U"{}号機:IPアドレス{}"_fmt(i + 1, info.toipAddress)).drawAt(15, { x,y }, (robotSelection[i] == true) ? ((ActiveRobot == i) ? OperatedColor : ActiveColor) : InactiveColor);
			}

		}

		//ESCキーで終了
		
		if (KeyEscape.down())
		{
			break;
		}
		
	}

	robots.clear();

	WSACleanup();
}
