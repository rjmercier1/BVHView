#ifndef BVH_H
#define BVH_H

#include <vector>
#include <map>
#include <string>
#include <ImathQuat.h>
#include "dualquaternion.h"

using namespace  std;

//
//  BVH
//
class  BVH
{
public:

    enum  ChannelEnum
    {
        X_ROTATION, Y_ROTATION, Z_ROTATION,
        X_POSITION, Y_POSITION, Z_POSITION
    };
    struct  Joint;

    struct  Channel
    {
        Joint *              joint;
        ChannelEnum          type;
        int                  index;
    };

    struct  Joint
    {
        string               name;
        int                  index;
        Joint *              parent;
        vector< Joint * >    children;
        V3f                  offset;
        bool                 has_site;
        V3f                  site;
        vector< Channel * >  channels;
    };


private:
    bool                     is_load_success;

    string                   file_name;
    string                   motion_name;

    int                      num_channel;
    vector< Channel * >      channels;
    vector< Joint * >        joints;
    map< string, Joint * >   joint_index;

    int                      num_frame;
    double                   interval;
    double *                 motion;


public:

    BVH();
    BVH( const string& bvh_file_name );
    ~BVH();


    void  Clear();


    void  Load( const string& bvh_file_name );

public:

    bool  IsLoadSuccess() const { return is_load_success; }


    const string &  GetFileName() const { return file_name; }
    const string &  GetMotionName() const { return motion_name; }


    int             GetNumJoint() const { return  joints.size(); }
    const Joint *   GetJoint( int no ) const { return  joints[no]; }
    int             GetNumChannel() const { return  channels.size(); }
    const Channel * GetChannel( int no ) const { return  channels[no]; }

    const Joint *   GetJoint( const string & j ) const  {
        map< string, Joint * >::const_iterator  i = joint_index.find( j );
        return  ( i != joint_index.end() ) ? (*i).second : NULL; }
    const Joint *   GetJoint( const char * j ) const  {
        map< string, Joint * >::const_iterator  i = joint_index.find( j );
        return  ( i != joint_index.end() ) ? (*i).second : NULL; }


    int     GetNumFrame() const { return  num_frame; }
    double  GetInterval() const { return  interval; }
    double  GetMotion( int f, int c ) const { return  motion[ f*num_channel + c ]; }


    void  SetMotion( int f, int c, double v ) { motion[ f*num_channel + c ] = v; }

public:
    void  RenderFigure( int frame_no, float scale = 1.0f );
    static void  RenderFigure( const Joint * root, const double * data, DualQuaternion, float scale = 1.0f );
    static void  RenderBone( V3f, V3f );
};


#endif // BVH_H
