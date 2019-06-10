//
// Created by ziqwang on 2019-04-13.
//

#include "ShapeOpt.h"
#include <igl/boundary_loop.h>
void ShapeOpt::loadMesh(iglMesh &_mesh)
{
    mesh_ = _mesh;
    V_ = mesh_.V_.transpose();
    solver = ShapeOp::Solver();
    solver.setPoints(V_);
    //std::cout << V_ << std::endl;
}

void ShapeOpt::addPlanarConstraint(double weight)
{
    //in plane constraint
    std::vector<int> id_vector;
    for(int id = 0; id < mesh_.faces_.size(); id++)
    {
        id_vector.clear();
        for(int verID : mesh_.faces_[id]){
            id_vector.push_back(verID);
        }
        shared_ptr<ShapeOp::Constraint> constraint = make_shared<ShapeOp::PlaneConstraint>(id_vector, weight, V_);
        solver.addConstraint(constraint);
    }
}

void ShapeOpt::addRegularConstraint(double weight)
{
    //regular
    for(int id = 0 ;id < mesh_.faces_.size(); id++)
    {
        int num_verIDs = mesh_.faces_[id].size();


        Eigen::Vector3d center = getCentroid(id);
        Vector3d normal = getNormal(id);
        Vector3d x_axis = Vector3d(1, 0, 0).cross(normal);
        if(x_axis.norm() < 1e-4) x_axis = Vector3d(1, 0, 0).cross(normal);
        Vector3d y_axis = normal.cross(x_axis);
        Eigen::MatrixXd shape(3, num_verIDs);
        vector<int> id_vector;
        for(int jd = 0; jd < num_verIDs; jd ++)
        {
            float angle = 3.1415926 * 2 / num_verIDs * jd;
            Vector3d pt = x_axis * std::cos(angle) + y_axis * std::sin(angle) + center;
            shape.col(jd) << pt[0], pt[1], pt[2];
            id_vector.push_back(mesh_.faces_[id][jd]);
        }
        shared_ptr<ShapeOp::SimilarityConstraint> constraint = make_shared<ShapeOp::SimilarityConstraint>(id_vector, weight, V_ ,true, true, true);
        std::vector<ShapeOp::Matrix3X> shapes;
        shapes.push_back(shape);
        constraint->setShapes(shapes);
        solver.addConstraint(constraint);
    }
}

void ShapeOpt::addBoundaryConstraint(double weight)
{
    std::vector<std::vector<int> > L;
    igl::boundary_loop(mesh_.F_, L);

    for(int id = 0; id < L.size(); id++)
    {
        for(int jd = 0; jd < L[id].size(); jd++)
        {
            std::vector<int> id_vector;
            int verID = L[id][jd]; id_vector.push_back(verID);
            ShapeOp::Vector3 pt = V_.col(verID);
            shared_ptr<ShapeOp::ClosenessConstraint> constraint = make_shared<ShapeOp::ClosenessConstraint>(id_vector, weight,  V_);
            constraint->setPosition(pt);
            solver.addConstraint(constraint);
        }
    }

}

void ShapeOpt::runShapeOp(int iterations)
{
    solver.initialize();
    solver.solve(iterations);
    ShapeOp::Matrix3X P = solver.getPoints();
    V_ = P;
    //std::cout << V_ << std::endl;
}

iglMesh ShapeOpt::getMesh()
{
    mesh_.V_ = V_.transpose();
    return mesh_;
}

Vector3d ShapeOpt::getCentroid(int fid)
{
    Vector3d centroid(0, 0, 0);
    for(int jd = 0; jd < mesh_.faces_[fid].size(); jd ++)
    {
        int vID = mesh_.faces_[fid][jd];
        Vector3d pt = V_.col(vID);
        centroid += pt;
    }
    centroid /= mesh_.faces_[fid].size();
    return centroid;
}

Vector3d ShapeOpt::getNormal(int fid) {


    vector<vector<double>> vers;
    for(int jd = 0; jd < mesh_.faces_[fid].size(); jd ++)
    {
        int vID = mesh_.faces_[fid][jd];
        vector<double> pt;
        pt.push_back(V_(0, vID));pt.push_back(V_(1, vID));pt.push_back(V_(2, vID));
        vers.push_back(pt);
    }

    if(vers.size() < 3)
    {
        return Vector3d(0, 0, 0);
    }

    Vector3d centroid = getCentroid(fid);

    double xx, xy, xz, yy, yz, zz;
    xx = xy = xz = yy = yz = zz = 0;
    for(vector<double> pt: vers)
    {
        Vector3d r = Vector3d(pt[0], pt[1], pt[2]) - centroid;
        xx += r[0] * r[0];
        xy += r[0] * r[1];
        xz += r[0] * r[2];
        yy += r[1] * r[1];
        yz += r[1] * r[2];
        zz += r[2] * r[2];
    }

    double det_x = yy*zz - yz*yz;
    double det_y = xx*zz - xz*xz;
    double det_z = xx*yy - xy*xy;

    double maxDet = std::max(det_x, std::max(det_y, det_z));
    if(maxDet <= 0){
        return Vector3d(0, 0, 0);
    }

    Vector3d normal(0, 0, 0);
    if(maxDet == det_x)
    {
        normal = Vector3d(det_x, xz*yz - xy*zz, xy*yz - xz*yy);
    }
    else if(maxDet == det_y)
    {
        normal = Vector3d(xz*yz - xy*zz, det_y, xy*xz - yz*xx);
    }
    else {
        normal = Vector3d(xy*yz - xz*yy, xy*xz - yz*xx, det_z);
    };
    normal /= normal.norm();
    return normal;
}
