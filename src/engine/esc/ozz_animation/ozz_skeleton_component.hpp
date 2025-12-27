#pragma once
#include <string>
#include <vector>
#include <memory>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/base/maths/soa_transform.h>
#include <ozz/base/maths/vec_float.h>
#include <glm/glm.hpp>

// Component holding ozz skeleton data and transform buffers
// This is the runtime representation of a skeleton
class OzzSkeletonComponent final {
public:
    std::string SkeletonGuid;  // GUID of the skeleton asset

    // Ozz runtime skeleton (shared across instances with same skeleton)
    std::shared_ptr<ozz::animation::Skeleton> Skeleton;

    // Transform buffers (per-instance, unique to this entity)
    std::vector<ozz::math::SoaTransform> LocalTransforms;  // Local space (SoA format for SIMD)
    std::vector<ozz::math::Float4x4> ModelMatrices;        // Model space (world space relative to root)

    // Converted matrices for rendering (compatible with current renderer)
    std::vector<glm::mat4> BoneMatrices;  // Final matrices for skinning (model space * inverse bind pose)

    // Cache bone names for debugging/IK targeting
    std::vector<std::string> BoneNames;

    // Sampling cache for optimization (required by ozz)
    std::unique_ptr<ozz::animation::SamplingJob::Context> SamplingCache;

    OzzSkeletonComponent() = default;
    explicit OzzSkeletonComponent(const std::string& skelGuid)
        : SkeletonGuid(skelGuid) {}

    // Initialize transform buffers based on skeleton
    void AllocateBuffers(const ozz::animation::Skeleton& skeleton) {
        const int num_joints = skeleton.num_joints();
        const int num_soa_joints = skeleton.num_soa_joints();

        LocalTransforms.resize(num_soa_joints);
        ModelMatrices.resize(num_joints);
        BoneMatrices.resize(num_joints, glm::mat4(1.0f));

        // Create and resize sampling cache
        SamplingCache = std::make_unique<ozz::animation::SamplingJob::Context>();
        SamplingCache->Resize(num_joints);

        // Cache bone names
        BoneNames.clear();
        for (int i = 0; i < num_joints; ++i) {
            BoneNames.push_back(skeleton.joint_names()[i]);
        }
    }
};
