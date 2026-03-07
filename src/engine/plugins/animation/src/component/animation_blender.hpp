#pragma once
#include "animation_soa_types.hpp"
#include "animation_blender_config.hpp"
#include <cmath>

class AnimationBlender {
private:
    AnimationBlenderConfig _config;

public:
    void Configure(const AnimationBlenderConfig& config) noexcept {
        _config = config;
    }

    [[nodiscard]] const AnimationBlenderConfig& GetConfig() const noexcept {
        return _config;
    }

    [[nodiscard]] AnimationBlenderConfig& GetMutableConfig() noexcept {
        return _config;
    }

    [[nodiscard]] AnimationBlendMode GetMode() const noexcept {
        return _config.Mode;
    }

    [[nodiscard]] float GetTransitionDuration() const noexcept {
        return _config.TransitionDuration;
    }

    [[nodiscard]] float GetAnimationSpeed() const noexcept {
        return _config.AnimationSpeed;
    }

    [[nodiscard]] bool IsVelocityBased() const noexcept {
        return _config.VelocityBasedSpeed;
    }

    [[nodiscard]] float GetVelocitySpeedScale() const noexcept {
        return _config.VelocitySpeedScale;
    }

    [[nodiscard]] const Guid& GetDirectionalForward() const noexcept {
        return _config.DirectionalForwardGuid;
    }

    [[nodiscard]] const Guid& GetDirectionalBack() const noexcept {
        return _config.DirectionalBackGuid;
    }

    [[nodiscard]] const Guid& GetDirectionalLeft() const noexcept {
        return _config.DirectionalLeftGuid;
    }

    [[nodiscard]] const Guid& GetDirectionalRight() const noexcept {
        return _config.DirectionalRightGuid;
    }

    static void LerpSoA(BoneSoA& result, const BoneSoA& a, const BoneSoA& b, float t) {
        const size_t count = a.Count;
        result.Resize(count);

        for (size_t i = 0; i < count; ++i) {
            result.PosX[i] = a.PosX[i] + (b.PosX[i] - a.PosX[i]) * t;
            result.PosY[i] = a.PosY[i] + (b.PosY[i] - a.PosY[i]) * t;
            result.PosZ[i] = a.PosZ[i] + (b.PosZ[i] - a.PosZ[i]) * t;

            result.ScaleX[i] = a.ScaleX[i] + (b.ScaleX[i] - a.ScaleX[i]) * t;
            result.ScaleY[i] = a.ScaleY[i] + (b.ScaleY[i] - a.ScaleY[i]) * t;
            result.ScaleZ[i] = a.ScaleZ[i] + (b.ScaleZ[i] - a.ScaleZ[i]) * t;

            float ax = a.RotX[i], ay = a.RotY[i], az = a.RotZ[i], aw = a.RotW[i];
            float bx = b.RotX[i], by = b.RotY[i], bz = b.RotZ[i], bw = b.RotW[i];

            if (ax * bx + ay * by + az * bz + aw * bw < 0.0f) {
                bx = -bx;
                by = -by;
                bz = -bz;
                bw = -bw;
            }

            float rx = ax + (bx - ax) * t;
            float ry = ay + (by - ay) * t;
            float rz = az + (bz - az) * t;
            float rw = aw + (bw - aw) * t;

            float len = std::sqrt(rx * rx + ry * ry + rz * rz + rw * rw);
            float invLen = 1.0f / len;
            result.RotX[i] = rx * invLen;
            result.RotY[i] = ry * invLen;
            result.RotZ[i] = rz * invLen;
            result.RotW[i] = rw * invLen;
        }
    }

