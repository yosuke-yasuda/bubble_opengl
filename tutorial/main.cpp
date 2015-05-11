/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

// Basket ball demo.
// Serves as a test for the sphere vs trimesh collider
// By Bram Stolk.
// Press the spacebar to reset the position of the ball.

#include <assert.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include "texturepath.h"
#include "basket_geom.h" // this is our world mesh

#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

// some constants

#define RADIUS 0.14

// dynamics and collision objects (chassis, 3 wheels, environment)

static dWorldID world;
static dSpaceID space;

static dBodyID sphbody[4];
static dGeomID sphgeom[4];

static dJointGroupID contactgroup;
static dGeomID world_mesh;


// this is called by dSpaceCollide when two objects in space are
// potentially colliding.

static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    assert(o1);
    assert(o2);
    
    if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
    {
        fprintf(stderr,"testing space %p %p\n", (void*)o1, (void*)o2);
        // colliding a space with something
        dSpaceCollide2(o1,o2,data,&nearCallback);
        // Note we do not want to test intersections within a space,
        // only between spaces.
        return;
    }
    
    //  fprintf(stderr,"testing geoms %p %p\n", o1, o2);
    
    const int N = 32;
    dContact contact[N];
    int n = dCollide (o1,o2,N,&(contact[0].geom),sizeof(dContact));
    if (n > 0)
    {
        for (int i=0; i<n; i++)
        {
            // Paranoia  <-- not working for some people, temporarily removed for 0.6
            //dIASSERT(dVALIDVEC3(contact[i].geom.pos));
            //dIASSERT(dVALIDVEC3(contact[i].geom.normal));
            //dIASSERT(!dIsNan(contact[i].geom.depth));
            contact[i].surface.slip1 = 0.7;
            contact[i].surface.slip2 = 0.7;
            contact[i].surface.mode = dContactSoftERP | dContactSoftCFM | dContactApprox1 | dContactSlip1 | dContactSlip2;
            contact[i].surface.mu = 50.0; // was: dInfinity
            contact[i].surface.soft_erp = 0.96;
            contact[i].surface.soft_cfm = 0.04;
            dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
            dJointAttach (c,
                          dGeomGetBody(contact[i].geom.g1),
                          dGeomGetBody(contact[i].geom.g2));
        }
    }
}


// start simulation - set viewpoint

static void start()
{
    dAllocateODEDataForThread(dAllocateMaskAll);
    
    static float xyz[3] = {-8,0,5};
    static float hpr[3] = {0.0f,-29.5000f,0.0000f};
    dsSetViewpoint (xyz,hpr);
}



static void reset_ball(void)
{
    float sx=0.0f, sy=3.40f, sz=7.15;
    
    dQuaternion q;
    dQSetIdentity(q);
    for(int i=0;i<sizeof(sphbody);i++){
        dBodySetPosition (sphbody[i], sx, sy, sz+1.0f*(i));
        dBodySetQuaternion(sphbody[i], q);
        dBodySetLinearVel (sphbody[i], 0,0,0);
        dBodySetAngularVel (sphbody[i], 0,0,0);
     }
}


// called when a key pressed

static void command (int cmd)
{
    switch (cmd)
    {
        case ' ':
            reset_ball();
            break;
        case 'a':
            reset_ball();
            break;
    }
}


// simulation loop

