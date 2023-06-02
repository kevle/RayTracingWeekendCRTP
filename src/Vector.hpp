#pragma once

#include "Base.hpp"
#include "Math.hpp"

// Forward declarations
template <typename Derived>
struct VectorExprBase;
template <typename Derived>
struct VectorExpr;
template <typename ScalarType, SizeType Size>
struct Vector;
template <typename Derived, typename ScalarType, SizeType Size>
struct VectorData;
template <typename ScalarType>
struct Vector3;
template <typename DerivedA, typename DerivedB>
struct SumExpr;
template <typename DerivedA, typename DerivedB>
struct SubExpr;
template <typename Derived, typename Operation>
struct UnaryOp;
template <typename ScalarB, typename ScalarFunc>
struct ApplyScalarFunc;
template <typename Derived, typename Operation>
struct BinaryOp;

template <typename Derived, typename ScalarType, SizeType Rows, SizeType Cols>
struct MatrixData;
template <typename ScalarType, SizeType Rows, SizeType Cols>
struct Matrix;
template <typename ScalarType, SizeType Cols>
struct MatrixRow;

template <typename ScalarB, typename ScalarFunc>
struct Traits<ApplyScalarFunc<ScalarB, ScalarFunc>>
{
    using Scalar = ScalarB;
};

template <typename Derived, typename ScalarType, SizeType Size>
struct Traits<VectorData<Derived, ScalarType, Size>>
{
    using Scalar = ScalarType;
    constexpr static auto size = Size;
    constexpr static bool PrimalStorage = true;
    constexpr static bool MatrixType = false;
};

template <typename ScalarType, SizeType Size>
struct Traits<Vector<ScalarType, Size>>
{
    using Scalar = ScalarType;
    constexpr static auto size = Size;
    constexpr static bool PrimalStorage = true;
    constexpr static bool MatrixType = false;
};

template <typename ScalarType>
struct Traits<Vector3<ScalarType>>
{
    using Scalar = ScalarType;
    constexpr static auto size = 3;
    constexpr static bool PrimalStorage = true;
    constexpr static bool MatrixType = false;
};

template <typename DerivedA, typename DerivedB>
struct Traits<SumExpr<DerivedA, DerivedB>>
{
    using Scalar = decltype(typename Traits<DerivedA>::Scalar() + typename Traits<DerivedB>::Scalar());
    constexpr static auto size = Traits<DerivedA>::size;
    constexpr static bool PrimalStorage = false;
    constexpr static bool MatrixType = Traits<DerivedA>::MatrixType && Traits<DerivedB>::MatrixType;
};

template <typename DerivedA, typename DerivedB>
struct Traits<SubExpr<DerivedA, DerivedB>>
{
    using Scalar = decltype(typename Traits<DerivedA>::Scalar() - typename Traits<DerivedB>::Scalar());
    constexpr static auto size = Traits<DerivedA>::size;
    constexpr static bool PrimalStorage = false;
    constexpr static bool MatrixType = Traits<DerivedA>::MatrixType && Traits<DerivedB>::MatrixType;
};

template <typename Derived>
struct Traits<VectorExprBase<Derived>>
{
    using Scalar = typename Traits<Derived>::Scalar;
    constexpr static auto size = Traits<Derived>::size;
    constexpr static bool PrimalStorage = Traits<Derived>::PrimalStorage;
    constexpr static bool MatrixType = Traits<Derived>::MatrixType;
};

template <typename Derived, typename Operation>
struct Traits<UnaryOp<Derived, Operation>>
{
    using Scalar = typename Traits<Derived>::Scalar;
    constexpr static auto size = Traits<Derived>::size;
    constexpr static bool PrimalStorage = false;
    constexpr static bool MatrixType = Traits<Derived>::MatrixType;
};

template <typename Derived, typename ScalarB, typename ScalarFunc>
struct Traits<UnaryOp<Derived, ApplyScalarFunc<ScalarB, ScalarFunc>>>
{
    using Scalar = decltype(typename Traits<Derived>::Scalar() * typename Traits<ApplyScalarFunc<ScalarB, ScalarFunc>>::Scalar());
    constexpr static auto size = Traits<Derived>::size;
    constexpr static bool PrimalStorage = false;
    constexpr static bool MatrixType = Traits<Derived>::MatrixType;
};

