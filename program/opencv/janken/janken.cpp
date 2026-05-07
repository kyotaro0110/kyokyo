#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>

using namespace cv;
using namespace std;
using namespace chrono;

void jankenpose(int *a);
void judge(int a, int b, int *flag);
void pose();
void fingerhandle(int finger, int *a);
int maxIndex(const vector<vector<Point>> &contours);
double maxArea(const vector<vector<Point>> &contours);
void fingercalc(const vector<vector<Point>> &contours, int maxIndex);
int fingercount1(const vector<vector<Point>> &contours, int maxIndex);
int frequentNum(int a[], int N);
void jankenimg(int a, Mat *frame, int gameTurn);
void jankenposeseen(vector<vector<Point>> contours, int maxIndex(vector<vector<Point>> contours), int frequency, int *s, int finger[]);

int main()
{
    const int CAMERA_INDEX = 1;
    srand((unsigned)time(NULL));

    VideoCapture cap(CAMERA_INDEX);

    if (!cap.isOpened())
    {
        cerr << "カメラが開けません\n";
        return -1;
    }

    CascadeClassifier face_cascade;
    string cascade_path = "C:/opencv/build/etc/haarcascades/haarcascade_frontalface_default.xml";
    if (!face_cascade.load(cascade_path))
    {
        cerr << "顔検出読み込み失敗" << cascade_path << endl;
        return -1;
    }

    const int frequency = 20;
    int finger[frequency];
    for (int i = 0; i < frequency; i++) finger[i] = 0;
    int frefinger = 0;

    bool paused = false;
    bool ready = false;
    int a = -1, b = 0, flag = 0;
    int s = 0;

    cout << "enterキーを押すとじゃんけん勝負が始まります。" << endl;

    while (true)
    {
        if (flag == 0)
        {
            s++;
            if (s == frequency * 7) s = 0;

            Mat frame;
            if (!paused)
            {
                cap >> frame;
                if (frame.empty()) break;
            }
            else
            {
                Mat tmp; cap >> tmp;
                if (tmp.empty()) break;
                frame = tmp.clone();
            }

            Mat gray, hsv;

            // -----------【中央限定 ROI 定義】-----------
            int cx = frame.cols / 4;
            int cy = frame.rows / 4;
            int cw = frame.cols / 2;
            int ch = frame.rows / 2;

            Rect centerROI(cx, cy, cw, ch);
            Mat frame_center = frame(centerROI);

            // -----------【中央だけマスク処理】-----------
            Mat hsv_center;
            cvtColor(frame_center, hsv_center, COLOR_BGR2HSV);

            Scalar lower(0, 30, 60);
            Scalar upper(30, 150, 255);

            Mat mask_center;
            inRange(hsv_center, lower, upper, mask_center);

            // マスク全体は frame と同じサイズ（周囲は黒）
            Mat mask = Mat::zeros(frame.size(), CV_8UC1);
            mask_center.copyTo(mask(centerROI));

            // ノイズ除去
            erode(mask, mask, Mat(), Point(-1, -1), 2);
            dilate(mask, mask, Mat(), Point(-1, -1), 2);

            // 顔を黒塗りする
            cvtColor(frame, gray, COLOR_BGR2GRAY);
            vector<Rect> faces;
            face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(100, 100));

            for (auto &f : faces)
            {
                rectangle(mask, f, Scalar(0), FILLED);
            }

            // 輪郭抽出
            vector<vector<Point>> contours;
            findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

            // デバッグ表示
            Mat resized;
            resize(frame, resized, Size(300, 300));
            imshow("frame", resized);

            if (!contours.empty())
            {
                int idx = maxIndex(contours);
                if (maxArea(contours) < 1500) continue;

                drawContours(frame, contours, idx, Scalar(0,255,0), 2);

                int key = waitKey(1);
                if (key == 27) break;

                // -----------【ENTERでじゃんけん開始】-----------
                if (key == 13 || key == '\r')
                {
                    if (!paused && !ready)
                    {
                        cout << "じゃんけん" << endl;

                        auto start = steady_clock::now();
                        const int sample_count = 10;
                        s = 0;

                        while (steady_clock::now() - start < chrono::seconds(2))
                        {
                            cap >> frame;
                            if (frame.empty()) break;

                            // 中央ROI取得
                            Mat frame_center2 = frame(centerROI);

                            // HSV マスク（中央だけ）
                            Mat hsv_center2, mask_center2;
                            cvtColor(frame_center2, hsv_center2, COLOR_BGR2HSV);
                            inRange(hsv_center2, lower, upper, mask_center2);

                            // mask 全体へコピー
                            Mat mask2 = Mat::zeros(frame.size(), CV_8UC1);
                            mask_center2.copyTo(mask2(centerROI));

                            vector<vector<Point>> contours2;
                            findContours(mask2, contours2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

                            if (!contours2.empty())
                            {
                                int idx2 = maxIndex(contours2);
                                finger[s % sample_count] = fingercount1(contours2, idx2);
                            }
                            else finger[s % sample_count] = 0;

                            s++;
                            waitKey(1);
                        }

                        frefinger = frequentNum(finger, frequency);
                        fingerhandle(frefinger, &a);
                        jankenimg(a, &frame, 1);

                        cout << "あなたは" << (a==0?"グー":a==1?"チョキ":"パー") << "を出しています（ENTERキーを押してね）" << endl;

                        paused = true;
                        ready = true;
                    }
                    else if (paused && ready)
                    {
                        jankenpose(&b);
                        jankenimg(b, &frame, 0);
                        judge(a, b, &flag);

                        paused = false;
                        ready = false;
                        a = -1;
                    }
                }
            }
        }
    }
    return 0;
}


