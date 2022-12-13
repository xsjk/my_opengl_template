#include <cloth.h>
#include <time_system.h>

// RectCloth::RectCloth() 

RectCloth::RectCloth(float cloth_weight,
                     const glm::uvec2& mass_dim,
                     float dx,
                     float stiffness, float damping_ratio) :
    // Mesh(std::vector<MeshVertex>(mass_dim.x * mass_dim.y),
    //      std::vector<glm::uvec3>((mass_dim.y - 1) * (mass_dim.x - 1) * 2),
    //      GL_STREAM_DRAW, GL_STATIC_DRAW,
    //      true),
    // private
    mass_dim(mass_dim),
    n_mass(mass_dim.x * mass_dim.y),
    mass_weight(cloth_weight / float(n_mass)),
    dx(dx),
    stiffness(stiffness),
    damping_ratio(damping_ratio),
    is_fixed_masses(n_mass),
    world_velocities(n_mass),
    world_accelerations(n_mass) 
{
      
  grid_VBO->data.reserve(n_mass);
  grid_face_EBO->data.reserve((mass_dim.x - 1) * (mass_dim.y - 1) * 6);
  grid_line_EBO->data.reserve(mass_dim.x * (mass_dim.y - 1) + (mass_dim.x - 1) * mass_dim.y);

  const auto local_width = mass_dim.x * dx;
  const auto local_height = mass_dim.y * dx;

  for (int ih = 0; ih < mass_dim.y; ++ih)
    for (int iw = 0; iw < mass_dim.x; ++iw)
      grid_VBO->data.push_back({ 
        vec3(
          iw * dx - local_width * 0.5, 
          ih * dx - local_height * 0.5,
          0
        ), 
        vec3(0,0,1), 
        vec2(iw / float(mass_dim.x), ih / float(mass_dim.y))
      });
  
  for (int ih = 0; ih < mass_dim.y - 1; ++ih)
    for (int iw = 0; iw < mass_dim.x - 1; ++iw) {
      unsigned index[4] = {
        get_index(iw, ih),
        get_index(iw+1, ih),
        get_index(iw+1, ih+1),
        get_index(iw, ih+1)
      };
      grid_face_EBO->data.push_back(index[0]);
      grid_face_EBO->data.push_back(index[1]);
      grid_face_EBO->data.push_back(index[2]);
      grid_face_EBO->data.push_back(index[3]);
      grid_line_EBO->data.push_back(index[0]);
      grid_line_EBO->data.push_back(index[1]);
      grid_line_EBO->data.push_back(index[0]);
      grid_line_EBO->data.push_back(index[3]);
    }
  // last row

  
  grid_face->set_buffer_mode(GL_STREAM_DRAW);
}