template <typename Derived, typename Operation>
struct Traits<BinaryOp<Derived, Operation>>
{
    using Scalar = typename Operation::Scalar;
    constexpr static auto size = Traits<Derived>::size;
    constexpr static bool PrimalStorage = false;
    constexpr static bool MatrixType = Traits<Derived>::MatrixType;
};

template <typename Derived>
struct VecBase : ExprBase<Derived>
{
    using Scalar = typename Traits<Derived>::Scalar;
    using Index = SizeType;
    using MyType = Derived;
};

template <class Derived>
struct VectorExprBase : VecBase<Derived>
{
    using Base = VecBase<Derived>;
    using Scalar = std::remove_const_t<typename Base::Scalar>;
    using Index = typename Base::Index;
    using VecBase<Derived>::get_me;
    constexpr static auto size = Traits<Derived>::size;

    constexpr Scalar& operator[](Index i)
    {
        return get_me()[i];
    }

    constexpr Scalar operator[](Index i) const
    {
        return get_me()[i];
    }

    constexpr Vector<Scalar, size> eval() const
    {
        return Vector<Scalar, size>(*this);
    }

    constexpr operator Vector<Scalar, size>() const
    {
        return eval();
    }

    constexpr Scalar squared_length() const
    {
        Scalar sum = 0;
        for (SizeType i = 0; i < size; ++i)
        {
            Scalar val = (*this)[i];
            sum += val * val;
        }
        return sum;
    }

    constexpr Scalar length() const
    {
        return std::sqrt(squared_length());
    }

    template <typename DerivedR>
    constexpr Scalar dot(const VectorExprBase<DerivedR>& rhs) const
    {
        Scalar sum = 0;
        for (SizeType i = 0; i < size; ++i)
        {
            sum += (*this)[i] * rhs[i];
        }
        return sum;
    }

    constexpr Vector<Scalar, size> normalized() const
    {
        Vector<Scalar, size> out;
        Scalar invLength = Scalar(1) / length();
        for (SizeType i = 0; i < size; ++i)
        {
            out[i] = (*this)[i] * invLength;
        }
        return out;
    }
};

// Unary operations
template <class Derived, class Operation>
struct UnaryOp : VectorExprBase<UnaryOp<Derived, Operation>>
{
    typename ExpressionStorage<Derived>::type operand;
    Operation operation;

    using Scalar = typename VectorExprBase<UnaryOp<Derived, Operation>>::Scalar;

    constexpr UnaryOp(const Derived& operand, Operation operation = Operation())
        : operand(operand)
        , operation(operation)
    {
    }

    constexpr Scalar operator[](SizeType i) const
    {
        return operation(operand[i]);
    }
};

struct NegFunc
{
    template <typename T>
    T operator()(T in) const
    {
        return -in;
    }
};

template <typename Derived>
using NegExpr = UnaryOp<Derived, NegFunc>;

struct MulScalar
{
    template <typename A, typename B>
    auto operator()(A a, B b) const
    {
        return a * b;
    }
};

struct DivScalar
{
    template <typename A, typename B>
    auto operator()(A a, B b) const
    {
        return a / b;
    }
};

struct AddScalar
{
    template <typename A, typename B>
    auto operator()(A a, B b) const
    {
        return a + b;
    }
};

struct SubScalar
{
    template <typename A, typename B>
    auto operator()(A a, B b) const
    {
        return a - b;
    }
};

template <typename ScalarB, typename ScalarFunc>
struct ApplyScalarFunc
{
    const ScalarB b;

    constexpr ApplyScalarFunc(ScalarB b)
        : b(b)
    {
    }

    template <typename T>
    auto operator()(T in) const
    {
        return ScalarFunc()(in, b);
    }
};

template <typename Derived, typename ScalarB>
using MulScalarOp = UnaryOp<Derived, ApplyScalarFunc<ScalarB, MulScalar>>;

template <typename Derived, typename ScalarB>
using DivScalarOp = UnaryOp<Derived, ApplyScalarFunc<ScalarB, DivScalar>>;

