#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>

// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <unistd.h>
using namespace cv;
using namespace std;
using namespace chrono;

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
    int maxIndex = 0;
    double maxAreavalue = 0;
    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > maxAreavalue)
        {
            maxAreavalue = area;
            maxIndex = i;
        }
    }
    return maxAreavalue;
}

void jankenpose(int *a)
{
    double random = (double)rand() / RAND_MAX;
    cout << "相手は";
    if (random < 1 / 3.0)
    {
        *a = 0;
        cout << "グー";
    }
    else if (random >= 1 / 3.0 && random < 2 / 3.0)
    {
        *a = 1;
        cout << "チョキ";
    }
    else
    {
        *a = 2;
        cout << "パー";
    }
    cout << "を出しています" << endl;
}
void judge(int a, int b, int *flag)
{
    int janken[3][3];
    Mat img;
    for (int i = 0; i < 3; i++)
    {
        for (int k = 0; k < 3; k++)
        {
            janken[i][k] = 0;
        }
    }
    // cout << a << b << endl;
    janken[a][b] = 1;
    if (janken[0][1] == 1 || janken[1][2] == 1 || janken[2][0] == 1)
    {

        cout << "あなたの勝ちです" << endl;
        img = imread("you_win.png", IMREAD_UNCHANGED);
       
        cout << "enterキーを押して再戦" << endl;
        *flag = 1;
    }
    else if (janken[0][2] == 1 || janken[1][0] == 1 || janken[2][1] == 1)
    {

        cout << "あなたの負けです" << endl;
        img = imread("you_lose.png", IMREAD_UNCHANGED);

        cout << "enterキーを押して再戦" << endl;
        *flag = 1;
    }
    else
    {

        cout << "あいこで（ENTERキーを押してね）" << endl;
        img = imread("Draw1.png",IMREAD_UNCHANGED);
        *flag = 0;
    }
     imshow("result", img);
}

int fingercount1(const vector<vector<Point>> &contours, int maxIndex)
{
    vector<int> hull;
    convexHull(contours[maxIndex], hull, false, false);
    int fingercount = 0;

    if (hull.size() > 5)
    {

        vector<Vec4i> defects;
        convexityDefects(contours[maxIndex], hull, defects);

        for (auto d : defects)
        { // この分はdefectsの中から順番に取り出す

            Point start = contours[maxIndex][d[0]];
            Point end = contours[maxIndex][d[1]];
            Point far = contours[maxIndex][d[2]];

            float depth = d[3] / 256.0;
            // cout << depth << endl;
            if (depth > 90 && far.y > start.y && far.y > end.y)
            {

                fingercount++;
                // finger[i%25] = fingercount;
            }
            // if(depth > 100 && depth < 105 && fingercount >0) fingercount--;
        }
        // cout << depth <<endl;
        if (fingercount > 5)
            fingercount = 5;
        if (fingercount >= 1)
            fingercount++;

        // cout << fingercount <<endl;
    }
    // cout << fingercount << endl;
    return fingercount;
}
void fingercalc(const vector<vector<Point>> &contours, int maxIndex)
{
    vector<int> hull;
    convexHull(contours[maxIndex], hull, false, false);

    if (hull.size() > 5)
    {

        vector<Vec4i> defects;
        convexityDefects(contours[maxIndex], hull, defects);

        int fingercount = 0;

        for (auto d : defects)
        { // この分はdefectsの中から順番に取り出す

            Point start = contours[maxIndex][d[0]];
            Point end = contours[maxIndex][d[1]];
            Point far = contours[maxIndex][d[2]];

            float depth = d[3] / 256.0;
            // cout << depth << endl;
            if (depth > 100 && far.y > start.y && far.y > end.y)
            {

                fingercount++;
                // finger[i%25] = fingercount;
            }
            if (depth > 100 && depth < 160)
                fingercount--;
        }
        // cout << depth <<endl;
        // cout << fingercount << endl;
        fingercount = 0;
    }
}
int frequentNum(int a[], int N)
{
    if (N <= 0)
        return 0;
    int maxcount = 0;
    int t = 0;
    for (int i = 0; i < N; i++)
    {
        int count = 0;
        for (int k = 0; k < N; k++)
        {
            if (a[i] == a[k])
            {
                count++;
            }
            if (count > maxcount)
            {
                maxcount = count;
                t = i;
            }
        }
    }
    return a[t];
}
void fingerhandle(int finger, int *a)
{
    // cout << finger << endl;
    if (finger >= 3)
        *a = 2;
    else if (finger >= 2 && finger < 3)
        *a = 1;
    else if (finger < 2)
        *a = 0;
}

void jankenimg(int a, Mat *frame, int gameTurn)
{
    Mat img, reimg;
    // Mat img = Mat::zeros(400, 600, CV_8UC3);
    // img.setTo(Scalar(255, 255, 255));
    string result;
    if (gameTurn)
        result = "自分の手";
    else
        result = "敵の手";
    if (a == 0)
    {
        img = imread("hand_gu-1.png", IMREAD_UNCHANGED);
    }
    else if (a == 1)
    {
        img = imread("hand_tyoki1.png", IMREAD_UNCHANGED);
    }
    else
    {
        img = imread("hand_pa-1.png", IMREAD_UNCHANGED);
    }
    resize(img, reimg, Size(500, 500));
    imshow(result, reimg);
    // waitKey(0);
}

void jankenposeseen(vector<vector<Point>> contours, int maxIndex(vector<vector<Point>> contours), int frequency, int *s, int finger[])
{
    int frefinger;
    int a = 0;
    finger[*s % 10] = fingercount1(contours, maxIndex(contours));
    if (*s >= 10)
        frefinger = frequentNum(finger, frequency);
    fingerhandle(fingercount1(contours, maxIndex(contours)), &a);
    fingerhandle(frefinger, &a);
    cout << "あなたは" << (a == 0 ? "グー" : a == 1 ? "チョキ"
                                                    : "パー")
         << "を出しています" << endl;
    cout << frefinger << endl;
}