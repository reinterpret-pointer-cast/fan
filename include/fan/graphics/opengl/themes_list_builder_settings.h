#define BLL_set_BaseLibrary 1
#define BLL_set_namespace fan::opengl
#define BLL_set_prefix theme_list
#define BLL_set_type_node uint8_t
#define BLL_set_node_data fan_2d::graphics::gui::theme_t* theme_id;
#define BLL_set_Link 0
#define BLL_set_AreWeInsideStruct 1
#define BLL_set_NodeReference_Overload_Declare \
  theme_list_NodeReference_t() = default; \
  theme_list_NodeReference_t(fan_2d::graphics::gui::theme_t*);