template <typename Derived, typename ScalarB>
using AddScalarOp = UnaryOp<Derived, ApplyScalarFunc<ScalarB, AddScalar>>;

template <typename Derived, typename ScalarB>
using SubScalarOp = UnaryOp<Derived, ApplyScalarFunc<ScalarB, SubScalar>>;

// Binary operations
template <class Derived, class Operation>
struct BinaryOp : public VectorExprBase<BinaryOp<Derived, Operation>>
{
    const Operation op;

    using Scalar = typename VectorExprBase<UnaryOp<Derived, Operation>>::Scalar;

    constexpr BinaryOp(const Operation& op)
        : op(op)
    {
    }

    constexpr auto operator[](SizeType i) const
    {
        return op[i];
    }
};

template <typename DerivedA_, typename DerivedB_>
struct SumFunc
{
    using Derived = DerivedA_;
    typename ExpressionStorage<Derived>::type lhs;
    typename ExpressionStorage<Derived>::type rhs;

    using Scalar = decltype(typename VecBase<Derived>::Scalar() * typename VecBase<DerivedB_>::Scalar());
};

// Now we cann add also Unary / Binary operations as expressions
template <class Derived>
struct VectorExpr : VectorExprBase<Derived>
{
    using VectorExprBase<Derived>::get_me;
    constexpr NegExpr<Derived> operator-() const
    {
        return NegExpr<Derived>(get_me());
    }
};

template <typename DerivedA, typename DerivedB>
struct SumExpr : public VectorExpr<SumExpr<DerivedA, DerivedB>>
{
    typename ExpressionStorage<DerivedA>::type lhs;
    typename ExpressionStorage<DerivedB>::type rhs;

    using BaseA = VecBase<DerivedA>;
    using ScalarA = typename BaseA::Scalar;
    using BaseB = VecBase<DerivedB>;
    using ScalarB = typename BaseB::Scalar;

    using Scalar = decltype(ScalarA() + ScalarB());

    constexpr SumExpr(const DerivedA& lhs, const DerivedB& rhs)
        : lhs(lhs)
        , rhs(rhs)
    {
    }

    constexpr Scalar operator[](SizeType i) const
    {
        return lhs[i] + rhs[i];
    }
};

// TODO: Create general Univariate / Bivariate functions to reduce redundancy
template <typename DerivedA, typename DerivedB>
struct SubExpr : public VectorExpr<SubExpr<DerivedA, DerivedB>>
{
    typename ExpressionStorage<DerivedA>::type lhs;
    typename ExpressionStorage<DerivedB>::type rhs;

    using BaseA = VecBase<DerivedA>;
    using ScalarA = typename BaseA::Scalar;
    using BaseB = VecBase<DerivedB>;
    using ScalarB = typename BaseB::Scalar;

    using Scalar = decltype(ScalarA() + ScalarB());

    constexpr SubExpr(const DerivedA& lhs, const DerivedB& rhs)
        : lhs(lhs)
        , rhs(rhs)
    {
    }

    constexpr Scalar operator[](SizeType i) const
    {
        return lhs[i] - rhs[i];
    }
};

template <typename Derived, typename ScalarType, SizeType Size>
struct VectorData : public VectorExpr<Derived>
{
protected:
    static_assert(Size > 0, "Size too small.");

    ScalarType v[Size];

    template <class L>
    constexpr void Init(SizeType offset, L x)
    {
        v[offset] = x;
    }

    template <class L, class... T>
    constexpr void Init(SizeType offset, L x, T... rest)
    {
        // static_assert(std::is_same<L, ScalarType>::value, "Wrong value type.");
        v[offset] = x;
        Init(offset + SizeType(1), rest...);
    }

public:
    // using MyType = VectorData<Derived, ScalarType, Size>;
    using MyType = Derived;
    using Scalar = ScalarType;
    using Index = SizeType;

    using VectorExpr<Derived>::get_me;

    constexpr VectorData()
        : v{}
    {
    }

    template <typename DerivedB>
    constexpr VectorData(const VectorExprBase<DerivedB>& rhs)
    {
        static_assert(Traits<DerivedB>::size == Size, "Size mismatch.");
        for (SizeType i = 0; i < Size; ++i)
        {
            (*this)[i] = rhs[i];
        }
    }

