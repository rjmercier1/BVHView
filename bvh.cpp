/**
***  BVH Player
***  Copyright (c) 2004-2017, Masaki OSHITA (www.oshita-lab.org)
***  Released under the MIT license http:
**/


#include <fstream>
#include <string.h>
#include <ImathQuat.h>
#include <ImathBox.h>
#include <math.h>
#include <OpenGL/gl.h>

#include "bvh.h"

BVH::BVH()
{
    motion = NULL;
    Clear();
}


BVH::BVH( const string& bvh_file_name )
{
    motion = NULL;
    Clear();

    Load( bvh_file_name );
}


BVH::~BVH()
{
    Clear();
}



void  BVH::Clear()
{
    for ( size_t i=0; i<channels.size(); i++ )
        delete  channels[ i ];
    for ( size_t i=0; i<joints.size(); i++ )
        delete  joints[ i ];
    if ( motion != NULL )
        delete  motion;

    is_load_success = false;

    file_name = "";
    motion_name = "";

    num_channel = 0;
    channels.clear();
    joints.clear();
    joint_index.clear();

    num_frame = 0;
    interval = 0.0;
    motion = NULL;
}

void  BVH::Load( const string& bvh_file_name )
{
    
    #define  BUFFER_LENGTH  1024*32

    ifstream  file;
    char      line[ BUFFER_LENGTH ];
    char *    token;
    char      separater[] = " :,\t\r";
    vector< Joint * >   joint_stack;
    Joint *   joint = NULL;
    Joint *   new_joint = NULL;
    bool      is_site = false;
    double    x, y ,z;
    int       lineno = 0;

    
    Clear();

    
    file_name = bvh_file_name;
    string base_filename = file_name.substr(file_name.find_last_of("/\\") + 1);
    string::size_type const p(base_filename.find_last_of('.'));
    motion_name = base_filename.substr(0, p);

    
    file.open( bvh_file_name, ios::in );
    if ( file.is_open() == 0 )  return; 

    
    while ( ! file.eof() )
    {
        
        if ( file.eof() )
            goto bvh_error;

        
        file.getline( line, BUFFER_LENGTH );
        token = strtok( line, separater );
        lineno += 1;

        
        if ( token == NULL )  continue;

        
        if ( strcmp( token, "{" ) == 0 )
        {
            
            joint_stack.push_back( joint );
            joint = new_joint;
            continue;
        }
        
        if ( strcmp( token, "}" ) == 0 )
        {
            
            joint = joint_stack.back();
            joint_stack.pop_back();
            is_site = false;
            continue;
        }

        
        if ( ( strcmp( token, "ROOT" ) == 0 ) ||
             ( strcmp( token, "JOINT" ) == 0 ) )
        {
            
            new_joint = new Joint();
            new_joint->index = joints.size();
            new_joint->parent = joint;
            new_joint->has_site = false;
            new_joint->offset = V3f(0);
            new_joint->site = V3f(0);
            joints.push_back( new_joint );
            if ( joint )
                joint->children.push_back( new_joint );

            
            token = strtok( NULL, "" );
            while ( *token == ' ' )  token ++;
            new_joint->name = token;

            
            joint_index[ new_joint->name ] = new_joint;
            continue;
        }

        
        if ( ( strcmp( token, "End" ) == 0 ) )
        {
            new_joint = joint;
            is_site = true;
            continue;
        }

        
        if ( strcmp( token, "OFFSET" ) == 0 )
        {
            
            token = strtok( NULL, separater );
            x = token ? atof( token ) : 0.0;
            token = strtok( NULL, separater );
            y = token ? atof( token ) : 0.0;
            token = strtok( NULL, separater );
            z = token ? atof( token ) : 0.0;

            
            if ( is_site )
            {
                joint->has_site = true;
                joint->site[0] = x;
                joint->site[1] = y;
                joint->site[2] = z;
            }
            else
            
            {
                joint->offset[0] = x;
                joint->offset[1] = y;
                joint->offset[2] = z;
            }
            continue;
        }

        
        if ( strcmp( token, "CHANNELS" ) == 0 )
        {
            
            token = strtok( NULL, separater );
            joint->channels.resize( token ? atoi( token ) : 0 );

            
            for ( size_t i=0; i<joint->channels.size(); i++ )
            {
                
                Channel *  channel = new Channel();
                channel->joint = joint;
                channel->index = channels.size();
                channels.push_back( channel );
                joint->channels[ i ] = channel;

                
                token = strtok( NULL, separater );
                if ( strcmp( token, "Xrotation" ) == 0 )
                    channel->type = X_ROTATION;
                else if ( strcmp( token, "Yrotation" ) == 0 )
                    channel->type = Y_ROTATION;
                else if ( strcmp( token, "Zrotation" ) == 0 )
                    channel->type = Z_ROTATION;
                else if ( strcmp( token, "Xposition" ) == 0 )
                    channel->type = X_POSITION;
                else if ( strcmp( token, "Yposition" ) == 0 )
                    channel->type = Y_POSITION;
                else if ( strcmp( token, "Zposition" ) == 0 )
                    channel->type = Z_POSITION;
            }
        }

        
        if ( strcmp( token, "MOTION" ) == 0 )
            break;
    }

    
    file.getline( line, BUFFER_LENGTH );
    token = strtok( line, separater );
    if ( strcmp( token, "Frames" ) != 0 )
        goto bvh_error;
    token = strtok( NULL, separater );
    if ( token == NULL )
        goto bvh_error;
    num_frame = atoi( token );

    file.getline( line, BUFFER_LENGTH );
    token = strtok( line, ":" );
    if ( strcmp( token, "Frame Time" ) != 0 )
        goto bvh_error;
    token = strtok( NULL, separater );
    if ( token == NULL )
        goto bvh_error;
    interval = atof( token );

    num_channel = channels.size();
    motion = new double[ num_frame * num_channel ];

    
    for ( int i=0; i<num_frame; i++ )
    {
        file.getline( line, BUFFER_LENGTH );
        token = strtok( line, separater );
        for ( int j=0; j<num_channel; j++ )
        {
            if ( token == NULL )
                goto bvh_error;
            motion[ i*num_channel + j ] = atof( token );
            token = strtok( NULL, separater );
        }
    }

    
    file.close();

    
    is_load_success = true;

    return;

bvh_error:
    file.close();
}


