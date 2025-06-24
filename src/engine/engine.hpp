#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "render/mesh.hpp"
#include "system/window.hpp"
#include "asset/asset_loader.hpp"
#include "components/entity.hpp"
#include "components/sprite_renderer.hpp"
#include "components/transforms/transform.hpp"
#include "components/transforms/rect_transform.hpp"
#include "components/transforms/rect_transform_root.hpp"
#include "components/camera_component.hpp"
#include "components/ui_text_renderer.hpp"
#include "render_pipeline/render_pipeline.hpp"
#include "system/time.hpp"
#include "asset/asset_manager.hpp"
#include "scene/scene_manager.hpp"