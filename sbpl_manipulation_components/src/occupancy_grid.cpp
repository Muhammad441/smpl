/*
 * Copyright (c) 2010, Maxim Likhachev
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of Pennsylvania nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

 /** \author Benjamin Cohen */

#include <sbpl_manipulation_components/occupancy_grid.h>
#include <ros/console.h>
#include <leatherman/viz.h>

using namespace std;

namespace sbpl_arm_planner
{

OccupancyGrid::OccupancyGrid(double dim_x, double dim_y, double dim_z, double resolution, double origin_x, double origin_y, double origin_z)
{
  grid_ = new distance_field::PropagationDistanceField(dim_x, dim_y, dim_z, resolution, origin_x, origin_y,  origin_z, 0.40);
  grid_->reset();
}

OccupancyGrid::OccupancyGrid(distance_field::PropagationDistanceField* df)
{
  grid_ = df;
}

OccupancyGrid::~OccupancyGrid()
{
  /*
  if(grid_)
    delete grid_;
  */
}

void OccupancyGrid::getGridSize(int &dim_x, int &dim_y, int &dim_z)
{
  dim_x = grid_->getSize(distance_field::PropagationDistanceField::DIM_X) / grid_->getResolution(distance_field::PropagationDistanceField::DIM_X);
  dim_y = grid_->getSize(distance_field::PropagationDistanceField::DIM_Y) / grid_->getResolution(distance_field::PropagationDistanceField::DIM_Y);
  dim_z = grid_->getSize(distance_field::PropagationDistanceField::DIM_Z) / grid_->getResolution(distance_field::PropagationDistanceField::DIM_Z);
}

void OccupancyGrid::getWorldSize(double &dim_x, double &dim_y, double &dim_z)
{
  dim_x = grid_->getSize(distance_field::PropagationDistanceField::DIM_X);
  dim_y = grid_->getSize(distance_field::PropagationDistanceField::DIM_Y);
  dim_z = grid_->getSize(distance_field::PropagationDistanceField::DIM_Z);
}

void OccupancyGrid::reset()
{
  grid_->reset();
}

void OccupancyGrid::getOrigin(double &wx, double &wy, double &wz)
{
  grid_->gridToWorld(0, 0, 0, wx, wy, wz);
}

double OccupancyGrid::getResolution()
{
  return grid_->getResolution(distance_field::PropagationDistanceField::DIM_X);
}

void OccupancyGrid::updateFromCollisionMap(const arm_navigation_msgs::CollisionMap &collision_map)
{
  if(collision_map.boxes.empty())
  {
    ROS_DEBUG("[grid] collision map received is empty.");
    return;
  }
  reference_frame_ = collision_map.header.frame_id;
  grid_->addCollisionMapToField(collision_map);
}

void OccupancyGrid::addCube(double origin_x, double origin_y, double origin_z, double size_x, double size_y, double size_z)
{
  int num_points=0;
  std::vector<tf::Vector3> pts;

  for (double x=origin_x-size_x/2.0; x<=origin_x+size_x/2.0; x+=grid_->getResolution(distance_field::PropagationDistanceField::DIM_X))
  {
    for (double y=origin_y-size_y/2.0; y<=origin_y+size_y/2.0; y+=grid_->getResolution(distance_field::PropagationDistanceField::DIM_Y))
    {
      for (double z=origin_z-size_z/2.0; z<=origin_z+size_z/2.0; z+=grid_->getResolution(distance_field::PropagationDistanceField::DIM_Z))
      {
        pts.push_back(tf::Vector3(x,y,z));
        ++num_points;
      }
    }
  }

  grid_->addPointsToField(pts);
}

void OccupancyGrid::getOccupiedVoxels(const geometry_msgs::Pose &pose, const std::vector<double> &dim, std::vector<Eigen::Vector3d> &voxels)
{
  Eigen::Vector3d vin, vout, v(pose.position.x, pose.position.y, pose.position.z);
  Eigen::Matrix3d m(Eigen::Quaterniond(pose.orientation.w, pose.orientation.x, pose.orientation.y, pose.orientation.z));

  for (double x=0-dim[0]/2.0; x<=dim[0]/2.0; x+=grid_->getResolution(distance_field::PropagationDistanceField::DIM_X))
  {
    for (double y=0-dim[1]/2.0; y<=dim[1]/2.0; y+=grid_->getResolution(distance_field::PropagationDistanceField::DIM_Y))
    {
      for (double z=0-dim[2]/2.0; z<=dim[2]/2.0; z+=grid_->getResolution(distance_field::PropagationDistanceField::DIM_Z))
      {
        vin(0) = (x);
        vin(1) = (y);
        vin(2) = (z); 
        vout = m*vin;
        vout += v;

        voxels.push_back(vout);
      }
    }
  }
}

visualization_msgs::MarkerArray OccupancyGrid::getVisualization(std::string type)
{
  visualization_msgs::MarkerArray ma;

  if(type.compare("bounds") == 0)
  {
    double dimx, dimy, dimz, originx, originy, originz;
    std::vector<geometry_msgs::Point> pts(10);
    getOrigin(originx, originy, originz);
    getWorldSize(dimx,dimy,dimz);
    pts[0].x = originx;      pts[0].y = originy;      pts[0].z = originz;
    pts[1].x = originx+dimx; pts[1].y = originy;      pts[1].z = originz;
    pts[2].x = originx+dimx; pts[2].y = originy+dimy; pts[2].z = originz;
    pts[3].x = originx;      pts[3].y = originy+dimy; pts[3].z = originz;
    pts[4].x = originx;      pts[4].y = originy;      pts[4].z = originz;
    pts[5].x = originx;      pts[5].y = originy;      pts[5].z = originz+dimz;
    pts[6].x = originx+dimx; pts[6].y = originy;      pts[6].z = originz+dimz;
    pts[7].x = originx+dimx; pts[7].y = originy+dimy; pts[7].z = originz+dimz;
    pts[8].x = originx;      pts[8].y = originy+dimy; pts[8].z = originz+dimz;
    pts[9].x = originx;      pts[9].y = originy;      pts[9].z = originz+dimz;

    ma.markers.resize(1);
    ma.markers[0] = viz::getLineMarker(pts, 0.05, 10, getReferenceFrame(), "collision_space_bounds", 0);
  }
  else if(type.compare("distance_field") == 0)
  {
    visualization_msgs::Marker m;
    // grid_->getIsoSurfaceMarkers(0.01, 0.03, getReferenceFrame(), ros::Time::now(),  Eigen::Affine3d::Identity(), m);
    grid_->getIsoSurfaceMarkers(0.01, 0.03, getReferenceFrame(), ros::Time::now(), tf::Transform(tf::createIdentityQuaternion(), tf::Vector3(0,0,0)), m);
    ma.markers.push_back(m);
  }
  else
    ROS_ERROR("No visualization found of type '%s'.", type.c_str());
  return ma;
}


}
