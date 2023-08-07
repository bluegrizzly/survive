#pragma once
#include "DrawDebugHelpers.h"

#define SHOW_DEBUG false
#define SHOW_DEBUG_AI false
#define SHOW_DEBUG_COMBAT false

#define DRAW_SPHERE(location, color, radius, duration) if (GetWorld()) { DrawDebugSphere(GetWorld(), location, radius, 12, color, false, duration); }
#define DRAW_SPHERE_SingleFrame(location, color, radius) if (GetWorld()) {  DrawDebugSphere(GetWorld(), location, radius, 12, color, false, -1.f); }
#define DRAW_LINE(startLocation, endLocation, color)  if ( GetWorld()) { DrawDebugLine(GetWorld(),startLocation, endLocation, color, true, -1.f, 0, 1.f);}
#define DRAW_LINE_SingleFrame(startLocation, endLocation, color)  if ( GetWorld()) { DrawDebugLine(GetWorld(),startLocation, endLocation, color, false, -1.f, 0, 1.f);}
#define DRAW_POINT(location, color) if (GetWorld()) { DrawDebugPoint(GetWorld(), location, 10.f, color, true); }
#define DRAW_POINT_SingleFrame(location, color) if (GetWorld()) { DrawDebugPoint(GetWorld(), location, 10.f, color, false, -1.f); }
#define DRAW_VECTOR(startLocation, endLocation, color) if (GetWorld()) \
    { \
        DrawDebugLine(GetWorld(),startLocation, endLocation, color, true, -1.f, 0, 1.f); \
        DrawDebugPoint(GetWorld(), endLocation, 10.f, color, true); \
    } 
#define DRAW_VECTOR_SingleFrame(startLocation, endLocation, color) if (GetWorld()) \
    { \
        DrawDebugLine(GetWorld(), startLocation, endLocation, color, false, -1.f, 0, 1.f); \
        DrawDebugPoint(GetWorld(), endLocation, 10.f, color, false, -1.f); \
    } 

#define DRAW_CUBE(location, size, color) if (GetWorld()) {  DrawDebugBox(GetWorld(), location, FVector(size,size,size), color,true); }
#define DRAW_CUBE_SingleFrame(location, size, color) if (GetWorld()) {  DrawDebugBox(GetWorld(), location, FVector(size,size,size), color, false, -1.f); }

