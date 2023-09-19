#include <axis.h>
#include <utils.h>
#include <scene.h>
#include <object_data.h>


ArrowMesh::ArrowMesh(float head_radius, float body_radius, float head_height, float body_length, int n) : RenderData([=](){
   
  float tan = head_radius / head_height;

  std::vector<Vertex> points;

  vec3 pos, norm;

  // arrow head vertices
  for (int i = 0; i < n; i++) {

    // point at the top
    pos = vec3(0, 0, body_length);
    norm = vec3(
      tan * cos(2 * PI * (i+0.5) / n),
      tan * sin(2 * PI * (i+0.5) / n),
      1
    );      points.push_back({pos, norm});
      
    // point on the circle
    pos = vec3(
      head_radius * cos(2 * PI * i / n),
      head_radius * sin(2 * PI * i / n),
      body_length - head_height
    );
    norm = vec3(
      tan * cos(2 * PI * i / n),
      tan * sin(2 * PI * i / n),
      1
    );
    points.push_back({pos, norm});
      
    // point on the circle
    pos = vec3(
      head_radius * cos(2 * PI * (i + 1) / n),
      head_radius * sin(2 * PI * (i + 1) / n),
      body_length - head_height
    );
    norm = vec3(
      tan * cos(2 * PI * (i + 1) / n),
      tan * sin(2 * PI * (i + 1) / n),
      1
    );
    points.push_back({pos, norm});

  }

  // arrow head bottom vertices
  for(int i = 0; i < n; i++) {

    // point at the center
    pos = vec3(0, 0, body_length - head_height);
    norm = vec3(0, 0, -1);
    points.push_back({pos, norm});
      
    // point on the circle
    pos = vec3(
      head_radius * cos(2 * PI * (i + 1) / n),
      head_radius * sin(2 * PI * (i + 1) / n),
      body_length - head_height
    );
    points.push_back({pos, norm});
      
    // point on the circle
    pos = vec3(
      head_radius * cos(2 * PI * i / n),
      head_radius * sin(2 * PI * i / n),
      body_length - head_height
    );
    points.push_back({pos, norm});
  }

  // arrow body vertices
  for(int i = 0; i < n; i++) {

    // point at the top
    pos = vec3(
      body_radius * cos(2 * PI * (i) / n),
      body_radius * sin(2 * PI * (i) / n),
      body_length - head_height
    );
    norm = vec3(
      cos(2 * PI * (i) / n),
      sin(2 * PI * (i) / n),
      0
    );
    points.push_back({pos, norm});

    // point at the bottom
    pos = vec3(
      body_radius * cos(2 * PI * (i) / n),
      body_radius * sin(2 * PI * (i) / n),
      0
    );
    points.push_back({pos, norm});

    // point at the top
    pos = vec3(
      body_radius * cos(2 * PI * (i + 1) / n),
      body_radius * sin(2 * PI * (i + 1) / n),
      body_length - head_height
    );
    norm = vec3(
      cos(2 * PI * (i + 1) / n),
      sin(2 * PI * (i + 1) / n),
      0
    );
    points.push_back({pos, norm});

    // point at the bottom
    pos = vec3(
      body_radius * cos(2 * PI * (i+1) / n),
      body_radius * sin(2 * PI * (i+1) / n),
      0
    );
    points.push_back({pos, norm});

    // point at the top
    pos = vec3(
      body_radius * cos(2 * PI * (i + 1) / n),
      body_radius * sin(2 * PI * (i + 1) / n),
      body_length - head_height
    );

    points.push_back({pos, norm});

    // point at the bottom
    pos = vec3(
      body_radius * cos(2 * PI * (i) / n),
      body_radius * sin(2 * PI * (i) / n),
      0
    );
    norm = vec3(
      cos(2 * PI * (i) / n),
      sin(2 * PI * (i) / n),
      0
    );
    points.push_back({pos, norm});

  }

  return points;

}()){}


SphereMesh::SphereMesh(float radius, int n) : RenderData([=](){

  std::vector<Vertex> points;

  const auto gen = [=](int i, int j) {
    const auto pos = radius*vec3{
      cos(2*PI*i/n) * sin(PI*j/n),
      sin(2*PI*i/n) * sin(PI*j/n),
      cos(PI*j/n)
    };
    return Vertex{pos, pos};
  };

  for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
      points.push_back(gen(i, j));
      points.push_back(gen(i, j + 1));
      points.push_back(gen(i + 1, j));
      points.push_back(gen(i + 1, j + 1));
      points.push_back(gen(i + 1, j));
      points.push_back(gen(i, j + 1));
    }
  }

  return points;

}()){}

const std::function<vec3(float, float)> Arrow::mesh_generator {[](float u, float v) {
  vec3 pos;
  const double body_length = 0.8;
  const double head_height = 0.2;
  const double head_radius = 0.1;
  const double body_radius = 0.05;
  if (v < body_length) {
    pos = vec3(
      body_radius * cos(2 * PI * u),
      body_radius * sin(2 * PI * u),
      v
    );
  } else {
    pos = vec3(
      head_radius * cos(2 * PI * u) * (1 - (v - body_length) / head_height),
      head_radius * sin(2 * PI * u) * (1 - (v - body_length) / head_height),
      v
    );
  }
  return pos;
}};

const RenderData Arrow::mesh{mesh_generator};

vec3 Arrow::ArrowData::get_pos(vec2 uv) const {
  return Affine::matrix()*vec4(mesh_generator(uv.x, uv.y),0);
}

void Arrow::ArrowData::ondrag(unsigned x, unsigned y, vec2 uv) {
  const auto diff = get_pos(uv) - parent.last_drag;
  set_displacement(get_displacement()+diff);
  // parent.last_drag+=diff;
  std::cout << "diff: " << diff << std::endl;
}

void Arrow::ArrowData::mousedown(int button, int mode, vec2 uv) {
  parent.last_drag = get_pos(uv);

  std::cout << "pos: " << get_pos(uv) << std::endl;
}

void Arrow::ArrowData::mouseup(int button, int mode, vec2 uv) {
  // parent.last_drag = get_pos(uv);
  // set_displacement(get_displacement()+diff);
}




void Arrow::init(Scene &scene) {
  scene.add(mesh_object);
}

Arrow::Arrow(vec3 start, vec3 end) : start{start}, end{end} {
  __update__();
}

void Arrow::__update__() {
  auto dir = end - start;
  auto length = glm::length(dir);
  dir = normalize(dir);
  mesh_object->set_displacement(start);
  mesh_object->set_scale(1, 1, length);
  mesh_object->set_rotation(atan2(dir.x, dir.z), -asin(dir.y) , 0);
  mesh_object->set_color(color);
  mesh_object->set_visibility(visibility);
}

Arrow::ArrowData::ArrowData(Arrow& arrow)
  : ObjectData{mesh}, parent(arrow) {}


Sphere::Sphere(vec3 center, float radius) : ObjectData{mesh}, center{center}, radius{radius} {
  set_displacement(center);
  set_scale(radius);
}

const SphereMesh Sphere::mesh;



// AxesObject::Axis::Axis(AxesObject* parent, vec3 dir) : Arrow{vec3(0), dir} {}


// void AxesObject::Axis::ondrag(unsigned,unsigned,int) {}

void AxesObject::init(Scene& scene) {}

void AxesObject::__update__() {

}