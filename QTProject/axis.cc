/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2003-2019
 * GPL 3
 */


#include "axis.h"

using namespace _colors_ne;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_axis::_axis(float Size)
{
    Vertices.resize(6);
    Vertices[0]= _vertex3f(-Size, 0, 0);// x
    Vertices[1] = _vertex3f(-Size , 0 , 0 ) ;
    Vertices[2] = _vertex3f( 0 ,Size , 0 ) ; // y
    Vertices[3] = _vertex3f(0 , -Size , 0 ) ;
    Vertices[4] = _vertex3f( 0 , 0 ,Size) ; // z
    Vertices[5] = _vertex3f(0 ,0 , -Size) ;

    Colors.resize(6);
    Colors[0] = _vertex3f(1, 0, 0); // red
    Colors[1] = _vertex3f(1, 0, 0);
    Colors[2] = _vertex3f(0, 1, 0); // green
    Colors[3] = _vertex3f(0, 1, 0);
    Colors[4] = _vertex3f(0, 0, 1); // blue
    Colors[5] = _vertex3f(0, 0, 1);

}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/


void _axis::draw_line()
{

    //glDrawElements(GL_LINES, Vertices.size(), GL_UNSIGNED_INT, nullptr);
    //glLineWidth(1);
    //int vertexLocation = program->attributeLocation("vertex");
    //int matrixLocation = program->uniformLocation("matrix");
    //
    //
    //
    //program->enableAttributeArray(vertexLocation);
    ////program->setAttributeArray(vertexLocation, Vertices.data(), 3);
    ////program->setAttributeArray(vertexLocation, Vertices.data());
    //program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(Vertices[0]));
    //program->setUniformValue(matrixLocation, Projection);
    //
    //glPointSize(10);
    //glDrawArrays(GL_LINES, 0, 3);
}



