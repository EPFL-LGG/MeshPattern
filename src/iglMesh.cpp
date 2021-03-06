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

vector<vector<int>> iglMesh::getFaces() {
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
    UV_.setZero();

    UV_ = V_.leftCols(2);
    Eigen::Vector2d min_coeff, max_coeff;
    min_coeff = UV_.colwise().minCoeff();
    max_coeff = UV_.colwise().maxCoeff();
    for(int id = 0; id < UV_.rows(); id++)
    {
        UV_(id, 0) = (UV_(id, 0) - min_coeff(0)) / (max_coeff(0) - min_coeff(0));
        UV_(id, 1) = (UV_(id, 1) - min_coeff(1)) / (max_coeff(1) - min_coeff(1));
    }
}

void iglMesh::parametrization_lscm()
{
    UV_.setZero();

    Eigen::VectorXi bnd,b(2);
    igl::boundary_loop(F_,bnd);
    b(0) = bnd(0);
    b(1) = bnd(((int)round(bnd.size() / 2)));
    MatrixXd bc(2,2);
    bc<<0,0,1,0;
    igl::lscm(V_,F_,b,bc,UV_);

    Eigen::Vector2d min_coeff, max_coeff;
    min_coeff = UV_.colwise().minCoeff();
    max_coeff = UV_.colwise().maxCoeff();
    for(int id = 0; id < UV_.rows(); id++)
    {
        UV_(id, 0) = (UV_(id, 0) - min_coeff(0));
        UV_(id, 1) = (UV_(id, 1) - min_coeff(1));
    }
    return;
}

bool iglMesh::mapPoint3D_simple(Eigen::Vector3d pt, Eigen::Vector3d &l, int &faceID)
{
    if(UV_.isZero())
    {
        return false;
    }
    else{
        Eigen::MatrixXd P(1, 3), L(1, 3);
        P << pt(0), pt(1), pt(2);
        for(int id = 0; id < F_.rows(); id++)
        {
            Eigen::MatrixXd A, B, C;
            getTriFace(id, UV_, A, B, C);
            igl::barycentric_coordinates(P, A, B, C, L);
            if(L.minCoeff() < -1e-5 || L.maxCoeff() > 1 + 1e-5){
                continue;
            }
            else{
                l = L.row(0);
                faceID = id;
                return true;
            }
        }
    }

    return false;
}

void iglMesh::mapMesh3D_simple(iglMesh &baseMesh)
{
    if(UV_.isZero()){
        parametrization_simple();
    }

    vector<bool> inMesh;
    int count = 0;
    for(int id = 0; id < baseMesh.V_.rows(); id++)
    {
        Eigen::Vector3d pt = baseMesh.V_.row(id);
        Eigen::Vector3d l(0, 0, 0);
        int faceID = -1;
        if(mapPoint3D_simple(pt, l, faceID))
        {
            Eigen::MatrixXd A, B, C;
            getTriFace(faceID, V_, A, B, C);
            baseMesh.V_.row(id) = A.row(0) * l(0) + B.row(0) * l(1) + C.row(0) * l(2);
            inMesh.push_back(true);
        }
        else{
            inMesh.push_back(false);
        }
    }

    for(auto it = baseMesh.faces_.begin(); it != baseMesh.faces_.end();)
    {
        bool all_in_refMesh = true;
        for(int jd = 0; jd < (*it).size(); jd++)
        {
            int vid = (*it)[jd];
            if(inMesh[vid] == false){
                all_in_refMesh = false;
                break;
            }
        }
        if(!all_in_refMesh)
        {
            it = baseMesh.faces_.erase(it);
        }
        else{
            it++;
        }
    }

    baseMesh.cleanMesh();
}

