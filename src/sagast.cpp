#include "feature/agast.h"
#include <cv.h>
#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <vignetting_model/vignetting/vignettingtable.h>

using namespace cv;
using namespace std;

void
insertionSort( std::vector< cv::KeyPoint >& arr )
{
    // low response to high response

    int num = arr.size( );
    cv::KeyPoint temp;
    int i, j;

    for ( i = 1; i < num; i++ )
    {
        temp = arr[i];
        for ( j = i; j > 0 && arr[j - 1].response > temp.response; j-- )
        {
            arr[j] = arr[j - 1];
        }
        arr[j] = temp;
    }
}

int
main( )
{
    std::string file_vig
    = "/home/gao/ws/src/vins/config/dual/down_vignetting_model_calib.yaml";
    std::string file_cam  = "/home/gao/ws/src/vins/config/dual/cam_down_camera_calib.yaml";
    std::string file_img  = "/home/gao/ws/devel/lib/camera_model/image_down/IMG_280.png";
    std::string file_mask = "/home/gao/ws/src/vins/config/dual/mask_down.jpg";

    camera_model::VignettingTable* vignetting;
    vignetting = new camera_model::VignettingTable( file_vig, file_mask );

    Mat frame_in = imread( file_img, cv::IMREAD_GRAYSCALE );
    double sum_t = 0;
    double t     = getTickCount( );

    cv::namedWindow( "m_vignetting", cv::WINDOW_NORMAL );
    imshow( "src", frame_in );
    imshow( "m_vignetting", frame_in );

    Mat frame_color;
    cv::cvtColor( frame_in, frame_color, COLOR_GRAY2BGR );
    int resize_num = 1;

    Mat frame_fast, frame_fast_c;
    frame_in.copyTo( frame_fast );
    frame_color.copyTo( frame_fast_c );
    imshow( "frame_fast_c", frame_fast_c );

    std::vector< KeyPoint > keyPoints_1, keyPoints_2;
    cv::Ptr< cv::FastFeatureDetector > fast = cv::FastFeatureDetector::create( );
    fast->setThreshold( 40 );
    sum_t = 0;
    for ( int i = 0; i < 100; ++i )
    {
        t = getTickCount( );
        fast->detect( frame_fast, keyPoints_1 );
        t = ( ( double )getTickCount( ) - t ) / getTickFrequency( );
        sum_t += resize_num * t * 1000;
    }
    std::cout << "avg cost " << sum_t / 1000 << " ms" << std::endl;
    drawKeypoints( frame_fast_c, keyPoints_1, frame_fast_c, Scalar( 0, 0, 255 ), DrawMatchesFlags::DRAW_OVER_OUTIMG );
    std::cout << "Fast cost " << resize_num * t * 1000 << " ms" << std::endl;
    std::cout << "size " << keyPoints_1.size( ) << std::endl;

    cv::namedWindow( "FAST", cv::WINDOW_NORMAL );
    imshow( "FAST", frame_fast_c );

    keyPoints_1.clear( );

    Mat frame_agast16, frame_agast16_c;
    frame_in.copyTo( frame_agast16 );
    frame_color.copyTo( frame_agast16_c );

    cv::Ptr< cv::AgastFeatureDetector > agast16
    = cv::AgastFeatureDetector::create( 40, true, cv::AgastFeatureDetector::AGAST_7_12d );

    std::cout << "agast16 cost " << resize_num * t * 1000 << " ms" << std::endl;

    t = getTickCount( );
    agast16->detect( frame_agast16, keyPoints_1 );
    t = ( ( double )getTickCount( ) - t ) / getTickFrequency( );
    std::cout << resize_num * t * 1000 << std::endl;
    //        std::cout  << keyPoints_1.size( ) << std::endl;

    drawKeypoints( frame_agast16_c, keyPoints_1, frame_agast16_c, Scalar( 0, 0, 255 ), DrawMatchesFlags::DRAW_OVER_OUTIMG );
    std::cout << "size " << keyPoints_1.size( ) << std::endl;

    cv::namedWindow( "agast16", cv::WINDOW_NORMAL );
    imshow( "agast16", frame_agast16_c );

    keyPoints_1.clear( );
    keyPoints_2.clear( );
    Mat frame = vignetting->removeLUT( frame_in );
    Mat frame_agast12, frame_agast12_c;
    frame.copyTo( frame_agast12 ); // TODO
    frame_color.copyTo( frame_agast12_c );

    cv::Ptr< cv::AgastDetector > agast12
    = cv::AgastDetector::create( 40, true, cv::AgastDetector::SAGAST_12d );

    agast12->loadMask( file_mask );
    agast12->loadCamera( file_cam );

    sum_t = 0;
    for ( int i = 0; i < 1; ++i )
    {
        t = getTickCount( );
        if ( 0 )
        {
            agast12->buildOffsetsTable( );
            agast12->saveOffsetsTable( "/home/gao/ws/src/vins/config/dual/table" );
        }
        else
        {
            agast12->loadOffsetsTable( "/home/gao/ws/src/vins/config/dual/table" );
        }
        t = ( ( double )getTickCount( ) - t ) / getTickFrequency( );
        sum_t += resize_num * t * 1000;
    }
    std::cout << "avg cost " << sum_t / 1000 << " ms" << std::endl;
    std::cout << "agast12 table cost " << resize_num * t * 1000 << " ms" << std::endl;

    sum_t = 0;
    for ( int i = 0; i < 1; ++i )
    {
        t = getTickCount( );
        agast12->detect( frame_agast12, keyPoints_1 );
        t = ( ( double )getTickCount( ) - t ) / getTickFrequency( );
        sum_t += resize_num * t * 1000;
    }
    insertionSort( keyPoints_1 );
    for ( int pt_index = 0; pt_index < 100; ++pt_index )
    {
        keyPoints_2.push_back( keyPoints_1[pt_index] );
    }
    drawKeypoints( frame_agast12_c, keyPoints_1, frame_agast12_c, Scalar( 0, 0, 255 ), DrawMatchesFlags::DRAW_OVER_OUTIMG );
    std::cout << "avg cost " << sum_t / 1000 << " ms" << std::endl;
    std::cout << "agast12 cost " << resize_num * t * 1000 << " ms" << std::endl;
    std::cout << "size " << keyPoints_1.size( ) << std::endl;

    cv::namedWindow( "agast12", cv::WINDOW_NORMAL );
    imshow( "agast12", frame_agast12_c );

    //    keyPoints_1.clear( );

    // Mat frame_orb;
    // frame.copyTo( frame_orb );
    //
    // Mat descriptors_1, descriptors_2;
    // cv::Ptr< cv::ORB > orb = cv::ORB::create( );
    // orb->setMaxFeatures( 1000 );
    // orb->setFastThreshold( 0 );
    // t = getTickCount( );
    // orb->detect( frame_orb, keyPoints_1 );
    // t = ( ( double )getTickCount( ) - t ) / getTickFrequency( );
    // std::cout << "cost detect " << resize_num * t * 1000 << " ms" << std::endl;
    //
    // t = getTickCount( );
    // orb->compute( frame_orb, keyPoints_1, descriptors_1 );
    // t = ( ( double )getTickCount( ) - t ) / getTickFrequency( );
    // std::cout << "cost compute " << resize_num * t * 1000 << " ms" << std::endl;
    //
    // drawKeypoints( frame_orb, keyPoints_1, frame_orb, Scalar( 0, 0, 255 ),
    // DrawMatchesFlags::DRAW_OVER_OUTIMG );
    // //    std::cout << "cost " << t << "s" << std::endl;
    // std::cout << "size " << keyPoints_1.size( ) << std::endl;

    //    std::cout << "descriptors_1 " << descriptors_1 << std::endl;

    //    cv::namedWindow( "ORB", cv::WINDOW_NORMAL );
    //    imshow( "ORB", frame_orb );

    cvWaitKey( 0 );

    return 0;
}