    constexpr VectorData(Scalar init)
    {
        for (Index i = 0; i < Size; ++i)
        {
            v[i] = init;
        }
    }

    template <class... T>
    explicit constexpr VectorData(Scalar x, T... rest)
    {
        static_assert(sizeof...(T) == Size - 1, "No constructor matches.");
        v[0] = x;
        Init(SizeType(1), rest...);
    }

    template <typename DerivedB>
    constexpr MyType& operator=(const VectorExprBase<DerivedB>& rhs)
    {
        for (SizeType i = 0; i < Size; ++i)
        {
            (*this)[i] = rhs[i];
        }
        return get_me();
    }
    template <typename DerivedB>
    constexpr MyType& operator+=(const VectorExprBase<DerivedB>& rhs)
    {
        for (SizeType i = 0; i < Size; ++i)
        {
            (*this)[i] += rhs[i];
        }
        return get_me();
    }
    template <typename DerivedB>
    constexpr MyType& operator-=(const VectorExprBase<DerivedB>& rhs)
    {
        for (SizeType i = 0; i < Size; ++i)
        {
            (*this)[i] -= rhs[i];
        }
        return get_me();
    }
    template <typename DerivedB>
    constexpr MyType& operator*=(const VectorExprBase<DerivedB>& rhs)
    {
        for (SizeType i = 0; i < Size; ++i)
        {
            (*this)[i] *= rhs[i];
        }
        return get_me();
    }
    template <typename DerivedB>
    constexpr MyType& operator/=(const VectorExprBase<DerivedB>& rhs)
    {
        for (SizeType i = 0; i < Size; ++i)
        {
            (*this)[i] /= rhs[i];
        }
        return get_me();
    }
    constexpr MyType& operator+=(const Scalar& rhs)
    {
        for (SizeType i = 0; i < Size; ++i)
        {
            (*this)[i] += rhs;
        }
        return get_me();
    }
    constexpr MyType& operator-=(const Scalar& rhs)
    {
        for (SizeType i = 0; i < Size; ++i)
        {
            (*this)[i] -= rhs;
        }
        return get_me();
    }
    constexpr MyType& operator*=(const Scalar& rhs)
    {
        for (SizeType i = 0; i < Size; ++i)
        {
            (*this)[i] *= rhs;
        }
        return get_me();
    }
    constexpr MyType& operator/=(const Scalar& rhs)
    {
        for (SizeType i = 0; i < Size; ++i)
        {
            (*this)[i] /= rhs;
        }
        return get_me();
    }

    constexpr MyType& normalize()
    {
        ScalarType invLength = ScalarType(1) / this->length();
        return get_me() *= invLength;
    }

    constexpr Scalar& operator[](SizeType i)
    {
        return v[i];
    }

    constexpr Scalar operator[](SizeType i) const
    {
        return v[i];
    }
};

template <typename ScalarType, SizeType Size>
struct Vector : public VectorData<Vector<ScalarType, Size>, ScalarType, Size>
{
    using VectorData<Vector<ScalarType, Size>, ScalarType, Size>::VectorData;
};

template <typename ScalarType>
struct Vector3 : public VectorData<Vector3<ScalarType>, ScalarType, 3>
{
    using Scalar = ScalarType;

    using VectorData<Vector3<ScalarType>, ScalarType, 3>::VectorData;
    using VectorData<Vector3<ScalarType>, ScalarType, 3>::operator[];
    using VectorData<Vector3<ScalarType>, ScalarType, 3>::operator-;
    using VectorData<Vector3<ScalarType>, ScalarType, 3>::operator=;
    using VectorData<Vector3<ScalarType>, ScalarType, 3>::operator-=;
    using VectorData<Vector3<ScalarType>, ScalarType, 3>::operator+=;
    using VectorData<Vector3<ScalarType>, ScalarType, 3>::operator*=;
    using VectorData<Vector3<ScalarType>, ScalarType, 3>::operator/=;

    constexpr Vector3() {}

    constexpr Vector3(const Vector<ScalarType, 3>& rhs)
    {
        *this = rhs;
    }