static void simLoop (int pause)
{
    double simstep = 0.001; // 1ms simulation steps
    double dt = dsElapsedTime();
    
    int nrofsteps = (int) ceilf(dt/simstep);
    //  fprintf(stderr, "dt=%f, nr of steps = %d\n", dt, nrofsteps);
    
    for (int i=0; i<nrofsteps && !pause; i++)
    {
        dSpaceCollide (space,0,&nearCallback);
        dWorldQuickStep (world, simstep);
        dJointGroupEmpty (contactgroup);
    }
    
    dsSetColor (1,1,1);
    const dReal *SPos[] = {dBodyGetPosition(sphbody[0]), dBodyGetPosition(sphbody[1]), dBodyGetPosition(sphbody[2]), dBodyGetPosition(sphbody[3])};
    const dReal *SRot[] = {dBodyGetRotation(sphbody[0]), dBodyGetRotation(sphbody[1]), dBodyGetRotation(sphbody[2]), dBodyGetRotation(sphbody[3])};
    float spos[][3] = {
        {static_cast<float>(SPos[0][0]), static_cast<float>(SPos[0][1]), static_cast<float>(SPos[0][2])},
        {static_cast<float>(SPos[1][0]), static_cast<float>(SPos[1][1]), static_cast<float>(SPos[1][2])},
        {static_cast<float>(SPos[2][0]), static_cast<float>(SPos[2][1]), static_cast<float>(SPos[2][2])},
        {static_cast<float>(SPos[3][0]), static_cast<float>(SPos[3][1]), static_cast<float>(SPos[3][2])}
    };
    float srot[][12] = {
        { static_cast<float>(SRot[0][0]), static_cast<float>(SRot[0][1]), static_cast<float>(SRot[0][2]), static_cast<float>(SRot[0][3]), static_cast<float>(SRot[0][4]), static_cast<float>(SRot[0][5]), static_cast<float>(SRot[0][6]), static_cast<float>(SRot[0][7]), static_cast<float>(SRot[0][8]), static_cast<float>(SRot[0][9]), static_cast<float>(SRot[0][10]), static_cast<float>(SRot[0][11]) },
        { static_cast<float>(SRot[1][0]), static_cast<float>(SRot[1][1]), static_cast<float>(SRot[1][2]), static_cast<float>(SRot[1][3]), static_cast<float>(SRot[1][4]), static_cast<float>(SRot[1][5]), static_cast<float>(SRot[1][6]), static_cast<float>(SRot[1][7]), static_cast<float>(SRot[1][8]), static_cast<float>(SRot[1][9]), static_cast<float>(SRot[1][10]), static_cast<float>(SRot[1][11]) },
        { static_cast<float>(SRot[2][0]), static_cast<float>(SRot[2][1]), static_cast<float>(SRot[2][2]), static_cast<float>(SRot[2][3]), static_cast<float>(SRot[2][4]), static_cast<float>(SRot[2][5]), static_cast<float>(SRot[2][6]), static_cast<float>(SRot[2][7]), static_cast<float>(SRot[2][8]), static_cast<float>(SRot[2][9]), static_cast<float>(SRot[2][10]), static_cast<float>(SRot[2][11]) },
        { static_cast<float>(SRot[3][0]), static_cast<float>(SRot[3][1]), static_cast<float>(SRot[3][2]), static_cast<float>(SRot[3][3]), static_cast<float>(SRot[3][4]), static_cast<float>(SRot[3][5]), static_cast<float>(SRot[3][6]), static_cast<float>(SRot[3][7]), static_cast<float>(SRot[3][8]), static_cast<float>(SRot[3][9]), static_cast<float>(SRot[3][10]), static_cast<float>(SRot[3][11]) }
        
    };
    
    for(int i=0; i<sizeof(sphbody); i++){
        dsDrawSphere
        (
         spos[i],
         srot[i],
         RADIUS
         );
    }
    
    // draw world trimesh
    dsSetColor(0.4,0.7,0.9);
    dsSetTexture (DS_NONE);
    
    const dReal* Pos = dGeomGetPosition(world_mesh);
    //dIASSERT(dVALIDVEC3(Pos));
    float pos[3] = { static_cast<float>(Pos[0]), static_cast<float>(Pos[1]), static_cast<float>(Pos[2]) };
    
    const dReal* Rot = dGeomGetRotation(world_mesh);
    //dIASSERT(dVALIDMAT3(Rot));
    float rot[12] = { static_cast<float>(Rot[0]), static_cast<float>(Rot[1]), static_cast<float>(Rot[2]), static_cast<float>(Rot[3]), static_cast<float>(Rot[4]), static_cast<float>(Rot[5]), static_cast<float>(Rot[6]), static_cast<float>(Rot[7]), static_cast<float>(Rot[8]), static_cast<float>(Rot[9]), static_cast<float>(Rot[10]), static_cast<float>(Rot[11] )};
    
    int numi = sizeof(world_indices)  / sizeof(dTriIndex);
    
    for (int i=0; i<numi/3; i++)
    {
        int i0 = world_indices[i*3+0];
        int i1 = world_indices[i*3+1];
        int i2 = world_indices[i*3+2];
        float *v0 = world_vertices+i0*3;
        float *v1 = world_vertices+i1*3;
        float *v2 = world_vertices+i2*3;
        dsDrawTriangle(pos, rot, v0,v1,v2, true); // single precision draw
    }
}


