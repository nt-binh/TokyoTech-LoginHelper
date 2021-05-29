#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <chrono>

using namespace std;
using namespace cv;

double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

bool detectSlide(cv::Point pt1, cv::Point pt2){
    double dx = fabs(pt1.x - pt2.x);
    double dy = fabs(pt1.y - pt2.y);

    return ((dx <= 60) || (dx > 150)) && ((dy <= 60) || (dy > 150));
}

bool checkOrientation(std::vector<cv::Point> approx){
    for( int i = 0; i < 3; i++ )
    {
        for (int j = i + 1; j < 4; j++)
        if (!detectSlide(approx[i], approx[j])){
            return false;
        }
    }
    return true;
}

/*
bool checkRect(cv::Point pt1, cv::Point pt2, char coordinates){
    double edge1;
    if (coordinates == 'x')
        edge1 = pt1.x - pt2.x;
    if (coordinates == 'y')
        edge1 = pt1.y - pt2.y;
    return fabs(edge1) < 40;
}
*/

static void findSquares( const cv::Mat& image, std::vector<std::vector<Point> >& squares, float ang, std::vector<double>& area, int threshold_level)
{
    squares.clear();
    cv::Mat pyr, timg, gray0(image.size(), CV_8U), gray;
    // down-scale and upscale the image to filter out the noise
    cv::pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    cv::pyrUp(pyr, timg, image.size());
    std::vector<std::vector<cv::Point> > contours;
    
    // find squares in every color plane of the image
    //int threshold_level = 13;
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        cv::mixChannels(&timg, 1, &gray0, 1, ch, 1);
        // try several threshold levels
        for( int l = 0; l < threshold_level; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l > 0 && l < 5)
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                cv::Canny(gray0, gray, 0, 20, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                cv::dilate(gray, gray, cv::Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                // tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/threshold_level;
            }
            // find contours and store them all as a list
            cv::findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
            std::vector<cv::Point> approx;
            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                cv::approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.02, true);
                if( approx.size() == 4 &&
                    fabs(cv::contourArea(approx)) > 2.5e+05 &&
                    fabs(cv::contourArea(approx)) < 6.6e+05 && 
                    cv::isContourConvex(approx) )
                {   
                    double maxCosine = 0;
                    for( int j = 2; j < 6; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[(j-1) % 4]));
                        maxCosine = MAX(maxCosine, cosine);
                    }
                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence

                    // if( maxCosine < ang && detectSlide(approx[0], approx[1]))
                    if( maxCosine < ang && checkOrientation(approx))
                    {
                        squares.push_back(approx);
                        area.push_back(fabs(cv::contourArea(approx)));
                    }
                }
            }
        }
    }
}

static void findCircle(const cv::Mat& image, std::vector<Vec3f>& circles){
    cv::Mat gray;
    cv::cvtColor(image, gray, COLOR_BGR2GRAY);
    cv::medianBlur(gray, gray, 5);
    cv::HoughCircles(gray, circles, HOUGH_GRADIENT, 1,
                 gray.rows/16,
                 100, 30, 100, 300
    );
}

