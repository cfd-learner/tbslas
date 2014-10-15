// *************************************************************************
// Copyright (C) 2014 by Arash Bakhtiari
// You may not use this file except in compliance with the License.
// You obtain a copy of the License in the LICENSE file.

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// *************************************************************************

#ifndef SRC_TREE_TREE_UTILS_H_
#define SRC_TREE_TREE_UTILS_H_

#include <vector>

#include "tree/tree_common.h"

namespace tbslas {

template <typename real_t, typename InputFunction>
void construct_tree(const size_t N,
                    const size_t M,
                    const int cheb_deg,
                    const int depth,
                    const bool adap,
                    const real_t tol,
                    const MPI_Comm& comm,
                    const InputFunction input_fn,
                    const int data_dof,
                    Tree_t<real_t>& tree) {
  //Various parameters.
  typename Node_t<real_t>::NodeData tree_data;
  tree_data.dim       = COORD_DIM;
  tree_data.max_depth = depth;
  tree_data.cheb_deg  = cheb_deg;

  //Set input function pointer
  tree_data.input_fn  = input_fn;
  tree_data.data_dof  = data_dof;
  tree_data.tol       = tol;

  //Set source coordinates.
  std::vector<real_t> pt_coord;
  pt_coord=point_distrib<real_t>(UnifGrid,N,comm);
  tree_data.max_pts  = M; // Points per octant.
  tree_data.pt_coord = pt_coord;

  //initialize with input data.
  tree.Initialize(&tree_data);
  tree.RefineTree();
  tree.Balance21(pvfmm::FreeSpace);
  //tree.RedistNodes();
}

template<typename real_t>
std::vector<int>
isOutside(Node_t<real_t>* n,
          const std::vector<real_t> x,
          const std::vector<real_t> y,
          const std::vector<real_t> z) {
  assert((x.size() == y.size()) && (y.size() == z.size()));

  real_t* node_coord = n->Coord();
  int depth          = n->Depth();
  real_t length      = static_cast<real_t>(std::pow(0.5, depth));

  real_t xmin = node_coord[0];
  real_t xmax = xmin + length;
  real_t ymin = node_coord[1];
  real_t ymax = ymin + length;
  real_t zmin = node_coord[2];
  real_t zmax = zmin + length;

  std::vector<int> out_index_list;
  for (int i = 0; i < x.size(); i++) {
    if ( x[i] < xmin || x[i] > xmax) {
      out_index_list.push_back(i);
      continue;
    }
    if ( y[i] < ymin || y[i] > ymax) {
      out_index_list.push_back(i);
      continue;
    }
    if ( z[i] < zmin || z[i] > zmax) {
      out_index_list.push_back(i);
      continue;
    }
  }
  return out_index_list;
}

}
#endif // SRC_TREE_TREE_UTILS_H_
