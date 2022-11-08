// Copyright 2022 DeepMind Technologies Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MUJOCO_SRC_PLUGIN_ELASTICITY_CABLE_H_
#define MUJOCO_SRC_PLUGIN_ELASTICITY_CABLE_H_

#include <optional>
#include <vector>

#include <mujoco/mjdata.h>
#include <mujoco/mjmodel.h>
#include <mujoco/mjtnum.h>


namespace mujoco::plugin::elasticity {

class Cable {
 public:
  // Creates a new Cable instance (allocated with `new`) or
  // returns null on failure.
  static std::optional<Cable> Create(const mjModel* m, mjData* d,
                                          int instance);
  Cable(Cable&&) = default;
  ~Cable() = default;

  void Compute(const mjModel* m, mjData* d, int instance);

  int i0;                         // index of first body
  int n;                          // number of bodies in the cable
  std::vector<int> prev;          // indices of previous bodies   (n x 1)
  std::vector<int> next;          // indices of next bodies       (n x 1)

  
  //element depending stiffness constants
  struct stiffness_ {      
      mjtNum L_Dyz;      // =L/Dyz  L:= beam Length, Dyz: distance to outermost Edge of Beam on yz plane
      mjtNum L_Dy;       // =L/Dy  Dy: distance to outermost Edge of Beam in y direction
      mjtNum L_Dz;       // =L/Dz  Dz: distance to outermost Edge of Beam in z direction
      mjtNum J_Dyz;      // =J/Dyz  
      mjtNum Iy_Dy;      // =Iy/Dy
      mjtNum Iz_Dz;      // =Iz/Dz
      mjtNum dOmega_Yield[3]; // =-L_Dxx / (1 + 1 / YieldStrain_[X]); //omega (beam angle) equivalent yields
      mjtNum Dyz;        // 
      mjtNum Dy;         // 
      mjtNum Dz;         //     
  };
  //material constants
  struct stiffness_consts_ {
      mjtNum E;            //youngs modulus
      mjtNum G;            //torsional modulus            
      mjtNum k_eps_UtY_G;  // =k / (UltimateStrain - YieldStrain_G) k: exponential constant factor 
      mjtNum k_eps_UtY_E;  // =k / (UltimateStrain - YieldStrain_E) k: exponential constant factor 
      mjtNum sig_UtY;      // =UltimateStress-YieldStress            
      mjtNum sigY;         // =Yieldstress      
      mjtNum epsY_G;       // =YieldStrain torsion
      mjtNum epsY_E;       // =YieldStrain bending
      mjtNum Df;           // internal damping factor
  };
  
  std::vector<mjtNum> omega0;     // reference curvature          (n x 3)
  std::vector<mjtNum> userdata;   // export of userdata from plugin  (n x 3) e.g. stress for colorcoded vis
  mjtNum userdatamin;             //storage of min values of userdata
  mjtNum userdatamax;             //storage of max values of userdata
  std::vector<stiffness_> Sel;    
  stiffness_consts_ Sconst;

 private:
  Cable(const mjModel* m, mjData* d, int instance);
};

}  // namespace mujoco::plugin::elasticity

#endif  // MUJOCO_SRC_PLUGIN_ELASTICITY_CABLE_H_