cv::Mat imgSegmentation(cv::Mat src){
    // ------------------------- IMAGE SEGMENTATION ------------------------ //
    // Show the original image
    //imshow("Source Image", src);
    // Change the background from white to black 
    // extract better results during the use of Distance Transform
    for ( int i = 0; i < src.rows; i++ ) {
        for ( int j = 0; j < src.cols; j++ ) {
            if ( src.at<Vec3b>(i, j) == Vec3b(255,255,255) )
            {
                src.at<Vec3b>(i, j)[0] = 0;
                src.at<Vec3b>(i, j)[1] = 0;
                src.at<Vec3b>(i, j)[2] = 0;
            }
        }
    }
    // Show image with black background
    //imshow("Black Background Image", src);
    // Sharpen the image
    //cv::Mat kernel = (cv::Mat_<float>(3,3) <<
    //              1,  1, 1,
    //              1, -8, 1,
    //              1,  1, 1);
    cv::Mat kernel = (cv::Mat_<float>(5,5) <<
                  1,  1,   1,  1, 1,
                  1,  1, -14,  1, 1,
                  1,  1,   1,  1, 1); 
    // Laplacian filtering
    cv::Mat imgLaplacian;
    cv::filter2D(src, imgLaplacian, CV_32F, kernel);
    cv::Mat sharp;
    src.convertTo(sharp, CV_32F);
    cv::Mat imgResult = sharp - imgLaplacian;
    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8UC3);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
    // Show the laplace filtered image
    //imshow( "Laplace Filtered Image", imgLaplacian );
    // Show sharpened image
    //imshow( "New Sharped Image", imgResult );
    // Create binary image from original image
    cv::Mat bw;
    cv::cvtColor(imgResult, bw, COLOR_BGR2GRAY);
    cv::threshold(bw, bw, 100, 255, THRESH_BINARY | THRESH_OTSU);
    //imshow("Binary Image", bw);

    // Perform the distance transform algorithm
    cv::Mat dist;
    cv::distanceTransform(bw, dist, DIST_L1, 5);
    // Normalize the distance image for range = {0.0, 1.0}
    // then visualize and threshold
    cv::normalize(dist, dist, 0, 1.0, NORM_MINMAX);
    //imshow("Distance Transform Image", dist);
    
    // Threshold to obtain the peaks
    // Create markers for the foreground objects
    cv::threshold(dist, dist, 0.4, 1.0, THRESH_BINARY);
    // Dilate distanced image
    cv::Mat kernel1 = cv::Mat::ones(3, 3, CV_8U);
    cv::dilate(dist, dist, kernel1);
    //imshow("Peaks", dist);
    
    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    cv::Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    
    // Find total markers
    std::vector<std::vector<Point> > contours;
    cv::findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    
    // Create the marker image for the watershed algorithm
    cv::Mat markers = cv::Mat::zeros(dist.size(), CV_32S);
    
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++)
    {
        drawContours(markers, contours, static_cast<int>(i), Scalar(static_cast<int>(i)+1), -1);
    }
    
    // Draw the background marker
    cv::circle(markers, Point(5,5), 3, Scalar(255), -1);
    //imshow("Markers", markers*10);
    
    // Perform the watershed algorithm
    cv::watershed(imgResult, markers);
    cv::Mat mark;
    markers.convertTo(mark, CV_8U);
    cv::bitwise_not(mark, mark);
    //imshow("Markers_v2", mark); 
    
    // Generate random colors
    std::vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = theRNG().uniform(0, 256);
        int g = theRNG().uniform(0, 256);
        int r = theRNG().uniform(0, 256);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }

    // Create the result image
    cv::Mat dst = cv::Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled objects with random colors
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
            if (index > 0 && index <= static_cast<int>(contours.size()))
            {
                dst.at<Vec3b>(i,j) = colors[index-1];
            }
        }
    }

    // Visualize the final image
    // imshow("Final Result", dst);
    // cv::imwrite("dst.png", dst);
    return dst;
}

cv::Mat claheTransform(cv::Mat bgr_image){
    cv::Mat lab_image;
    cv::cvtColor(bgr_image, lab_image, cv::COLOR_BGR2Lab);
    
    // Extract the L channel from the lab image
    std::vector<cv::Mat> lab_planes(3);

    // L image in lab_planes[0]
    cv::split(lab_image, lab_planes);  

    // apply the CLAHE algorithm to the L channel
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(2);
    cv::Mat dst;
    clahe->apply(lab_planes[0], dst);

    // Merge the the color planes back into an Lab image
    dst.copyTo(lab_planes[0]);
    cv::merge(lab_planes, lab_image);

   // Convert back to RGB
   cv::Mat image_clahe;
   cv::cvtColor(lab_image, image_clahe, cv::COLOR_Lab2BGR);
   return image_clahe;
}

