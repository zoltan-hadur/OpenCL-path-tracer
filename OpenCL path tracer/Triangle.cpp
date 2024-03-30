#include "Triangle.h"

Triangle::Triangle(Vector3 v1, Vector3 v2, Vector3 v3, cl_uint mati, cl_uint texi)
{
    this->v1 = v1;
    this->v2 = v2;
    this->v3 = v3;
    this->mati = mati;
    this->texi = texi;
}