void RectCloth::update_pos() {
  

  for (auto _=0; _<simulation_steps_per_fixed_update_time * Time::fixed_update_times_this_frame; ++_) {
    // update acceleration
    #pragma omp parallel for collapse(2)
    for (auto ih=0; ih<mass_dim.y; ++ih)
      for(auto iw=0; iw<mass_dim.x; ++iw) {
        auto i = get_index(iw, ih);
        auto pos = get_pos(i);
        auto norm = get_normal(i);
        if (is_fixed_masses[i] || i==mousehold) continue;
        constexpr auto gaussian = [](float x, float y) { return exp2f(-x*x-y*y); };
        const vec3 wind = gaussian(pos.x, pos.y) * vec3{0,0,5};
        const vec3 air_resistance = wind -  world_velocities[i];
        world_accelerations[i] = 
            vec3{0,-9.8,0}  // gravity
          + get_force(ivec2(iw, ih)) / dm * damping_ratio // elastic force
          + 2e-3f * glm::dot(air_resistance, norm) * norm * get_dS(iw, ih) / dm; // air
          ;
      }

    #pragma omp parallel for
    for (auto i=0; i<n_mass; ++i) {
      world_velocities[i] += world_accelerations[i] * dt;  
      vec3 new_pos = get_pos(i) + world_velocities[i] * dt;   

      // check collision with ground
      const float ground_height = -2.3 + dx;
      if (new_pos.y < ground_height) {
        new_pos.y = ground_height;
        // friction
        world_velocities[i].y *= -0.5;
        world_velocities[i].x *= 0.9;
        world_velocities[i].z *= 0.9;
      }

      // check collision with the cube
      constexpr vec3 cube_center = {-3.5, -1.8, 0.3};
      const float cube_half_size = 0.5 + dx/2;
      auto cube_to_mass = new_pos - cube_center;
      auto abs_cube_to_mass = glm::abs(cube_to_mass);
      if ( abs_cube_to_mass.x < cube_half_size
        && abs_cube_to_mass.y < cube_half_size
        && abs_cube_to_mass.z < cube_half_size) 
      {
        unsigned axis[3] = {0,1,2};
        std::sort(axis, axis+3, [&abs_cube_to_mass](unsigned a, unsigned b) { return abs_cube_to_mass[a] < abs_cube_to_mass[b]; });
        new_pos[axis[2]] = cube_center[axis[2]] + cube_half_size * glm::sign(cube_to_mass[axis[2]]);
        world_velocities[i][axis[2]] *= -0.5;
        world_velocities[i][axis[1]] *= 0.9;
        world_velocities[i][axis[0]] *= 0.9;
      }

      // check collision with sphere
      const float sphere_radius = 0.5 + EPSILON;
      constexpr vec3 sphere_center = {3.5, -1.8, 0.3};
      auto sphere_to_mass = new_pos - sphere_center;
      if (glm::length(sphere_to_mass) < sphere_radius) {
        auto sphere_to_mass_normalized = glm::normalize(sphere_to_mass);
        new_pos = sphere_center + sphere_to_mass_normalized * sphere_radius;
        // friction
        world_velocities[i] -= glm::dot(world_velocities[i], sphere_to_mass_normalized) * sphere_to_mass_normalized;
        world_velocities[i] *= 0.1;
      }

      get_pos(i) = new_pos;

    }
  }

}

void RectCloth::update_normal() {
  // recalculate normals for each vertex
  const static auto compute_normal = [&](auto v1, auto v2, auto v3) {
    return glm::normalize(glm::cross(get_pos(v2) - get_pos(v1), get_pos(v3) - get_pos(v1)));
  };
  #pragma omp parallel for collapse(2)
  for (int ih = 0; ih < mass_dim.y; ++ih)
    for (int iw = 0; iw < mass_dim.x; ++iw) {
      constexpr float w_small = .125;
      constexpr float w_large = .25;
      auto i  = get_index(iw, ih);
      auto l  = get_index(iw - 1, ih);
      auto r  = get_index(iw + 1, ih);
      auto u  = get_index(iw, ih + 1);
      auto d  = get_index(iw, ih - 1);
      auto lu = get_index(iw - 1, ih + 1);
      auto rd = get_index(iw + 1, ih - 1);
      auto& pos = get_pos(i);
      auto& normal = get_normal(i);

      normal = { 0, 0, 0 };

      if (iw > 0 && ih < mass_dim.y - 1) {
        normal += compute_normal(l, i, lu) * w_small;
        normal += compute_normal(i, u, lu) * w_small;
      }
      if (iw < mass_dim.x - 1 && ih < mass_dim.y - 1) {
        normal += compute_normal(i, r, u) * w_large;
      }
      if (iw > 0 && ih > 0) {
        normal += compute_normal(l, d, i) * w_large;
      }
      if (iw < mass_dim.x - 1 && ih > 0) {
        normal += compute_normal(d, rd, i) * w_small;
        normal += compute_normal(rd, r, i) * w_small;
      }

      normal = -glm::normalize(normal);
    }

}


void RectCloth::KeyboardCallback(int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch(key) {
      case GLFW_KEY_P:
        paused = !paused;
        break;
      case GLFW_KEY_1:
        if (display_mode & POINTS) {
          hide(grid_points);
          display_mode &= ~POINTS;
        } else {
          show(grid_points);
          display_mode |= POINTS;
        }
        break;
      case GLFW_KEY_2:
        if (display_mode & LINES) {
          hide(grid_line);
          display_mode &= ~LINES;
        } else {
          show(grid_line);
          display_mode |= LINES;
        }
        break;

      case GLFW_KEY_3:
        if (display_mode & FACES) {
          hide(grid_face);
          display_mode &= ~FACES;
        } else {
          show(grid_face);
          display_mode |= FACES;
        }
        break;
    }
    
  }
}



