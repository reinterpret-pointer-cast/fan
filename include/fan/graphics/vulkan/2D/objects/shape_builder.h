#ifndef sb_get_loco
  #define sb_get_loco \
  loco_t* get_loco() { \
    loco_t* loco = OFFSETLESS(this, loco_t, sb_shape_var_name); \
    return loco; \
  }
#endif

sb_get_loco

#ifndef sb_vertex_count
  #define sb_vertex_count 6
#endif

void sb_open() {
  loco_t* loco = get_loco();
  auto context = loco->get_context();
  root = loco_bdbt_NewNode(&loco->bdbt);
  blocks.Open();
  bm_list.Open();

  m_shader.open(context);
  m_shader.set_vertex(context, sb_shader_vertex_path);
  m_shader.set_fragment(context, sb_shader_fragment_path);

  dsl_properties.write_descriptor_sets[0].binding = 0;
  dsl_properties.write_descriptor_sets[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  dsl_properties.write_descriptor_sets[0].flags = VK_SHADER_STAGE_VERTEX_BIT;

  dsl_properties.write_descriptor_sets[1].binding = 1;
  dsl_properties.write_descriptor_sets[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  dsl_properties.write_descriptor_sets[1].flags = VK_SHADER_STAGE_VERTEX_BIT;

  descriptor_layout_nr = context->descriptor_sets.push_layout(context, dsl_properties);

  fan::vulkan::pipelines_t::properties_t p;
  p.descriptor_set_layout = &context->descriptor_sets.get(descriptor_layout_nr).layout;
  p.shader = &m_shader;
  pipeline_nr = context->pipelines.push(context, p);
}
void sb_close() {
  loco_t* loco = get_loco();

  blocks.Close();
  bm_list.Close();

  assert(0);
  //loco_bdbt_close(&loco->bdbt);

  m_shader.close(loco->get_context(), &loco->m_write_queue);

  assert(0);
  //vkDestroyDescriptorSetLayout(loco->get_context()->device, descriptorSetLayout, nullptr);

  //for (uint32_t i = 0; i < blocks.size(); i++) {
  //  blocks[i].uniform_buffer.close(loco->get_context());
  //}
}

struct block_t;

// STRUCT MANUAL PADDING IS REQUIRED (32 BIT)
block_t* sb_push_back(fan::graphics::cid_t* cid, properties_t& p) {
  loco_t* loco = get_loco();
 
  loco_bdbt_NodeReference_t nr = root;
  loco_bdbt_Key_t<sizeof(instance_properties_t::key_t) * 8> k;
  typename decltype(k)::KeySize_t ki;
  k.Query(&loco->bdbt, &p.key, &ki, &nr);
  if (ki != sizeof(instance_properties_t::key_t) * 8) {
    auto lnr = bm_list.NewNode();
    auto ln = &bm_list[lnr];
    ln->first_block = blocks.NewNodeLast();
    blocks[ln->first_block].block.open(loco, this);
    // HERE
    ln->last_block = ln->first_block;
    ln->instance_properties = *(instance_properties_t*)&p;
    k.InFrom(&loco->bdbt, &p.key, ki, nr, lnr.NRI);
    nr = lnr.NRI;
  }

  instance_t it = p;
  shape_bm_Node_t* bmn = bm_list.GetNodeByReference(*(shape_bm_NodeReference_t*)&nr);
  block_t* last_block = &blocks[bmn->data.last_block].block;

  if (last_block->uniform_buffer.size() == max_instance_size) {
    auto nnr = blocks.NewNode();
    blocks.linkNext(bmn->data.last_block, nnr);
    bmn->data.last_block = nnr;
    last_block = &blocks[bmn->data.last_block].block;
    last_block->open(loco, this);
  }
  block_t* block = last_block;
  block->uniform_buffer.push_ram_instance(loco->get_context(), it);

  const uint32_t instance_id = block->uniform_buffer.size() - 1;

  block->cid[instance_id] = cid;

  block->uniform_buffer.common.edit(
    loco->get_context(),
    &loco->m_write_queue,
    instance_id * sizeof(instance_t),
    instance_id * sizeof(instance_t) + sizeof(instance_t)
  );

  cid->bm_id = ((shape_bm_NodeReference_t*)&nr)->NRI;
  cid->block_id = bmn->data.last_block.NRI;
  cid->instance_id = instance_id;

  block->p[instance_id] = *(instance_properties_t*)&p;
  return block;
}
void sb_erase(fan::graphics::cid_t* cid) {
  loco_t* loco = get_loco();
  auto bm_id = *(shape_bm_NodeReference_t*)&cid->bm_id;
  auto bm_node = bm_list.GetNodeByReference(bm_id);

  auto block_id = *(bll_block_NodeReference_t*)&cid->block_id;
  auto block_node = blocks.GetNodeByReference(*(bll_block_NodeReference_t*)&cid->block_id);
  auto block = &block_node->data.block;

  auto& last_block_id = bm_node->data.last_block;
  auto* last_block_node = blocks.GetNodeByReference(last_block_id);
  block_t* last_block = &last_block_node->data.block;
  uint32_t last_instance_id = last_block->uniform_buffer.size() - 1;

  // erase descriptorset from block
  assert(0);
  if (block_id == last_block_id && cid->instance_id == block->uniform_buffer.size() - 1) {
    block->uniform_buffer.common.m_size -= sizeof(instance_t);
    if (block->uniform_buffer.size() == 0) {
      auto lpnr = block_node->PrevNodeReference;

      block->close(loco);
      blocks.Unlink(block_id);
      blocks.Recycle(block_id);
      if (last_block_id == bm_node->data.first_block) {
        loco_bdbt_Key_t<sizeof(instance_properties_t::key_t) * 8> k;
        typename decltype(k)::KeySize_t ki;
        k.Remove(&loco->bdbt, &bm_node->data.instance_properties.key, root);
        bm_list.Recycle(bm_id);
      }
      else {
        //fan::print("here");
        last_block_id = lpnr;
      }
    }
  //  fan::print(shape_bm_usage(&bm_list));
    return;
  }

  instance_t* last_instance_data = last_block->uniform_buffer.get_instance(loco->get_context(), last_instance_id);

  block->uniform_buffer.copy_instance(
    loco->get_context(),
    cid->instance_id,
    last_instance_data
  );

  last_block->uniform_buffer.common.m_size -= sizeof(instance_t);

  block->p[cid->instance_id] = last_block->p[last_instance_id];

  block->cid[cid->instance_id] = last_block->cid[last_instance_id];
  block->cid[cid->instance_id]->block_id = block_id.NRI;
  block->cid[cid->instance_id]->instance_id = cid->instance_id;

  if (last_block->uniform_buffer.size() == 0) {
    auto lpnr = last_block_node->PrevNodeReference;

    last_block->close(loco);
    blocks.Unlink(last_block_id);
    blocks.Recycle(last_block_id);

    bm_node->data.last_block = lpnr;
  }

  block->uniform_buffer.common.edit(
    loco->get_context(),
    &loco->m_write_queue,
    cid->instance_id * sizeof(instance_t),
    cid->instance_id * sizeof(instance_t) + sizeof(instance_t)
  );
}

block_t* sb_get_block(fan::graphics::cid_t* cid) {
  auto& block_node = blocks[*(bll_block_NodeReference_t*)&cid->block_id];
  return &block_node.block;
}

template <typename T>
T get(fan::graphics::cid_t *cid, T instance_t::*member) {
  loco_t* loco = get_loco();
  auto block = sb_get_block(cid);
  return block->uniform_buffer.get_instance(loco->get_context(), cid->instance_id)->*member;
}
template <typename T, typename T2>
void set(fan::graphics::cid_t *cid, T instance_t::*member, const T2& value) {
  loco_t* loco = get_loco();
  auto block = sb_get_block(cid);
  block->uniform_buffer.edit_instance(loco->get_context(), cid->instance_id, member, value);
  block->uniform_buffer.common.edit(
    loco->get_context(),
    &loco->m_write_queue,
    cid->instance_id * sizeof(instance_t) + fan::ofof(member),
    cid->instance_id * sizeof(instance_t) + fan::ofof(member) + sizeof(T)
  );
}

void set_vertex(const fan::string& str) {
  loco_t* loco = get_loco();
  m_shader.set_vertex(loco->get_context(), str);
}
void set_fragment(const fan::string& str) {
  loco_t* loco = get_loco();
  m_shader.set_fragment(loco->get_context(), str);
}

template <uint32_t depth = 0>
void traverse_draw(auto nr) {
  loco_t* loco = get_loco();
  if constexpr(depth == instance_properties_t::key_t::count + 1) {
    auto bmn = bm_list.GetNodeByReference(*(shape_bm_NodeReference_t*)&nr);
    auto bnr = bmn->data.first_block;

    while(1) {
      auto node = blocks.GetNodeByReference(bnr);

      loco->get_context()->draw(
        sb_vertex_count,
        node->data.block.uniform_buffer.size(),
        0,
        pipeline_nr,
        &loco->get_context()->descriptor_sets.descriptor_list[node->data.block.descriptor_set_nr].descriptor_set[loco->get_context()->currentFrame]
      );

      if (bnr == bmn->data.last_block) {
        break;
      }
      bnr = node->NextNodeReference;
    }
  }
  else {
    //loco_bdbt_Key_t<sizeof(typename instance_properties_t::key_t::get_type<depth>::type) * 8> k;
    typename loco_bdbt_Key_t<sizeof(typename instance_properties_t::key_t::get_type<depth>::type) * 8>::Traverse_t kt;
    kt.init(nr);
    typename instance_properties_t::key_t::get_type<depth>::type o;
#if fan_use_uninitialized == 0
    memset(&o, 0, sizeof(o));
#endif
    while(kt.Traverse(&loco->bdbt, &o)) {
      loco->process_block_properties_element(this, o);
      traverse_draw<depth + 1>(kt.Output);
    }
  }
}

void sb_draw(uint32_t draw_mode = 0) {
  traverse_draw(root);
}

template <uint32_t i>
void sb_set_key(fan::graphics::cid_t* cid, auto value) {
  loco_t* loco = get_loco();
  auto block = sb_get_block(cid);
  properties_t p;
  *(instance_t*)&p = *block->uniform_buffer.get_instance(loco->get_context(), cid->instance_id);
  *(instance_properties_t*)&p = block->p[cid->instance_id];
  *p.key.get_value<i>() = value;
  sb_erase(cid);
  sb_push_back(cid, p);
}

fan::graphics::shader_t m_shader;
fan::vulkan::pipelines_t::nr_t pipeline_nr;
fan::vulkan::descriptor_sets_t::layout_nr_t descriptor_layout_nr;

struct block_t {
  void open(loco_t* loco, auto* shape) {
    uniform_buffer.open(loco->get_context());
    shape->dsl_properties.write_descriptor_sets[0].block_common = &uniform_buffer.common;
    shape->dsl_properties.write_descriptor_sets[0].range = sizeof(instance_t) * max_instance_size;
    shape->dsl_properties.write_descriptor_sets[1].block_common = &shape->m_shader.projection_view_block.common;
    shape->dsl_properties.write_descriptor_sets[1].range = sizeof(fan::mat4) * 2;
    descriptor_set_nr = loco->get_context()->descriptor_sets.push(
      loco->get_context(),
      shape->descriptor_layout_nr,
      shape->dsl_properties
    );
  }
  void close(loco_t* loco) {
    uniform_buffer.close(loco->get_context(), &loco->m_write_queue);
  }

  fan::graphics::core::uniform_block_t<instance_t, max_instance_size> uniform_buffer;
  fan::graphics::cid_t* cid[max_instance_size];
  instance_properties_t p[max_instance_size];
  fan::vulkan::descriptor_sets_t::nr_t descriptor_set_nr;
};

protected:

fan::vulkan::descriptor_set_layout_t<2> dsl_properties;

loco_bdbt_NodeReference_t root;

#define BLL_set_CPP_Node_ConstructDestruct
#define BLL_set_AreWeInsideStruct 1
#define BLL_set_prefix bll_block
#define BLL_set_BaseLibrary 1
#define BLL_set_Link 1
#define BLL_set_StoreFormat 1
//#define BLL_set_StoreFormat1_alloc_open malloc
//#define BLL_set_StoreFormat1_alloc_close free
#define BLL_set_type_node uint16_t
#define BLL_set_node_data \
    block_t block;
#include _FAN_PATH(BLL/BLL.h)

bll_block_t blocks;

#define BLL_set_CPP_Node_ConstructDestruct
#define BLL_set_AreWeInsideStruct 1
#define BLL_set_prefix shape_bm
#define BLL_set_BaseLibrary 1
#define BLL_set_Link 0
#define BLL_set_type_node uint16_t
#define BLL_set_node_data \
    bll_block_NodeReference_t first_block; \
    bll_block_NodeReference_t last_block; \
    instance_properties_t instance_properties;
#include _FAN_PATH(BLL/BLL.h)

shape_bm_t bm_list;

public:

#undef sb_shader_vertex_path
#undef sb_shader_fragment_path
#undef sb_vertex_count