    template <typename Derived>
    constexpr Vector3(const VectorExpr<Derived>& rhs)
    {
        *this = rhs;
    }

    constexpr Scalar& x()
    {
        return (*this)[0];
    }
    constexpr Scalar& y()
    {
        return (*this)[1];
    }
    constexpr Scalar& z()
    {
        return (*this)[2];
    }

    constexpr Scalar& r()
    {
        return x();
    }
    constexpr Scalar& g()
    {
        return y();
    }
    constexpr Scalar& b()
    {
        return z();
    }

    constexpr Scalar x() const
    {
        return (*this)[0];
    }
    constexpr Scalar y() const
    {
        return (*this)[1];
    }
    constexpr Scalar z() const
    {
        return (*this)[2];
    }

    constexpr Scalar r() const
    {
        return x();
    }
    constexpr Scalar g() const
    {
        return y();
    }
    constexpr Scalar b() const
    {
        return z();
    }

    constexpr Vector3<ScalarType> cross(const Vector3<ScalarType>& rhs) const
    {
        return Vector3<ScalarType>(
            ((*this)[1] * rhs[2] - (*this)[2] * rhs[1]),
            ((*this)[2] * rhs[0] - (*this)[0] * rhs[2]),
            ((*this)[0] * rhs[1] - (*this)[1] * rhs[0]));
    }
};

using Vector3i = Vector3<int>;
using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

using RGBPixel = Vector3<unsigned char>;

template <typename DerivedA, typename DerivedB, typename = std::enable_if_t<!Traits<DerivedA>::MatrixType>>
constexpr auto operator*(const VectorExpr<DerivedA>& lhs, const VectorExpr<DerivedB>& rhs)
{
    using Scalar = decltype(typename VectorExpr<DerivedA>::Scalar() * typename VectorExpr<DerivedB>::Scalar());
    constexpr auto size = VectorExpr<DerivedA>::size;
    Vector<Scalar, size> out;
    for (SizeType i = 0; i < size; ++i)
    {
        out[i] = lhs[i] * rhs[i];
    }
    return out;
}
template <typename T>
constexpr auto operator*(const Vector3<T>& lhs, const Vector3<T>& rhs)
{
    return ::operator*<Vector<T, 3>, Vector<T, 3>>(lhs, rhs);
}
template <typename Derived, typename ScalarR, typename = std::enable_if_t<std::is_floating_point<ScalarR>::value>>
constexpr auto operator*(const VectorExprBase<Derived>& lhs, ScalarR rhs)
{
    return MulScalarOp<Derived, ScalarR>(lhs.get_me(), ApplyScalarFunc<ScalarR, MulScalar>(rhs));
}
template <typename Derived, typename ScalarR, typename = std::enable_if_t<std::is_floating_point<ScalarR>::value>>
constexpr auto operator*(ScalarR lhs, const VectorExprBase<Derived>& rhs)
{
    return rhs * lhs;
}

template <typename Derived, typename ScalarR, typename = std::enable_if_t<std::is_floating_point<ScalarR>::value>>
auto operator+(const VectorExprBase<Derived>& lhs, ScalarR rhs)
{
    return AddScalarOp<Derived, ScalarR>(lhs.get_me(), ApplyScalarFunc<ScalarR, AddScalar>(rhs));
}
template <typename Derived, typename ScalarR, typename = std::enable_if_t<std::is_floating_point<ScalarR>::value>>
auto operator+(ScalarR lhs, const VectorExprBase<Derived>& rhs)
{
    return rhs + lhs;
}

template <typename Derived, typename ScalarR, typename = std::enable_if_t<std::is_floating_point<ScalarR>::value>>
auto operator-(const VectorExprBase<Derived>& lhs, ScalarR rhs)
{
    return SubScalarOp<Derived, ScalarR>(lhs.get_me(), ApplyScalarFunc<ScalarR, SubScalar>(rhs));
}
template <typename Derived, typename ScalarR, typename = std::enable_if_t<std::is_floating_point<ScalarR>::value>>
auto operator-(ScalarR lhs, const VectorExprBase<Derived>& rhs)
{
    return -rhs + lhs;
}

