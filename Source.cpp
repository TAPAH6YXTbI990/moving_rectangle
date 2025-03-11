#include <opencv2/opencv.hpp>
#include <iostream>
#include <locale>
#include <codecvt>

using namespace cv;
using namespace std;

string shapeFinder(const vector<Point>& contour)
{
    vector<Point> approx;
    double epsilon = 0.01 * arcLength(contour, true);
    approxPolyDP(contour, approx, epsilon, true);
    
    if (approx.size() == 4)
    {
        Rect rect = boundingRect(approx);
        return "rectangle";
    }
    return "???";
}

int main()
{
   
    VideoCapture cap("C:\\Users\\User\\.vscode\\projects\\movingRect.mp4");

    if (!cap.isOpened())
    {
        cerr << "Error when uploading the video!" << endl;
        return -1; 
    }
    
    Mat frame, gray, binary;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            cerr << "Error: empty frame!" << endl;
            break;
        }
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        threshold(gray, binary, 100, 150, THRESH_BINARY);

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(binary, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

        Mat contourOutput = frame.clone();

        for (size_t i = 0; i < contours.size(); i++)
        {
            double area = contourArea(contours[i]);
            if (area < 100)
                continue;

            string shape = shapeFinder(contours[i]);
            if (shape == "rectangle") {
                if (hierarchy[i][2] != -1)
                {
                    continue;
                }

                drawContours(contourOutput, contours, i, Scalar(255, 0, 0), 2);

                Moments M = moments(contours[i]);
                Point center(M.m10 / M.m00, M.m01 / M.m00);
                putText(contourOutput, shape, center, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2);
            }
        }
        imshow("Result", contourOutput);
        
        if (waitKey(30) == 'q') break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}
