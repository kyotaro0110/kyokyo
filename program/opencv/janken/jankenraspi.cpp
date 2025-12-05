#include <opencv2/opencv.hpp>
#include "function1.hpp"

using namespace std;
using namespace cv;
using namespace chrono;

int main()
{
    // ---- 設定 ----
    const char *PC_IP = "192.168.23.145"; // 受信PCのIP
    const int PC_PORT = 5000;             // 受信PCのポート
    const int CAMERA_INDEX = 1;           // USBカメラ
    const double DEBOUNCE_SECONDS = 3.0;  // 連続送信抑制
    srand((unsigned)time(NULL));

    // ---- カメラ準備 ----
    VideoCapture cap(CAMERA_INDEX);

    if (!cap.isOpened())
    {
        cerr << "カメラが開けません\n";
        return -1;
    }

    // ---- HOG + SVM (人検知) ----
    HOGDescriptor hog;                                             // 人検出器
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector()); // 人の短形領域を取得

    auto last_sent = chrono::steady_clock::now() - chrono::seconds(10);

    CascadeClassifier face_cascade;
    string cascade_path = "C:/opencv/build/etc/haarcascades/haarcascade_frontalface_default.xml";
    if (!face_cascade.load(cascade_path))
    {
        cerr << "顔検出読み込み失敗" << cascade_path << endl;
        return -1;
    }
    if (face_cascade.empty())
    {
        cerr << "顔検出器読み込み失敗\n";
        return -1;
    }
    const int frequency = 20;
    int finger[frequency];
    int frefinger = 0;
    for (int i = 0; i < frequency; i++)
        finger[i] = 0;
    int cpujanken[3]; // 0:グー、1:チョキ、2:パー
    int personjanken[3];
    for (int i = 0; i < 3; i++)
    {
        cpujanken[i] = 0;
        personjanken[i] = 0;
    }
    int s = 0;
    cout << "enterキーを押すとじゃんけん勝負が始まります。" << endl;

    bool paused = false;
    bool ready = false;
    int a = -1, b = 01, flag = 0, state = 0;
    int a_save = 0;
    int temp_a;
    while (true)
    {
        if (flag == 0)
        {
            s++;
            if (s == frequency * 7)
                s = 0;

            Mat frame;
            if (!paused)
            {
                cap >> frame;
                if (frame.empty())
                    break;
            }
            else
            {
                Mat subframe;
                cap >> subframe;
                if (subframe.empty())
                    break;
                frame = subframe.clone();
            }
            Mat resized;

            // imshow("frame", resized);

            int x = frame.cols / 4;
            int y = frame.rows / 4;
            int w = frame.cols / 2;
            int h = frame.rows / 2;
            Rect roi(x, y, w, h);

            Mat frame_roi = frame(roi);

            // リサイズして高速化
            Mat frame_proc;
            double scale = frame.cols > 640 ? 640.0 / frame.cols : 1.0;

            if (scale != 1.0)
                resize(frame, frame_proc, Size(), scale, scale);

            else
                frame_proc = frame;

            // マスク処理　さまざまな色を黒か白
            Mat gray, hsv, prevGray, diff, mask, mask1, mask2;
            cvtColor(frame, gray, COLOR_BGR2GRAY); // grayscale変換
            cvtColor(frame, hsv, COLOR_BGR2HSV);
            Scalar lower(0, 30, 60);
            Scalar upper(30, 150, 255);

            inRange(hsv, lower, upper, mask); //(入力,下限値、上限値、出力)

            erode(mask, mask, Mat(), Point(-1, -1), 2);
            dilate(mask, mask, Mat(), Point(-1, -1), 2);

            vector<Rect> faces;
            face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(100, 100));

            for (auto &f : faces)
            {
                rectangle(mask, f, Scalar(0), FILLED);
                // Rect neck(face.x, face.y + face.height,)
            }

            // static auto last_face_time = chrono::steady_clock::now() - chrono::seconds(20);
            // static bool face_block = false;

            // if(face_block){
            //     auto now = chrono::steady_clock::now();
            //     if(now - last_face_time < chrono::seconds(10)){
            //         mask = Mat::zeros(mask.size(),mask.type());
            //     } else {
            //         face_block = false;
            //     }
            // }

            Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
            morphologyEx(mask, mask, MORPH_OPEN, kernel); // ノイズ除去
            morphologyEx(mask, mask, MORPH_CLOSE, kernel);

            // 輪郭抽出
            vector<vector<Point>> contours;
            findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

            for (size_t i = 0; i < contours.size(); i++)
            {
                drawContours(mask, contours, (int)i, Scalar(255), FILLED);
            }

            if (!contours.empty())
            {
                // 面積が最大の輪郭を選ぶ

                drawContours(frame, contours, maxIndex(contours), Scalar(0, 255, 0), 2);

                if (maxArea(contours) < 1500)
                    continue;
                // 谷間を計算

                vector<Point> approx;
                double epsilon = 0.01 * arcLength(contours[maxIndex(contours)], true);
                approxPolyDP(contours[maxIndex(contours)], approx, epsilon, true);
                // fingercalc(contours,maxIndex(contours));

                // imshow("hull", mask);
                resize(frame, resized, Size(250, 250));
                imshow("frame", resized);
                imshow("mask",mask);
                vector<cv::Rect> rects;
                //  cout << fingercount1(contours,maxIndex(contours)) << endl;
                // jankenposeseen(contours,maxIndex(contours),frequency,&s,finger);
                // jankenimg(a);

                // 表示用（オプション）
                for (auto &r : rects)
                {
                    rectangle(frame_proc, r, Scalar(0, 255, 0), 2);
                }
                int f = 1;
                int Key = waitKey(1);
                if (Key == 27)
                    break; // ESCで終了
                if (Key == 13 || Key == 10 || Key == '\r')
                {
                    if (!paused && !ready)
                    {

                        cout << "じゃんけん" << endl;
                        auto start = steady_clock::now();
                        s = 0;
                        const int sample_count = 10;
                        while (chrono::steady_clock::now() - start < chrono::seconds(2))
                        {
                            cap >> frame;
                            if (frame.empty())
                                break;

                            // マスク作成
                            cvtColor(frame, gray, COLOR_BGR2GRAY);
                            cvtColor(frame, hsv, COLOR_BGR2HSV);
                            Scalar lower(0, 30, 60), upper(30, 150, 255);
                            inRange(hsv, lower, upper, mask);
                            erode(mask, mask, Mat(), Point(-1, -1), 2);
                            dilate(mask, mask, Mat(), Point(-1, -1), 2);

                            // 輪郭計算
                            vector<vector<Point>> contours;
                            findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

                            // 最大輪郭の指数を取得
                            if (!contours.empty())
                            {
                                int idx = maxIndex(contours);
                                finger[s % sample_count] = fingercount1(contours, idx);
                            }
                            else
                            {
                                finger[s % sample_count] = 0;
                            }

                            s++;
                            waitKey(1);
                        }
                        frefinger = frequentNum(finger, frequency);

                        fingerhandle(fingercount1(contours, maxIndex(contours)), &a);
                        jankenimg(a, &frame, 1);
                        cout << "あなたは" << (a == 0 ? "グー" : a == 1 ? "チョキ"
                                                                        : "パー")
                             << "を出しています（ENTERキーを押してね）" << endl;
                        paused = true;
                        ready = true;
                    }
                    else if (paused && ready)
                    {
                        // fingerhandle(frefinger,&a);
                        jankenpose(&b);
                        jankenimg(b, &frame, 0);
                        // cout << a << b << endl;

                        judge(a, b, &flag);

                        if (flag == 0)
                        {
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                            paused = false;
                            ready = false;
                            a = -1;
                        }
                        else
                        {
                            paused = false;
                            ready = false;
                            flag = 0;
                            a = -1;
                        }
                    }
                }
            }
        }
    }
    // close(sock);
    return 0;
}

