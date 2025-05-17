#define GLEW_STATIC

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "render/mesh.h"
#include "system/window.h"
#include "asset/asset_loader.h"
#include "components/entity.h"
#include "components/sprite_renderer.h"
#include "components/transforms/transform.h"
#include "components/transforms/rect_transform.h"
#include "components/transforms/rect_transform_root.h"
#include "components/camera_component.h"
#include "components/ui_text_renderer.h"
#include "render_pipeline/render_pipeline.h"