#include "dvs_calibration/board_detection.h"

std::vector<cv::Point2f> BoardDetection::findPattern(std::list<PointWithWeight> points, int dots, int minimum_mass)
{
  cv::Size patternsize(dots, dots); //number of centers
  std::vector<cv::Point2f> centers; //this will be filled by the detected centers
  std::vector<cv::Point2f> centers_tmp; //this will be filled by the detected centers
  std::vector<int> center_count;

  std::vector<std::list<PointWithWeight> > clusters;

  std::list<PointWithWeight>::iterator point_it, cluster_it;
  while (!points.empty())
  {
    std::list<PointWithWeight> current_cluster;
    point_it = points.begin();
    current_cluster.push_back(*point_it);
    point_it = points.erase(point_it);

    bool found_a_neighbor = true;
    while (found_a_neighbor)
    {
      found_a_neighbor = false;
      for (point_it = points.begin(); point_it != points.end(); ++point_it)
      {
        for (cluster_it = current_cluster.begin(); cluster_it != current_cluster.end(); ++cluster_it)
        {
          cv::Point dist = point_it->point - cluster_it->point;
          if (dist.x >= -1 && dist.x <= 1 && dist.y >= -1 && dist.y <= 1)
          {
            current_cluster.push_back(*point_it);
            point_it = points.erase(point_it);
            found_a_neighbor = true;
            break;
          }
        }
      }
    }

    int cluster_mass = 0;
    for (cluster_it = current_cluster.begin(); cluster_it != current_cluster.end(); ++cluster_it)
      cluster_mass += cluster_it->weight;
    if (cluster_mass >= minimum_mass)
      clusters.push_back(current_cluster);
  }

  std::vector<cv::Point2f> centers_good;
  if (clusters.size() == dots * dots)
  {
    std::vector<cv::Point2f> centers;
    for (int i = 0; i < clusters.size(); ++i)
    {
      cv::Point2f center(0, 0);
      int mass = 0;
      std::list<PointWithWeight>::iterator cluster_it;
      for (cluster_it = clusters[i].begin(); cluster_it != clusters[i].end(); ++cluster_it)
      {
        center.x += cluster_it->point.x * cluster_it->weight;
        center.y += cluster_it->point.y * cluster_it->weight;
        mass += cluster_it->weight;
      }
      center.x /= (double)mass;
      center.y /= (double)mass;
      centers.push_back(center);
    }

    CirclesGridClusterFinder grid(false);
    grid.findGrid(centers, patternsize, centers_good);
  }

  return centers_good;
}
