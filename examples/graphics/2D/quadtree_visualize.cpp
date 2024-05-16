#include <fan/pch.h>

#include <fan/trees/quad_tree.h>

int main() {
  fan::trees::quadtree_t qt(0.5, 0.5, 1);
  static constexpr f32_t n = 1000;
  for (int i = 0; i < n; ++i) {
    fan::vec2 p = fan::random::vec2(0, 1);
    fan::vec2 a, b;
    qt.insert(p, a, b);
  }

  loco_t loco;
  loco.get_context().camera_set_ortho(
    loco.orthographic_camera.camera,
    fan::vec2(-1, 1),
    fan::vec2(-1, 1)
  );

  std::vector<fan::graphics::rectangle_t> rectangles;
  std::vector<fan::graphics::rectangle_t> points;

  fan::trees::quadtree_t* qtp = &qt;

  int index = 0;
  fan::function_t<void(fan::trees::quadtree_t* qtp)> l;
  l = [&l, &rectangles, &index, &points](fan::trees::quadtree_t* qtp) -> void {
    // put this inside else divided 
    rectangles.push_back(fan::graphics::rectangle_t{ {
      .position = fan::vec3(qtp->position * 2 - 1, index++),
      .size = qtp->boundary * 2, // because coordinate is -1 -> 1 so * 2 when viewport size is 0-1
      .color = fan::random::color() - fan::vec4(0, 0, 0, 0.5)
      /*fan::color::hsv((float)index / n * 360.f, 100, 100)*/,
    .blending = true
  } });
    if (qtp->divided) {
      l(qtp->north_west);
      l(qtp->north_east);
      l(qtp->south_west);
      l(qtp->south_east);
    }
    for (auto& i : qtp->points) {
      points.push_back(fan::graphics::rectangle_t{ {
          .position = fan::vec3(i * 2 - 1, index),
          .size = fan::vec2(0.003), // because coordinate is -1 -> 1 so * 2 when viewport size is 0-1
          .color = fan::colors::white
          /*fan::color::hsv((float)index / n * 360.f, 100, 100)*/,
        .blending = true
      } });
    }
    };

  l(qtp);

  loco.loop([&] {

  });

  return 0;
}
