//
// Created by ziqwang on 2019-04-13.
//
#include "iglMesh.h"
#include <igl/polygon_mesh_to_triangle_mesh.h>
#include <igl/remove_duplicate_vertices.h>
#include <igl/barycentric_coordinates.h>
#include <igl/lscm.h>
#include <igl/boundary_loop.h>
#include <igl/AABB.h>
#include <igl/edges.h>
#include <algorithm>

void iglMesh::loadMesh(vector<vector<double>> &_vertices, vector<vector<int>> &_faces)
{
    //read vertices' coordinates
    V_ = Eigen::MatrixXd::Zero(_vertices.size() , 3);
    for(int id = 0; id < _vertices.size(); id++)
    {
        V_.row(id) = Eigen::RowVector3d(_vertices[id][0], _vertices[id][1], _vertices[id][2]);
    }

    //read faces
    faces_ = _faces;

    cleanMesh();
}

vector<vector<double>> iglMesh::getVertices()
{
    //write vertices
    vector<vector<double>> vertices;
    for(int id = 0; id < V_.rows(); id++){
        vector<double> pos;
        pos.push_back(V_(id, 0));pos.push_back(V_(id, 1));pos.push_back(V_(id, 2));
        vertices.push_back(pos);
    }
    return vertices;
}

vector<vector<int>> iglMesh::getFaces()
{
    return faces_;
}

vector<vector<double>> iglMesh::getUVs()
{
    //write vertices
    vector<vector<double>> uvs;
    for(int id = 0; id < UV_.rows(); id++){
        vector<double> pos;
        pos.push_back(UV_(id, 0));pos.push_back(UV_(id, 1)); pos.push_back(0);
        uvs.push_back(pos);
    }
    return uvs;
}

void iglMesh::parametrization_simple()
{

}

/*===============================================================
                         Private Function
  ===============================================================*/

void iglMesh::getTriFace(int faceID, Eigen::MatrixXd &V, Eigen::MatrixXd &A, Eigen::MatrixXd &B, Eigen::MatrixXd &C)
{
    if(0 <= faceID && faceID < F_.rows())
    {
        A = MatrixXd(1, 3);
        B = MatrixXd(1, 3);
        C = MatrixXd(1, 3);
        if(V.cols() == 2)
        {
            A << V(F_(faceID, 0), 0), V(F_(faceID, 0), 1), 0;
            B << V(F_(faceID, 1), 0), V(F_(faceID, 1), 1), 0;
            C << V(F_(faceID, 2), 0), V(F_(faceID, 2), 1), 0;
        }
        else if(V.cols() == 3){
            A << V(F_(faceID, 0), 0), V(F_(faceID, 0), 1), V(F_(faceID, 0), 2);
            B << V(F_(faceID, 1), 0), V(F_(faceID, 1), 1), V(F_(faceID, 0), 2);
            C << V(F_(faceID, 2), 0), V(F_(faceID, 2), 1), V(F_(faceID, 0), 2);
        }
    }

    return;
}

void iglMesh::cleanMesh()
{
    
    //get triangle face
    igl::polygon_mesh_to_triangle_mesh(faces_, F_);
    
    //remove duplicate
    MatrixXd SV;
    MatrixXi SF;
    Eigen::VectorXi SVI, SVJ;
    igl::remove_duplicate_vertices(V_, F_, 1e-6f, SV, SVI, SVJ, SF);
    F_ = SF;
    V_ = SV;
    
    for(int id = 0;id < faces_.size(); ++id)
    {
        for(int jd = 0; jd < faces_[id].size(); ++jd)
        {
            faces_[id][jd] = SVJ(faces_[id][jd]);
        }
    }
    
    
    vector<bool> inList; inList.resize(V_.rows(), false);
    for(int id = 0; id < faces_.size(); id++)
    {
        for(int jd = 0; jd < faces_[id].size(); jd++)
        {
            inList[faces_[id][jd]] = true;
        }
    }
    
    int count = 0;
    vector<int> newIndex;
    for(int id = 0; id < inList.size(); id++){
        if(inList[id]) {
            newIndex.push_back(count++);
        }
        else{
            newIndex.push_back(-1);
        }
    }
    
    SV = MatrixXd(count, 3);
    for(int id = 0; id < inList.size(); id++){
        if(inList[id]){
            SV.row(newIndex[id]) = V_.row(id);
        }
    }
    
    for(int id = 0; id < faces_.size(); id++)
    {
        for(int jd = 0; jd < faces_[id].size(); jd++)
        {
            faces_[id][jd] = newIndex[faces_[id][jd]];
        }
    }
    V_ = SV;
    
    //get triangle face
    igl::polygon_mesh_to_triangle_mesh(faces_, F_);
    return;
    
}
