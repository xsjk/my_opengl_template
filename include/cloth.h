#ifndef _CLOTH_H
#define _CLOTH_H

#include <defines.h>
#include <object_data.h>

#include <sampler.hpp>
#include <window.h>
#include <scene.h>
#include <group.h>
#include <time_system.h>

class RectCloth : public Group {


  static constexpr unsigned simulation_steps_per_fixed_update_time = 20;

  bool paused = true;

  int mousehold = -1;



  struct Points : public ObjectData {
    RectCloth& parent;
    unsigned i;
    Points(RectCloth&);
    virtual void ondrag(unsigned, unsigned, vec2) override;
    virtual void mousedown(int,int,vec2) override;
    virtual void mouseup(int,int,vec2) override;
    virtual void mouseover(vec2) override;
    virtual void mouseout(vec2) override;
    virtual void ondblclick(unsigned, unsigned, vec2) override;
  };

  struct Faces : public ObjectData {
    RectCloth& parent;
    Faces(RectCloth&);
    virtual void ondrag(unsigned, unsigned, vec2) override;
    virtual void mouseover(vec2) override;
    virtual void mouseout(vec2) override;
    virtual void mousedown(int,int,vec2) override;
    virtual void mouseup(int,int,vec2) override;
    virtual void ondblclick(unsigned, unsigned, vec2) override;
  };

  struct Lines : public ObjectData {
    RectCloth& parent;
    Lines(RectCloth&);
  };

  uvec2 mass_dim;
  unsigned n_mass;
  float mass_weight;


  float dx;
  float dm = mass_weight / n_mass;
  static constexpr float dt = Time::fixed_delta_time / simulation_steps_per_fixed_update_time;

  float stiffness;
  float damping_ratio;


  vec3 get_wind(const vec3& pos);

  Handler<VertexBufferObject> grid_VBO;
  Handler<ElementBufferObject> fixed_points_EBO;
  Handler<ElementBufferObject> grid_face_EBO;
  Handler<ElementBufferObject> grid_line_EBO;
  Handler<Faces> grid_face{*this};
  Handler<Lines> grid_line{*this};
  Handler<Points> grid_points{*this};

  std::vector<bool> is_fixed_masses;
  std::vector<vec3> world_velocities;
  std::vector<vec3> world_accelerations;




  /// @brief display mode
  /// @details 
  ///   - 1: show mesh points
  ///   - 2: show mesh grid,
  ///   - 4: show mesh faces,

  enum {
    POINTS = 1,
    LINES = 2,
    FACES = 4,
  };
  int display_mode = 4;

  friend Points;
  friend Faces;
  friend Lines;


  public:
    RectCloth(
      float cloth_weight,
      const glm::uvec2 & mass_dim,
      float dx,
      float stiffness, float damping_ratio
    );

    inline void reset(int pointID) { 
       world_velocities.at(pointID)
     = world_accelerations.at(pointID)
     = vec3(0);
    }

    inline void reset() {
      for (unsigned i = 0; i < n_mass; ++i) reset(i);
    }

    
    inline auto get_index(unsigned iw, unsigned ih) const { return ih * mass_dim.x + iw; }
    inline auto get_index(int iw, int ih) const { return get_index(unsigned(iw < 0 ? mass_dim.x + iw : iw), unsigned(ih < 0 ? mass_dim.y + ih : ih)); }
    inline auto get_index(ivec2 uv) const { return get_index(uv.x, uv.y); }
    inline auto get_index(float u, float v) const { return get_index(int(u*mass_dim.x+.5), int(v*mass_dim.y+.5)); }
    inline auto get_index(vec2 uv) const { return get_index(uv.x, uv.y); }
    inline auto& get_pos(int i) const { return grid_VBO->data[i].position; }
    inline auto& get_pos(int iw, int ih) const { return get_pos(get_index(iw, ih)); }
    inline auto& get_pos(ivec2 i) const { return get_pos(i.x, i.y); }
    inline auto& get_normal(int i) const { return grid_VBO->data[i].normal; }
    inline auto& get_normal(int iw, int ih) const { return get_normal(get_index(iw, ih)); }
    inline auto in_range(int iw, int ih) const { return 0 <= iw && iw < mass_dim.x && 0 <= ih && ih < mass_dim.y; }
    inline auto in_range(ivec2 i) const { return in_range(i.x, i.y); }


