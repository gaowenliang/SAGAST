
#include "../agast.h"
#include "../agast_score.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;

void
AgastDetector::getAgastOffsets( int pixel[16], int rowStride, int type )
{
    static const int offsets16[][2] = { { -3, 0 },  //
                                        { -3, -1 }, //
                                        { -2, -2 }, //
                                        { -1, -3 }, //
                                        { 0, -3 },  //
                                        { 1, -3 },  //
                                        { 2, -2 },  //
                                        { 3, -1 },  //
                                        { 3, 0 },   //
                                        { 3, 1 },   //
                                        { 2, 2 },   //
                                        { 1, 3 },   //
                                        { 0, 3 },   //
                                        { -1, 3 },  //
                                        { -2, 2 },  //
                                        { -3, 1 } };

    static const int offsets12d[][2] = { { -3, 0 },  //
                                         { -2, -1 }, //
                                         { -1, -2 }, //
                                         { 0, -3 },  //
                                         { 1, -2 },  //
                                         { 2, -1 },  //
                                         { 3, 0 },   //
                                         { 2, 1 },   //
                                         { 1, 2 },   //
                                         { 0, 3 },   //
                                         { -1, 2 },  //
                                         { -2, 1 } };

    static const int offsets12s[][2] = { { -2, 0 },  //
                                         { -2, -1 }, //
                                         { -1, -2 }, //
                                         { 0, -2 },  //
                                         { 1, -2 },  //
                                         { 2, -1 },  //
                                         { 2, 0 },   //
                                         { 2, 1 },   //
                                         { 1, 2 },   //
                                         { 0, 2 },   //
                                         { -1, 2 },  //
                                         { -2, 1 } };

    static const int offsets8[][2] = { { -1, 0 },  //
                                       { -1, -1 }, //
                                       { 0, -1 },  //
                                       { 1, -1 },  //
                                       { 1, 0 },   //
                                       { 1, 1 },   //
                                       { 0, 1 },   //
                                       { -1, 1 } };

    const int( *offsets )[2] = type == AgastDetector::OAST_9_16 ?
                               offsets16 :
                               type == AgastDetector::AGAST_7_12d ?
                               offsets12d :
                               type == AgastDetector::AGAST_7_12s ?
                               offsets12s :
                               type == AgastDetector::AGAST_5_8 ? offsets8 : 0;

    const int offsets_len
    = type == AgastDetector::OAST_9_16 ?
      16 :
      type == AgastDetector::AGAST_7_12d ?
      12 :
      type == AgastDetector::AGAST_7_12s ? 12 : type == AgastDetector::AGAST_5_8 ? 8 : 0;

    CV_Assert( pixel && offsets );

    int k = 0;
    for ( ; k < offsets_len; k++ )
        pixel[k] = offsets[k][0] + offsets[k][1] * rowStride;
}

int
AgastDetector::double2int( double a )
{
    if ( 0 )
    {
        int abs_a = round( abs( a ) );

        return a > 0 ? abs_a : ( -abs_a );
    }

    if ( 1 )
    {
        double abs_a = abs( a );
        int int_a    = int( abs_a );
        if ( abs_a - int_a >= 0.3 )
            return a > 0 ? ( abs_a + 1 ) : ( -( abs_a + 1 ) );
        else
            return a > 0 ? int_a : ( -int_a );
    }
}

Eigen::Vector2d
AgastDetector::calcAngle( const camera_model::CameraPtr cam, const Eigen::Vector2d pu )
{
    Eigen::Vector3d P;
    cam->liftSphere( pu, P );

    double theta = acos( P( 2 ) / P.norm( ) );
    double phi   = atan2( P( 1 ), P( 0 ) );

    Eigen::Vector2d angle( theta, phi );

    return angle;
}

void
AgastDetector::buildAgastOffsetsTable( )
{
    switch ( getType( ) )
    {
        case AgastDetector::AGAST_7_12d:
        {
            buildAgastOffsetsTable_AGAST_7_12d( );
            break;
        }
    }
}

void
AgastDetector::getAgastOffsets( short pixel[16], short rowStride, int type, int xx, int yy )
{
    switch ( type )
    {
        case AgastDetector::AGAST_7_12d:
        {
            getAgastOffsets_AGAST_7_12d( pixel, rowStride, xx, yy );
            break;
        }
    }
}

bool
AgastDetector::saveAgastOffsetsTable( std::string path )
{
    bool done;
    switch ( getType( ) )
    {
        case AgastDetector::AGAST_7_12d:
        {
            done = saveAgastOffsetsTable_AGAST_7_12d( path );
            break;
        }
    }

    return done;
}

bool
AgastDetector::loadAgastOffsetsTable( std::string path )
{
    bool done;
    switch ( getType( ) )
    {
        case AgastDetector::AGAST_7_12d:
        {
            done = loadAgastOffsetsTable_AGAST_7_12d( path );
            break;
        }
    }

    return done;
}