void RectCloth::hide(Handler<ObjectData> obj) {
  obj->set_visibility(false);
}

void RectCloth::show(Handler<ObjectData> obj) {
  obj->set_visibility(true);
}

RectCloth::Faces::Faces(RectCloth& p) :
  ObjectData{RenderData{p.grid_VBO, p.grid_face_EBO, GL_QUADS}},
  parent(p) 
{
  
}

// void RectCloth::Faces::mouseover(vec2 uv) {
//   std::cout << "mouse over mass grid at " << uv << std::endl;
// }

// void RectCloth::Faces::mouseout(vec2 uv) {
//   std::cout << "mouse out mass grid at " << uv << std::endl;
// }

    

RectCloth::Points::Points(RectCloth& p) :
  ObjectData{RenderData{p.grid_VBO, p.fixed_points_EBO, GL_POINTS}},
  parent(p) 
{

  
}

void RectCloth::Faces::ondrag(unsigned x, unsigned y, vec2 uv) {
  auto pointID = parent.mousehold;
  std::cout << "dragging mass grid at " << uv << " with pointID " << pointID << std::endl;
  if (pointID == -1) return;
  parent.reset(parent.mousehold);
  if (scene) {
    auto& display = scene->display;
    auto& old_world_coordinate = parent.get_pos(pointID);
    auto old_camera_coordinate = display.camera.world_to_camera(old_world_coordinate);
    auto new_screen_coordinate = vec3(x, y, old_camera_coordinate.z);
    auto new_camera_coordinate = display.screen_to_camera(new_screen_coordinate);
    auto new_world_coordinate = display.camera.camera_to_world({new_camera_coordinate, old_camera_coordinate.z});
    old_world_coordinate = new_world_coordinate;
  } else {
    std::cout << "no scene" << std::endl;
  }
}

void RectCloth::Faces::mouseover(vec2 uv) {
  auto pointID = parent.get_index(uv);
}

void RectCloth::Faces::mouseout(vec2 uv) {
  if(parent.mousehold == -1) return;
  parent.reset(parent.mousehold);
  parent.mousehold = -1;
}

void RectCloth::Faces::mousedown(int, int, vec2 uv) {
  parent.mousehold = parent.get_index(uv);
  if (parent.mousehold == -1) return;
  parent.reset(parent.mousehold);
}

void RectCloth::Faces::mouseup(int, int, vec2) {
  std::cout << "mouseup\n" ;
  if (parent.mousehold == -1) return;
  parent.reset(parent.mousehold);
  parent.mousehold = -1;
}

void RectCloth::Points::ondrag(unsigned x, unsigned y, vec2 uv) {
  auto pointID = parent.mousehold;
  std::cout << "dragging mass grid at " << uv << " with pointID " << pointID << std::endl;
  if (pointID == -1) return;
  parent.reset(parent.mousehold);
  if (scene) {
    auto& display = scene->display;
    auto& old_world_coordinate = parent.get_pos(pointID);
    auto old_camera_coordinate = display.camera.world_to_camera(old_world_coordinate);
    auto new_screen_coordinate = vec3(x, y, old_camera_coordinate.z);
    auto new_camera_coordinate = display.screen_to_camera(new_screen_coordinate);
    auto new_world_coordinate = display.camera.camera_to_world({new_camera_coordinate, old_camera_coordinate.z});
    old_world_coordinate = new_world_coordinate;
  } else {
    std::cout << "no scene" << std::endl;
  }
}

void RectCloth::Points::mousedown(int, int, vec2 uv) {
  parent.mousehold = parent.get_index(uv);
  if (parent.mousehold == -1) return;
}

void RectCloth::Points::mouseup(int, int, vec2 uv) {
  parent.mousehold = -1;
}

void RectCloth::Points::mouseover(vec2 uv) {
  std::cout << "uv " << uv << std::endl;
  set_color(1,0,0);
  set_point_size(20);
}

void RectCloth::Points::mouseout(vec2 uv) {
  std::cout << "uv " << uv << std::endl;
  set_color(0);
  set_point_size(10);
}

RectCloth::Lines::Lines(RectCloth& p):
  ObjectData{RenderData{p.grid_VBO, p.grid_line_EBO, GL_LINES}},
  parent(p) 
{

}