    inline float get_dS(int iw, int ih) const {
      vec3 p0 = get_pos(get_index(iw, ih)), p1, p2;
      if (iw == mass_dim.x - 1) p1 = get_pos(get_index(iw - 1, ih));
      else p1 = get_pos(get_index(iw + 1, ih));
      if (ih == mass_dim.y - 1) p2 = get_pos(get_index(iw, ih - 1));
      else p2 = get_pos(get_index(iw, ih + 1));
      return glm::length(glm::cross(p1 - p0, p2 - p0));
    }

    inline void init(Scene& s) override {
      s.add(grid_face);
      s.add(grid_line);
      s.add(grid_points);
      hide(grid_line);
      hide(grid_points);
    }

    inline void set_color(float r, float g, float b) {
      grid_face->set_color(r, g, b);
    }
    
    inline void set_fixed_mass(unsigned id) {
      is_fixed_masses[id] = true;
      if (std::find(fixed_points_EBO->data.begin(), fixed_points_EBO->data.end(), id) == fixed_points_EBO->data.end())
        fixed_points_EBO->data.push_back(id);
      world_accelerations[id] = vec3(0);
      world_velocities[id] = vec3(0);
    }

    inline void remove_fixed_mass(unsigned id) {
      is_fixed_masses[id] = false;
      auto it = std::find(fixed_points_EBO->data.begin(), fixed_points_EBO->data.end(), id);
      if (it != fixed_points_EBO->data.end()) fixed_points_EBO->data.erase(it);
    }

    inline void set_fixed_masses(const std::vector<ivec2>& fixed_masses) {
      for (const auto& m : fixed_masses) {
        int iw = m.x < 0 ? int(mass_dim.x) + m.x : m.x;
        int ih = m.y < 0 ? int(mass_dim.y) + m.y : m.y;
        std::cout << "fixed mass: " << iw << ", " << ih << std::endl;
        set_fixed_mass(get_index(iw, ih));
      }
    }


    inline void apply(const Affine &affine) {
      grid_face->get_data().apply(affine);
      dx *= affine.get_scale().x;
      update_normal();
    }

    inline void update() override {
      if (paused) return;
      update_pos();
      update_normal();
    }
    
    
    void update_pos();

    void update_normal();

    inline auto get_force(ivec2 from_index, ivec2 to_index, float origin_distance) const {
      if (in_range(from_index) && in_range(to_index)) {
        const auto from_position = get_pos(from_index);
        const auto to_postion = get_pos(to_index);
        const auto direction = from_position - to_postion;
        const auto new_distance = length(direction);
        const auto delta_distance = new_distance - origin_distance;
        return stiffness * delta_distance * direction / new_distance;
      }
      return vec3(0);
    }

    inline auto get_force(ivec2 index) const {
      constexpr auto sqrt2 = 1.4142135623730950488016887242097f;
      return get_force(index+ivec2(-2, 0), index, 2*dx)
            +get_force(index+ivec2(0, -2), index, 2*dx)
            +get_force(index+ivec2(2,  0), index, 2*dx)
            +get_force(index+ivec2(0,  2), index, 2*dx)
            +get_force(index+ivec2(-1, 0), index,   dx)
            +get_force(index+ivec2(0, -1), index,   dx)
            +get_force(index+ivec2(1,  0), index,   dx)
            +get_force(index+ivec2(0,  1), index,   dx)
            +get_force(index+ivec2(-1,-1), index, sqrt2*dx)
            +get_force(index+ivec2( 1,-1), index, sqrt2*dx)
            +get_force(index+ivec2(-1, 1), index, sqrt2*dx)
            +get_force(index+ivec2( 1, 1), index, sqrt2*dx);
    }

    void KeyboardCallback(int key, int scancode, int action, int mods) override;


    void hide(Handler<ObjectData> obj);
    void show(Handler<ObjectData> obj);
};

#endif