    static void BlendDirectionalSoA(
        BoneSoA& result,
        const BoneSoA& forward,
        const BoneSoA& back,
        const BoneSoA* left,
        const BoneSoA* right,
        float directionAngle
    ) {
        const size_t count = forward.Count;
        result.Resize(count);

        float absAngle = std::abs(directionAngle);
        float forwardWeight = 0.0f, backWeight = 0.0f, leftWeight = 0.0f, rightWeight = 0.0f;

        if (absAngle <= 90.0f) {
            float lateral = absAngle / 90.0f;
            forwardWeight = 1.0f - lateral;
            if (directionAngle < 0.0f && right) {
                rightWeight = lateral;
            } else if (directionAngle > 0.0f && left) {
                leftWeight = lateral;
            } else {
                forwardWeight = 1.0f;
            }
        } else {
            float lateral = (180.0f - absAngle) / 90.0f;
            backWeight = 1.0f - lateral;
            if (directionAngle < 0.0f && right) {
                rightWeight = lateral;
            } else if (directionAngle > 0.0f && left) {
                leftWeight = lateral;
            } else {
                backWeight = 1.0f;
            }
        }

        for (size_t i = 0; i < count; ++i) {
            float px = forward.PosX[i] * forwardWeight + back.PosX[i] * backWeight;
            float py = forward.PosY[i] * forwardWeight + back.PosY[i] * backWeight;
            float pz = forward.PosZ[i] * forwardWeight + back.PosZ[i] * backWeight;

            float sx = forward.ScaleX[i] * forwardWeight + back.ScaleX[i] * backWeight;
            float sy = forward.ScaleY[i] * forwardWeight + back.ScaleY[i] * backWeight;
            float sz = forward.ScaleZ[i] * forwardWeight + back.ScaleZ[i] * backWeight;

            float qx = forward.RotX[i] * forwardWeight;
            float qy = forward.RotY[i] * forwardWeight;
            float qz = forward.RotZ[i] * forwardWeight;
            float qw = forward.RotW[i] * forwardWeight;

            float bqx = back.RotX[i], bqy = back.RotY[i], bqz = back.RotZ[i], bqw = back.RotW[i];
            if (qx * bqx + qy * bqy + qz * bqz + qw * bqw < 0.0f) {
                bqx = -bqx; bqy = -bqy; bqz = -bqz; bqw = -bqw;
            }
            qx += bqx * backWeight;
            qy += bqy * backWeight;
            qz += bqz * backWeight;
            qw += bqw * backWeight;

            if (left && leftWeight > 0.0f) {
                px += left->PosX[i] * leftWeight;
                py += left->PosY[i] * leftWeight;
                pz += left->PosZ[i] * leftWeight;
                sx += left->ScaleX[i] * leftWeight;
                sy += left->ScaleY[i] * leftWeight;
                sz += left->ScaleZ[i] * leftWeight;

                float lqx = left->RotX[i], lqy = left->RotY[i], lqz = left->RotZ[i], lqw = left->RotW[i];
                if (qx * lqx + qy * lqy + qz * lqz + qw * lqw < 0.0f) {
                    lqx = -lqx; lqy = -lqy; lqz = -lqz; lqw = -lqw;
                }
                qx += lqx * leftWeight;
                qy += lqy * leftWeight;
                qz += lqz * leftWeight;
                qw += lqw * leftWeight;
            }

            if (right && rightWeight > 0.0f) {
                px += right->PosX[i] * rightWeight;
                py += right->PosY[i] * rightWeight;
                pz += right->PosZ[i] * rightWeight;
                sx += right->ScaleX[i] * rightWeight;
                sy += right->ScaleY[i] * rightWeight;
                sz += right->ScaleZ[i] * rightWeight;

                float rqx = right->RotX[i], rqy = right->RotY[i], rqz = right->RotZ[i], rqw = right->RotW[i];
                if (qx * rqx + qy * rqy + qz * rqz + qw * rqw < 0.0f) {
                    rqx = -rqx; rqy = -rqy; rqz = -rqz; rqw = -rqw;
                }
                qx += rqx * rightWeight;
                qy += rqy * rightWeight;
                qz += rqz * rightWeight;
                qw += rqw * rightWeight;
            }

            result.PosX[i] = px;
            result.PosY[i] = py;
            result.PosZ[i] = pz;
            result.ScaleX[i] = sx;
            result.ScaleY[i] = sy;
            result.ScaleZ[i] = sz;

            float len = std::sqrt(qx * qx + qy * qy + qz * qz + qw * qw);
            float invLen = 1.0f / len;
            result.RotX[i] = qx * invLen;
            result.RotY[i] = qy * invLen;
            result.RotZ[i] = qz * invLen;
            result.RotW[i] = qw * invLen;
        }
    }

    void Blend(
        BoneSoA& result,
        const BoneSoA& primary,
        const BoneSoA* secondary,
        float blendFactor
    ) const {
        if (_config.Mode == AnimationBlendMode::Transition && secondary) {
            LerpSoA(result, primary, *secondary, blendFactor);
        } else {
            result = primary;
        }
    }
};
