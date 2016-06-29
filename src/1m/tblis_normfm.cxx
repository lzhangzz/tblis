#include "tblis.hpp"

namespace tblis
{
namespace blis_like
{

template <typename T>
static void tblis_normfm_int(const Matrix<T>& A, T& norm)
{
    Scalar<T> norm_o(&norm);
    bli_normfm(const_cast<Matrix<T>&>(A), norm_o);
}

template <typename T>
static void tblis_normfm_int(const const_scatter_matrix_view<T>& A, T& restrict norm)
{
    dim_t m = A.length();
    dim_t n = A.width();
    inc_t rs = A.row_stride();
    inc_t cs = A.col_stride();
    const inc_t* restrict rscat = A.row_scatter();
    const inc_t* restrict cscat = A.col_scatter();
    const T* restrict p = A.data();

    norm = 0;

    if (rs == 0 && cs == 0)
    {
        for (dim_t i = 0;i < m;i++)
        {
            for (dim_t j = 0;j < n;j++)
            {
                T val = *(p+rscat[i]+cscat[j]);
                norm += norm2(val);
            }
        }
    }
    else if (rs == 0)
    {
        for (dim_t i = 0;i < m;i++)
        {
            for (dim_t j = 0;j < n;j++)
            {
                T val = *(p+rscat[i]+cs*j);
                norm += norm2(val);
            }
        }
    }
    else if (cs == 0)
    {
        for (dim_t i = 0;i < m;i++)
        {
            for (dim_t j = 0;j < n;j++)
            {
                T val = *(p+rs*i+cscat[j]);
                norm += norm2(val);
            }
        }
    }
    else
    {
        for (dim_t i = 0;i < m;i++)
        {
            for (dim_t j = 0;j < n;j++)
            {
                T val = *(p+rs*i+cs*j);
                norm += norm2(val);
            }
        }
    }

    norm = sqrt(real(norm));
}

template <typename T, typename MatrixA>
void tblis_normfm(const MatrixA& A, T& norm)
{
    MatrixA Av;
    ViewNoTranspose(const_cast<MatrixA&>(A), Av);
    tblis_normfm_int(Av, norm);
}

template <typename MatrixA>
typename MatrixA::type tblis_normfm(const MatrixA& A)
{
    typename MatrixA::type norm;
    tblis_normfm(A, norm);
    return norm;
}

#define INSTANTIATION(T,MT,NT,KT,MR,NR,KR) \
template void tblis_normfm(const        Matrix<T>& A, T& norm); \
template void tblis_normfm(const ScatterMatrix<T>& A, T& norm); \
template    T tblis_normfm(const        Matrix<T>& A); \
template    T tblis_normfm(const ScatterMatrix<T>& A);
DEFINE_INSTANTIATIONS()
#undef INSTANTIATION

}
}