//
// Created by ziqwang on 2019-04-13.
//

#ifndef COMPAS_LIBIGL_IGLMESH_H
#define COMPAS_LIBIGL_IGLMESH_H

#include <Eigen/Dense>
#include <vector>
#include <iostream>
using std::vector;
using Eigen::MatrixXd;
using Eigen::MatrixXi;
using Eigen::Vector3d;

/*
 * A simple interface exchange data from compas mesh and igl mesh
 */
class iglMesh {
public:

    //a matrix records vertices' coordinates
    //n x 3 matrix, n is the number of vertices
    //v_0^x v_0^y v_0^z
    //v_1^x v_1^y v_1^z
    //...
    //v_n^x v_n^y v_n^z
    MatrixXd V_;

    //a vector list record vertices' indices in each face
    //each face is a polygon
    vector<vector<int>> faces_;
    
    //faces_ after triangulation.
    //useful in libigl
    //m x 3 matrix, m is the number of triangle faces
    MatrixXi F_;
    
    //2D texture coordinates
    MatrixXd UV_;
    
    //IO
public:

    iglMesh(){};

    void loadMesh(vector<vector<double>> &_vertices, vector<vector<int>>& _faces);

    vector<vector<double>> getVertices();

    vector<vector<double>> getUVs();

    vector<vector<int>> getFaces();

    //Utility Function
public:

    void parametrization_simple();
    
    //Private Function
private:

    void cleanMesh();

    void getTriFace(int faceID, Eigen::MatrixXd &V, Eigen::MatrixXd &A, Eigen::MatrixXd &B, Eigen::MatrixXd &C);

public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


#endif //COMPAS_LIBIGL_IGLMESH_H