int main(int argc, char *argv[])
{
    auto start = chrono::high_resolution_clock::now(); 
    // Load the image
    cv::Mat src = imread(argv[1]);
    if( src.empty() )
    {
        std::cout << "Could not open or find the image!\n" << std::endl;
        return -1;
    }

    // resize original image to 1/4 size
    cv::resize(src, src, cv::Size(), 0.25, 0.25);
    std::cout << "Image area is: " << src.cols * src.rows << std::endl;
    std::cout << "Image cols: " << src.cols << " rows: " << src.rows << std::endl;
    cv::Mat sub = src.clone();
    cv::imshow("Source", src);

    //--------------------------------------- CLAHE TRANSFORM ----------------------------------------
    src = claheTransform(src);
    cv::imshow("CLAHE Transform", src);

    int threshold_level[2] = {13, 30};
    std::vector<double> thresholdSquaresArea;
    std::vector<std::vector<Point>> thresholdSquares;
    for (int i = 0; i < 2; i++){
        // Find squares without using Image Segmentation
        std::vector<std::vector<cv::Point>> squares;    //std::vector of std::vector of points of squares
        std::vector<double> squaresArea;
        findSquares(sub, squares, 0.1, squaresArea, threshold_level[i]);
        
        // Find squares using Image Segmentation
        cv::Mat dst = imgSegmentation(src);
        std::vector<std::vector<cv::Point>> Watershed;    //std::vector of std::vector of points of squares
        std::vector<double> WatershedArea;
        findSquares(dst, Watershed, 0.13, WatershedArea, threshold_level[i]);          

        // ----------------------------------- DRAW MULTIPLE SQUARES --------------------------------
        // cv::polylines(src, Watershed, true, Scalar(255, 255, 0), 1, LINE_AA);
        // cv::polylines(src, squares, true, Scalar(0, 255, 0), 1, LINE_AA);
        // for (int j = 0; j < squares.size(); j++){
        //     std::cout << squares[j] << " -- " << squaresArea[j] << std::endl;
        //     circle(src, squares[j][1], 1, Scalar(255, 255, 0), 10, 8, 0);
        // }
        // for (int k = 0; k < Watershed.size(); k++){
        //     std::cout << "This is watershed" << std::endl;
        //     std::cout << Watershed[k] << " -- " << WatershedArea[k] << std::endl;
        //     circle(src, Watershed[k][1], 1, Scalar(0, 0, 255), 10, 8, 0);
        // }

        // ------------------------------------ FIND VALID SQUARES ----------------------------------
        std::cout << "Watershed squares no: " << Watershed.size() << std::endl;
        std::cout << "Original method squares no: " << squares.size() << std::endl;
        int minSquaresArea = std::distance(squaresArea.begin(), min_element(squaresArea.begin(), squaresArea.end()));    //smallest element's index in squares std::vector (w/o image segmentation)
        if (Watershed.size() == 0 && squares.size() == 0){
            continue;
        }
        if (Watershed.size() == 0){
            //cv::polylines(src, squares[minSquaresArea], true, Scalar(0, 0, 255), 3, LINE_AA);
            thresholdSquares.push_back(squares[minSquaresArea]);
            thresholdSquaresArea.push_back(contourArea(squares[minSquaresArea]));
        }
        else{
            int val = -1;
            int minWatershedArea = std::distance(WatershedArea.begin(), min_element(WatershedArea.begin(), WatershedArea.end()));
            //std::cout << minWatershedArea << std::endl;
            for (int i = 0; i < squares.size(); i++){
                double ratio = squaresArea[i] / WatershedArea[minWatershedArea];
                if ( (ratio < 1.2 && ratio > 0.8))
                    val = i;
            }
            std::cout << "Val: " << val << std::endl;
            if (val == -1){
                //cv::polylines(src, Watershed[minWatershedArea], true, Scalar(0, 0, 255), 4, LINE_AA);
                thresholdSquares.push_back(Watershed[minWatershedArea]);
                thresholdSquaresArea.push_back(contourArea(Watershed[minWatershedArea]));
            }else
            {
                //cv::polylines(src, squares[val], true, Scalar(0, 0, 255), 4, LINE_AA);
                thresholdSquares.push_back(squares[val]);
                thresholdSquaresArea.push_back(contourArea(squares[val]));
            }
        }
    }
    int index = std::distance(thresholdSquaresArea.begin(), max_element(thresholdSquaresArea.begin(), thresholdSquaresArea.end()));
    
    //////////////////////////////////// DRAW VALID SQUARE ////////////////////////////////
    cv::polylines(src, thresholdSquares[index], true, Scalar(0, 0, 255), 4, LINE_AA);
    int topy, topx, width, height;

    //////////////////////////////////// GET VERTICES OF ROI FOR CROPPING IMAGE //////////////////////////
    if (fabs(thresholdSquares[index][0].x - thresholdSquares[index][1].x) > 50){
        topy = thresholdSquares[index][1].y;
        topx = thresholdSquares[index][1].x > thresholdSquares[index][2].x ? thresholdSquares[index][2].x : thresholdSquares[index][1].x;
        width = fabs(thresholdSquares[index][3].x - topx);
        height = fabs(thresholdSquares[index][3].y - topy);
    } else{
        topy = thresholdSquares[index][0].y;
        topx = thresholdSquares[index][0].x > thresholdSquares[index][1].x ? thresholdSquares[index][1].x : thresholdSquares[index][0].x;
        width = fabs(thresholdSquares[index][2].x - topx);
        height = fabs(thresholdSquares[index][2].y - topy);
    }

    /*
    //////////////////////////////////// DETECT CIRCLE ///////////////////////////////////////////////////
    std::vector<Vec3f> circles;
    findCircle(src, circles);
    std::cout << "Circle sizes: " << circles.size() << std::endl;
    int minRadius = 90000;
    Vec3i c;
    Point center;
    for( size_t i = 0; i < circles.size(); i++ )
    {
        c = circles[i];
        center = Point(c[0], c[1]);
        //circle( src, center, 1, Scalar(0,100,100), 3, LINE_AA);
        //minRadius = c[2];
        //std::cout << "Radius " << minRadius << std::endl;
        //std::cout << "Center " << c[0] << " " << c[1] << std::endl;
        //circle( src, center, minRadius, Scalar(255,0,255), 3, LINE_AA);
        if (fabs(c[0] - (src.cols/2)) < 100 && fabs(c[1] - (src.rows/2)) < 100 && c[2] > 120 && c[2] < 170){
            if (c[2] < minRadius){
                // circle outline
                minRadius = c[2];
            }
        }
    }
    circle( src, center, 1, Scalar(0,100,100), 3, LINE_AA);
    std::cout << "Radius " << minRadius << std::endl;
    std::cout << "Center " << c[0] << " " << c[1] << std::endl;
    circle( src, center, minRadius, Scalar(255,0,255), 3, LINE_AA);
    */

   /////////////////////////////////////////// CHECK BRIGHTNESS //////////////////////////////////
    cv::Mat hsvImg;
    cv::cvtColor(src, hsvImg, cv::COLOR_BGR2HSV);
    int meanV = 0;
    for (size_t i = 0; i < src.cols; i++){
        for (size_t j = 0; j < src.rows; j++){
            Vec3b hsv = hsvImg.at<Vec3b>(i, j);
            meanV += hsv.val[2];
        }
    }
    meanV /= src.rows*src.cols;
    std::cout << "V value: " << meanV << std::endl;
    imshow("Final", src);

    ///////////////////////////////////////// BRIGHTEN DARK IMAGES //////////////////////////////////
    if (meanV < 124){
        cv::Mat sub = src.clone();
        
        /////////////////////// BRIGHTEN + CLAHE TRANSFORM

        //cv::Mat circleROI = sub(Rect(src.cols/2 - 150, src.rows/2 - 150, src.cols * 0.5, src.rows * 0.4));
        //imshow("Circle ROI", circleROI);
        cv::Rect ROI(topx, topy, width, height);
        sub = sub(ROI);
        std::cout << thresholdSquares[index] << std::endl;
        // Show cropped image
        topy = sub.rows * 0.05;
        topx = sub.cols * 0.05;
        width = sub.cols * 0.95;
        height = sub.rows * 0.95;
        cv::Rect cropRect(topx, topy, width, height);
        cv::Mat cropImg = sub(cropRect);
        cropImg.convertTo(cropImg, -1, 1, 70);
        imshow("Brightened", cropImg);
        //--------------------------------------- CLAHE TRANSFORM ----------------------------------------
        //cropImg = claheTransform(cropImg);
        //cv::imshow("CLAHE Transform + Brighten", cropImg);
    }
    auto end = chrono::high_resolution_clock::now();
    double time_taken =  chrono::duration_cast<chrono::milliseconds>(end - start).count(); 
    std::cout << "Time taken by program is : " << time_taken * 1e-3 << " s" << std::endl;  
    waitKey(0);
	return 0;
}