/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#include "object3d.h"

using namespace _colors_ne;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _object3D::draw_line()
{
    //glBegin(GL_LINES);
    //for(unsigned int i=0; i< Triangles.size(); i++)
    //{
    //    glColor3f(1.0, 0.0, 0.0);
    //    glVertex3fv((GLfloat *) &Vertices[Triangles[i].x]);
    //    glColor3f(0.0, 1.0, 0.0);
    //    glVertex3fv((GLfloat *) &Vertices[Triangles[i].y]);
    //    glColor3f(0.0, 0.0, 1.0);
    //    glVertex3fv((GLfloat *) &Vertices[Triangles[i].z]);
    //}
    //glEnd();

//
// int vertexLocation = program->attributeLocation("vertex");
// int matrixLocation = program->uniformLocation("matrix");
//
//
// program->enableAttributeArray(vertexLocation);
// program->setAttributeArray(vertexLocation, Vertices.data(), 3);
// program->setUniformValue(matrixLocation, Projection);
//
// glDrawArrays(GL_LINES, 0, 3);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _object3D::draw_fill()
{
   //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   //glBegin(GL_TRIANGLES);
   //for(unsigned int i=0; i< Triangles.size(); i++)
   //{
   //    glColor3f(1.0, 0.0, 0.0);
   //    glVertex3fv((GLfloat *) &Vertices[Triangles[i].x]);
   //    glColor3f(0.0, 1.0, 0.0);
   //    glVertex3fv((GLfloat *) &Vertices[Triangles[i].y]);
   //    glColor3f(0.0, 0.0, 1.0);
   //    glVertex3fv((GLfloat *) &Vertices[Triangles[i].z]);
   //}
   //glEnd();

   //int vertexLocation = program->attributeLocation("vertex");
   //int matrixLocation = program->uniformLocation("matrix");
   //
   //
   //program->enableAttributeArray(vertexLocation);
   //program->setAttributeArray(vertexLocation, Vertices.data(), 3);
   //program->setUniformValue(matrixLocation, Projection);
   //
   //glDrawArrays(GL_TRIANGLES, 0, 3);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _object3D::draw_chess()
{
  
}

