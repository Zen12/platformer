#pragma once
#include <vector>
#include <cstddef>

constexpr size_t SIMD_ALIGNMENT = 32;
constexpr size_t SIMD_BATCH_SIZE = 8;

template<typename T>
class AlignedArray {
private:
    alignas(SIMD_ALIGNMENT) std::vector<T> _data;

public:
    void Resize(size_t count) {
        size_t aligned = ((count + SIMD_BATCH_SIZE - 1) / SIMD_BATCH_SIZE) * SIMD_BATCH_SIZE;
        _data.resize(aligned, T{});
    }

    void Clear() { _data.clear(); }
    [[nodiscard]] size_t Size() const { return _data.size(); }
    [[nodiscard]] bool Empty() const { return _data.empty(); }

    T& operator[](size_t i) { return _data[i]; }
    const T& operator[](size_t i) const { return _data[i]; }

    T* Data() { return _data.data(); }
    const T* Data() const { return _data.data(); }
};

struct BoneSoA {
    AlignedArray<float> PosX, PosY, PosZ;
    AlignedArray<float> RotX, RotY, RotZ, RotW;
    AlignedArray<float> ScaleX, ScaleY, ScaleZ;

    size_t Count = 0;

    void Resize(size_t count) {
        Count = count;
        PosX.Resize(count);
        PosY.Resize(count);
        PosZ.Resize(count);
        RotX.Resize(count);
        RotY.Resize(count);
        RotZ.Resize(count);
        RotW.Resize(count);
        ScaleX.Resize(count);
        ScaleY.Resize(count);
        ScaleZ.Resize(count);

        for (size_t i = 0; i < count; ++i) {
            RotW[i] = 1.0f;
            ScaleX[i] = 1.0f;
            ScaleY[i] = 1.0f;
            ScaleZ[i] = 1.0f;
        }
    }

    void Clear() {
        Count = 0;
        PosX.Clear();
        PosY.Clear();
        PosZ.Clear();
        RotX.Clear();
        RotY.Clear();
        RotZ.Clear();
        RotW.Clear();
        ScaleX.Clear();
        ScaleY.Clear();
        ScaleZ.Clear();
    }
};
