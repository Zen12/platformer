#pragma once
#include "../../entity/component_factory.hpp"
#include "rect_transform_component.hpp"
#include "rect_transform_component_serialization.hpp"
#include <iostream>

class RectTransformFactory final : public ComponentFactory<RectTransform, RectTransformComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<RectTransform> &component, const RectTransformComponentSerialization &serialization) override {

        if (const auto rect = component.lock()) {
            rect->SetParent(_scene.lock()->GetRoot());

            for (const auto &layout : serialization.Layouts)
            {
                if (layout.Type == "center_x")
                    rect->AddLayoutOption<CenterXLayoutOption>();
                else if (layout.Type == "center_y")
                    rect->AddLayoutOption<CenterYLayoutOption>();
                else if (layout.Type == "pixel_width")
                    rect->AddLayoutOption<PixelWidthLayoutOption>(layout.Value);
                else if (layout.Type == "pixel_height")
                    rect->AddLayoutOption<PixelHeightLayoutOption>(layout.Value);
                else if (layout.Type == "pivot")
                    rect->AddLayoutOption<PivotLayoutOption>(glm::vec2(layout.X, layout.Y));
                else if (layout.Type == "pixel_x")
                    rect->AddLayoutOption<PixelXLayoutOption>(layout.X, static_cast<AlignmentLayout>(layout.Value));
                else if (layout.Type == "pixel_y")
                    rect->AddLayoutOption<PixelYLayoutOption>(layout.Y, static_cast<AlignmentLayout>(layout.Value));
                else
                    std::cerr << "Unknown layout type: " << layout.Type << std::endl;
            }
        }
    }
};