void iglMesh::mapMesh3D_AABB(iglMesh &baseMesh)
{
    if(UV_.isZero()){
        parametrization_lscm();
    }

    igl::AABB<MatrixXd, 2> tree;
    tree.init(UV_, F_);

    vector<bool> inMesh;
    baseMesh.parametrization_simple();
    MatrixXd C;
    Eigen::VectorXi I;
    Eigen::VectorXd sqrD;
    tree.squared_distance(UV_, F_, baseMesh.UV_, sqrD, I, C);
    
    for(int id = 0; id < baseMesh.UV_.rows(); id++)
    {
        if(std::fabs(sqrD[id]) < 1e-5)
        {
            Eigen::MatrixXd A, B, C, L, P(1, 3);
            P << baseMesh.UV_(id, 0), baseMesh.UV_(id, 1), 0;
            getTriFace(I(id), UV_, A, B, C);
            igl::barycentric_coordinates(P, A, B, C, L);

            inMesh.push_back(true);
            getTriFace(I(id), V_, A, B, C);
            baseMesh.V_.row(id) = A.row(0) * L(0, 0) + B.row(0) * L(0, 1) + C.row(0) * L(0, 2);
        }
        else{
            inMesh.push_back(false);
        }
    }

    for(vector<vector<int>>::iterator it = baseMesh.faces_.begin(); it != baseMesh.faces_.end();)
    {
        bool all_in_refMesh = true;
        for(int jd = 0; jd < (*it).size(); jd++)
        {
            int vid = (*it)[jd];
            if(inMesh[vid] == false){
                all_in_refMesh = false;
                break;
            }
        }
        if(!all_in_refMesh){
            it = baseMesh.faces_.erase(it);
        }
        else{
            it++;
        }
    }

    baseMesh.cleanMesh();
}


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

iglMesh iglMesh::saveWireFrame(double thickness, int cylinder_pts)
{

    MatrixXi E;
    igl::edges(F_, E);


    vector<vector<int>> vv;
    vv.resize(V_.rows());
    for(int id = 0; id < faces_.size(); id++){
        for(int jd = 0; jd < faces_[id].size(); jd++){
            int prev = (jd - 1 + faces_[id].size()) % faces_[id].size();
            int next = (jd + 1) % faces_[id].size();
            vv[faces_[id][jd]].push_back(faces_[id][prev]);
            vv[faces_[id][jd]].push_back(faces_[id][next]);
        }
    }

    vector<vector<double>> vers;
    vector<vector<int>> faces;
    for(int id = 0; id < E.rows(); id++)
    {
        int sta = E(id, 0);
        int end = E(id, 1);
        Vector3d staPt = V_.row(sta);
        Vector3d endPt = V_.row(end);

        if(std::any_of(vv[sta].begin(), vv[sta].end(), [=](int a){return a == end;}))
        {
            Vector3d z_axis = endPt - staPt; z_axis /= (z_axis).norm();
            Vector3d x_axis = Vector3d(0, 1, 0).cross(z_axis);
            if(x_axis.norm() < 1e-4) x_axis = Vector3d(1, 0, 0).cross(z_axis); x_axis /= z_axis.norm();
            Vector3d y_axis = z_axis.cross(x_axis); y_axis /= y_axis.norm();

            for(int id = 0; id < cylinder_pts; id++)
            {
                float angle0 = 2 * 3.1415926/cylinder_pts * id;
                float angle1 = 2 * 3.1415926/cylinder_pts * (id + 1);
                Vector3d v_sta0 = staPt + x_axis * thickness * cos(angle0) + y_axis * thickness * sin(angle0);
                Vector3d v_sta1 = staPt + x_axis * thickness * cos(angle1) + y_axis * thickness * sin(angle1);
                Vector3d v_end0 = v_sta0 + (endPt - staPt);
                Vector3d v_end1 = v_sta1 + (endPt - staPt);
                int f = vers.size();
                vers.push_back({v_sta0(0),v_sta0(1), v_sta0(2)});
                vers.push_back({v_sta1(0),v_sta1(1), v_sta1(2)});
                vers.push_back({v_end1(0),v_end1(1), v_end1(2)});
                vers.push_back({v_end0(0),v_end0(1), v_end0(2)});
                faces.push_back({f, f + 1, f + 2, f + 3});
            }
        }
    }

    iglMesh mesh;
    mesh.loadMesh(vers, faces);
    return mesh;
}







