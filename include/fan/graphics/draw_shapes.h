//#if defined(loco_light)
//light.draw();
//#endif
//
//#if defined(loco_light_sun)
//light_sun.draw();
//#endif
//
//#if defined(loco_rectangle)
//  rectangle.draw();
//#endif
//#if defined(loco_circle)
//  circle.draw();
//#endif
//#if defined(loco_pixel_format_renderer)
//  pixel_format_renderer.draw();
//#endif
//#if defined(loco_sprite)
//  // can be moved
//  sprite.draw();
//#endif
//  #if defined(loco_line)
//  line.draw();
//  #endif
//#if defined(loco_letter)
//  // loco_t::text gets drawn here as well as it uses letter
//  letter.draw();
//#endif
//#if defined(loco_text_box)
//  text_box.draw();
//#endif
//#if defined(loco_button)
//  button.draw();
//#endif
//#if defined(loco_model_3d)
//  model.draw();
//#endif
//#if defined(loco_post_process)
//  post_process.draw();
//#endif
//
//
//for (auto it = m_draw_queue.begin(); it != m_draw_queue.end(); ++it) {
//  for (const auto& f : it->f) {
//    f();
//  }
//}

m_draw_queue.clear();