template <typename Derived, typename ScalarR>
auto operator/(const VectorExprBase<Derived>& lhs, ScalarR rhs)
{
    return DivScalarOp<Derived, ScalarR>(lhs.get_me(), ApplyScalarFunc<ScalarR, DivScalar>(rhs));
}

template <typename A, typename B>
constexpr SumExpr<A, B> operator+(const VectorExprBase<A>& a, const VectorExprBase<B>& b)
{
    return SumExpr<A, B>(a.get_me(), b.get_me());
}

template <typename A, typename B>
constexpr SubExpr<A, B> operator-(const VectorExprBase<A>& a, const VectorExprBase<B>& b)
{
    return SubExpr<A, B>(a.get_me(), b.get_me());
}

template <typename DerivedA, typename DerivedB>
constexpr auto dot(const VectorExprBase<DerivedA>& a, const VectorExprBase<DerivedB>& b)
{
    return a.dot(b);
}

template <typename ScalarA, typename ScalarB>
constexpr auto cross(const Vector3<ScalarA>& a, const Vector3<ScalarB>& b)
{
    return a.cross(b);
}

namespace CVec
{

// Componentwise min
template <typename A, typename B>
constexpr auto min(const VectorExpr<A>& a, const VectorExpr<B>& b)
{
    Vector<typename A::Scalar, A::size> r;
    for (int i = 0; i < A::size; ++i)
    {
        r[i] = ::min(a[i], b[i]);
    }
    return r;
}
// Componentwise max
template <typename A, typename B>
constexpr auto max(const VectorExpr<A>& a, const VectorExpr<B>& b)
{
    Vector<typename A::Scalar, A::size> r;
    for (int i = 0; i < A::size; ++i)
    {
        r[i] = ::max(a[i], b[i]);
    }
    return r;
}

// Check if every element in a is smaller
template <typename A, typename B>
constexpr auto operator<(const VectorExpr<A>& a, const VectorExpr<B>& b)
{
    static_assert(A::size == B::size, "Size mismatch.");

    for (int i = 0; i < A::size; ++i)
    {
        if (a[i] >= b[i])
            return false;
    }
    return true;
}

// Reduction min
template <typename A>
constexpr auto min(const VectorExpr<A>& v)
{
    int minIndex = 0;
    for (int i = 0; i < A::size; ++i)
    {
        if (v[i] < v[minIndex])
        {
            minIndex = i;
        }
    }
    return v[minIndex];
}
// Reduction max
template <typename A>
constexpr auto max(const VectorExpr<A>& v)
{
    int maxIndex = 0;
    for (int i = 0; i < A::size; ++i)
    {
        if (v[i] > v[maxIndex])
        {
            maxIndex = i;
        }
    }
    return v[maxIndex];
}
} // namespace CVec

using CVec::max;
using CVec::min;
using CVec::operator<;

inline Vector3f random_direction_unit_sphere(const float r1, const float r2)
{
    const float a(2.0f * r1 - 1.0f);
    const float b(r2 * tau<float>());
    Vector3f randomDirection(sqrt(1.f - a * a) * cos(b), sqrt(1.f - a * a) * sin(b), a); // Sample of the unit sphere.
    return randomDirection;
    // sqrt((sqrt(1 - x^2)*cos(y))^2 + (sqrt(1 - x^2)*sin(y))^2  + x^2) = 1
    // return  randomDirection.normalized(); //Should already be normalized.
}

inline Vector3f random_on_unit_sphere()
{
    return random_direction_unit_sphere(random<float>(), random<float>());
}

inline Vector3f random_in_unit_sphere()
{
    return random_on_unit_sphere() * std::sqrt(random<float>());
}

inline Vector3f random_in_unit_disk()
{
    // Vector3f p;
    // do {
    //	p = 2.0f * Vector3f(random<float>(), random<float>(), 0.f) - Vector3f(1.0f, 1.0f, 0.0f);
    // } while (dot(p,p) >= 1.0f);
    // return p;
    float sample = tau<float>() * random<float>();
    return Vector3f(sin(sample), cos(sample), 0.f) * std::sqrt(random<float>());
}

// n is assumed to be normalized
inline Vector3f reflect(const Vector3f& v, const Vector3f& n)
{
    return v - 2.0f * dot(v, n) * n;
}