int main (int argc, char **argv)
{
    dMass m;
    dMatrix3 R;
    
    // setup pointers to drawstuff callback functions
    dsFunctions fn;
    fn.version = DS_VERSION;
    fn.start = &start;
    fn.step = &simLoop;
    fn.command = &command;
    fn.stop = 0;
    fn.path_to_textures = DRAWSTUFF_TEXTURE_PATH;
    
    // create world
    dInitODE2(0);
    world = dWorldCreate();
    space = dHashSpaceCreate (0);
    
    contactgroup = dJointGroupCreate (0);
    dWorldSetGravity (world,0,0,-9.8);
    dWorldSetQuickStepNumIterations (world, 64);
    
    // Create a static world using a triangle mesh that we can collide with.
    int numv = sizeof(world_vertices)/(3*sizeof(float));
    int numi = sizeof(world_indices)/ sizeof(dTriIndex);
    printf("numv=%d, numi=%d\n", numv, numi);
    dTriMeshDataID Data = dGeomTriMeshDataCreate();
    
    //  fprintf(stderr,"Building Single Precision Mesh\n");
    
    dGeomTriMeshDataBuildSingle
    (
     Data,
     world_vertices,
     3 * sizeof(float),
     numv,
     world_indices,
     numi,
     3 * sizeof(dTriIndex)
     );
    
    world_mesh = dCreateTriMesh(space, Data, 0, 0, 0);
    dGeomTriMeshEnableTC(world_mesh, dSphereClass, false);
    dGeomTriMeshEnableTC(world_mesh, dBoxClass, false);
    dGeomSetPosition(world_mesh, 0, 0, 0.5);
    dRSetIdentity(R);
    //dIASSERT(dVALIDMAT3(R));
    
    dGeomSetRotation (world_mesh, R);
    //float sx=0.0, sy=3.40, sz=6.80;
    (void)world_normals; // get rid of compiler warning
    for (int i=0; i<sizeof(sphbody); i++){
        sphgeom[i] = dCreateSphere(0, RADIUS);
        sphbody[i] = dBodyCreate (world);
        dMassSetSphere (&m,1,RADIUS);
        dBodySetMass (sphbody[i],&m);
        dGeomSetBody (sphgeom[i],sphbody[i]);
        dSpaceAdd (space, sphgeom[i]);
    }
    reset_ball();
    // run simulation
    dsSimulationLoop (argc,argv,352,288,&fn);
    
    // Causes segm violation? Why?
    // (because dWorldDestroy() destroys body connected to geom; must call first!)
    for (int i=0; i<sizeof(sphbody); i++){
        dGeomDestroy(sphgeom[i]);
    }
    dGeomDestroy (world_mesh);
    
    dJointGroupEmpty (contactgroup);
    dJointGroupDestroy (contactgroup);
    dSpaceDestroy (space);
    dWorldDestroy (world);
    dCloseODE();
    return 0;
}
