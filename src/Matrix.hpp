#pragma once

#include "Vector.hpp"

template <typename Derived, typename ScalarType, SizeType Rows, SizeType Cols>
struct Traits<MatrixData<Derived, ScalarType, Rows, Cols>>
{
    using Scalar = ScalarType;
    constexpr static auto size = Rows * Cols;
    constexpr static bool PrimalStorage = true;
    constexpr static bool MatrixType = true;
};

template <typename ScalarType, SizeType Rows, SizeType Cols>
struct Traits<Matrix<ScalarType, Rows, Cols>>
{
    using Scalar = ScalarType;
    constexpr static auto size = Rows * Cols;
    constexpr static bool PrimalStorage = true;
    constexpr static bool MatrixType = true;
};

template <typename ScalarType, SizeType Cols>
struct Traits<MatrixRow<ScalarType, Cols>>
{
    using Scalar = ScalarType;
    constexpr static auto size = Cols;
    constexpr static bool PrimalStorage = false;
    constexpr static bool MatrixType = false;
};

// TODO: Maybe add stride for slicing operations
template <typename ScalarType, SizeType Cols>
struct MatrixRow : VectorExprBase<MatrixRow<ScalarType, Cols>>
{
    ScalarType* p;

    constexpr MatrixRow()
        : p(nullptr)
    {
    }

    constexpr MatrixRow(ScalarType* p)
        : p(p)
    {
    }

    constexpr ScalarType operator()(SizeType i) const
    {
        return p[i];
    }
    constexpr ScalarType operator[](SizeType i) const
    {
        return p[i];
    }
};

template <typename Derived, typename ScalarType, SizeType Rows, SizeType Cols>
struct MatrixData : public VectorData<Derived, ScalarType, Rows * Cols>
{
    using VectorData<Derived, ScalarType, Rows * Cols>::VectorData;
    using VectorData<Derived, ScalarType, Rows * Cols>::v;

    using Scalar = ScalarType;

    constexpr Scalar& operator()(SizeType row, SizeType col)
    {
        return v[col + row * Cols];
    }
    constexpr const Scalar& operator()(SizeType row, SizeType col) const
    {
        return v[col + row * Cols];
    }

    MatrixRow<ScalarType, Cols> row(SizeType i)
    {
        return MatrixRow<ScalarType, Cols>(&((*this)(i, 0)));
    }

    MatrixRow<const ScalarType, Cols> row(SizeType i) const
    {
        return MatrixRow<const ScalarType, Cols>(&((*this)(i, 0)));
    }
};

template <typename ScalarType, SizeType Rows, SizeType Cols>
struct Matrix : public MatrixData<Matrix<ScalarType, Rows, Cols>, ScalarType, Rows, Cols>
{
    using MatrixData<Matrix<ScalarType, Rows, Cols>, ScalarType, Rows, Cols>::MatrixData;
};

template <typename ScalarType>
Matrix<ScalarType, 3, 3> RotationX(ScalarType angle)
{
    return Matrix<ScalarType, 3, 3>(
        ScalarType(1), ScalarType(0), ScalarType(0),
        ScalarType(0), ScalarType(std::cos(angle)), ScalarType(-std::sin(angle)),
        ScalarType(0), ScalarType(std::sin(angle)), ScalarType(std::cos(angle)));
}
template <typename ScalarType>
Matrix<ScalarType, 3, 3> RotationY(ScalarType angle)
{
    return Matrix<ScalarType, 3, 3>(
        ScalarType(std::cos(angle)), ScalarType(0), ScalarType(std::sin(angle)),
        ScalarType(0), ScalarType(1), ScalarType(0),
        ScalarType(-std::sin(angle)), ScalarType(0), ScalarType(std::cos(angle)));
}
template <typename ScalarType>
Matrix<ScalarType, 3, 3> RotationZ(ScalarType angle)
{
    return Matrix<ScalarType, 3, 3>(
        ScalarType(std::cos(angle)), ScalarType(-std::sin(angle)), ScalarType(0),
        ScalarType(std::sin(angle)), ScalarType(std::cos(angle)), ScalarType(0),
        ScalarType(0), ScalarType(0), ScalarType(1));
}

template <typename ScalarType>
Matrix<ScalarType, 3, 3> Scale(ScalarType scale_factor)
{
    return Matrix<ScalarType, 3, 3>(
        ScalarType(scale_factor), ScalarType(0), ScalarType(0),
        ScalarType(0), ScalarType(scale_factor), ScalarType(0),
        ScalarType(0), ScalarType(0), ScalarType(scale_factor));
}

template <typename ScalarType, SizeType Size, SizeType Rows, typename DerivedM, typename DerivedV>
Vector<ScalarType, Size> operator*(const MatrixData<DerivedM, ScalarType, Rows, Size>& m, const VectorExprBase<DerivedV>& v)
{
    Vector<ScalarType, Size> res;
    for (SizeType i = 0; i < Rows; ++i)
    {
        res[i] = dot(m.row(i), v);
    }
    return res;
}

template <typename ScalarType, SizeType Size, SizeType Rows, typename DerivedV>
Vector<ScalarType, Size> operator*(const Matrix<ScalarType, Rows, Size>& m, const VectorExprBase<DerivedV>& v)
{
    return ::operator*<ScalarType, Rows, Size, Matrix<ScalarType, Rows, Size>, DerivedV>(m, v);
}