// ------------以下、あなたの元コード（変更なし）-----------------

int maxIndex(const vector<vector<Point>> &contours)
{
    int maxIndexvalue = 0;
    double maxArea = 0;
    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > maxArea)
        {
            maxArea = area;
            maxIndexvalue = i;
        }
    }
    return maxIndexvalue;
}

double maxArea(const vector<vector<Point>> &contours)
{
    double maxAreaValue = 0;
    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > maxAreaValue) maxAreaValue = area;
    }
    return maxAreaValue;
}

void jankenpose(int *a)
{
    double r = (double)rand() / RAND_MAX;
    cout << "相手は";
    if (r < 1 / 3.0) { *a = 0; cout << "グー"; }
    else if (r < 2 / 3.0) { *a = 1; cout << "チョキ"; }
    else { *a = 2; cout << "パー"; }
    cout << "を出しています" << endl;
}

void judge(int a, int b, int *flag)
{
    int t[3][3]={0};
    t[a][b] = 1;
    Mat img;

    if (t[0][1]||t[1][2]||t[2][0])
    {
        cout << "あなたの勝ちです" << endl;
        img = imread("you_win.png", IMREAD_UNCHANGED);
        *flag = 1;
    }
    else if (t[0][2]||t[1][0]||t[2][1])
    {
        cout << "あなたの負けです" << endl;
        img = imread("you_lose.png", IMREAD_UNCHANGED);
        *flag = 1;
    }
    else
    {
        cout << "あいこで（ENTERキーを押してね）" << endl;
        img = imread("Draw1.png", IMREAD_UNCHANGED);
        *flag = 0;
    }
    imshow("result", img);
}

int fingercount1(const vector<vector<Point>> &contours, int idx)
{
    vector<int> hull;
    convexHull(contours[idx], hull, false, false);

    int count = 0;
    if (hull.size() > 5)
    {
        vector<Vec4i> defects;
        convexityDefects(contours[idx], hull, defects);

        for (auto d : defects)
        {
            float depth = d[3] / 256.0;
            Point start = contours[idx][d[0]];
            Point end   = contours[idx][d[1]];
            Point far   = contours[idx][d[2]];

            if (depth > 90 && far.y > start.y && far.y > end.y)
                count++;
        }
        if (count > 5) count = 5;
        if (count >= 1) count++;
    }
    return count;
}

int frequentNum(int a[], int N)
{
    if (N <= 0) return 0;

    int maxcount = 0, index = 0;
    for (int i = 0; i < N; i++)
    {
        int cnt = 0;
        for (int k = 0; k < N; k++)
            if (a[i] == a[k]) cnt++;

        if (cnt > maxcount)
        {
            maxcount = cnt;
            index = i;
        }
    }
    return a[index];
}

void fingerhandle(int f, int *a)
{
    if (f >= 3) *a = 2;
    else if (f >= 2) *a = 1;
    else *a = 0;
}

void jankenimg(int a, Mat *frame, int gameTurn)
{
    Mat img, reimg;
    string result = (gameTurn ? "自分の手" : "敵の手");

    if (a == 0) img = imread("hand_gu-1.png", IMREAD_UNCHANGED);
    else if (a == 1) img = imread("hand_tyoki1.png", IMREAD_UNCHANGED);
    else img = imread("hand_pa-1.png", IMREAD_UNCHANGED);

    resize(img, reimg, Size(500, 500));
    imshow(result, reimg);
}