void  BVH::RenderFigure( int frame_no, float scale )
{
    Box3f bnds;
    for(size_t i = 0; i < joints.size(); i++) {
        bnds.extendBy(joints[i]->offset);
    }
    float dx = bnds.max.x - bnds.min.x;
    float dy = bnds.max.y - bnds.min.y;
    float dz = bnds.max.z - bnds.min.z;
    float mx = dx;
    if (dy > mx) mx = dy;
    if (dz > mx) mx = dz;

    
    RenderFigure( joints[ 0 ], motion + frame_no * num_channel, DualQuaternion(), 1.0/mx );
}

void  BVH::RenderFigure( const Joint * joint, const double * data, DualQuaternion xform, float scale )
{
    glPushMatrix();

    
    Imath::V3f translate;
    if ( joint->parent == NULL )
    {
        
        translate = scale * Imath::V3f( data[ 0 ], data[ 1 ], data[ 2 ] );
    }
    
    else
    {
        
        translate = scale * joint->offset;
    }

    
    Imath::Quatf q = Imath::Quatf::identity();
    for ( size_t i=0; i<joint->channels.size(); i++ )
    {
        Channel *  channel = joint->channels[ i ];
        if ( channel->type == X_ROTATION ) {
            
            q *= Imath::Quatf().setAxisAngle(Imath::V3f(1.0, 0.0, 0.0), M_PI * data[ channel->index ] / 180.0);
        } else if ( channel->type == Y_ROTATION ) {
            
            q *= Imath::Quatf().setAxisAngle(Imath::V3f(0.0, 1.0, 0.0), M_PI * data[ channel->index ] / 180.0);
        } else if ( channel->type == Z_ROTATION ) {
            
            q *= Imath::Quatf().setAxisAngle(Imath::V3f(0.0, 0.0, 1.0), M_PI * data[ channel->index ] / 180.0);
        }
    }

    DualQuaternion dq = DualQuaternion(q, translate) * xform;
    Imath::M44f m = dq.toMatrix();
    glMultMatrixf(&m[0][0]);

    
    
    if ( joint->children.size() == 0 )
    {
        RenderBone(V3f(0), scale * joint->site);
    }
    
    if ( joint->children.size() == 1 )
    {
        Joint *  child = joint->children[ 0 ];
        RenderBone(V3f(0), scale * child->offset);
    }
    
    if ( joint->children.size() > 1 )
    {
        
        V3f center = V3f(0);
        for ( size_t i=0; i<joint->children.size(); i++ )
        {
            Joint *  child = joint->children[ i ];
            center += child->offset;
        }
        center *= 1.0/(joint->children.size() + 1);

        
        RenderBone(V3f(0), scale * center);

        
        for ( size_t i=0; i<joint->children.size(); i++ )
        {
            Joint *  child = joint->children[ i ];
            RenderBone(scale * center, scale * child->offset);
        }
    }

    glPopMatrix();

    
    for ( size_t i=0; i<joint->children.size(); i++ )
    {
        RenderFigure( joint->children[ i ], data, dq, scale );
    }

}


void  BVH::RenderBone( V3f v0, V3f v1)
{
    glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(v0.x, v0.y, v0.z);
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(v1.x, v1.y, v1.z);
    glEnd();
}
