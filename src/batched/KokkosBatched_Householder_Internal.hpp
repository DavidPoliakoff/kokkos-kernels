#ifndef __KOKKOSBATCHED_HOUSEHOLDER_INTERNAL_HPP__
#define __KOKKOSBATCHED_HOUSEHOLDER_INTERNAL_HPP__


/// \author Kyungjoo Kim (kyukim@sandia.gov)

#include "KokkosBatched_Util.hpp"


namespace KokkosBatched {
  namespace Experimental {
    ///
    /// Serial Internal Impl
    /// ==================== 
    ///
    /// this impl follows the flame interface of householder transformation
    ///
    struct SerialLeftHouseholderInternal {
      template<typename ValueType>
      KOKKOS_INLINE_FUNCTION
      static int
      invoke(const int m_x2, 
             /* */ ValueType * chi1,
             /* */ ValueType * x2, const int x2s,
             /* */ ValueType * tau) {
        typedef ValueType value_type;
        typedef typename Kokkos::Details::ArithTraits<ValueType>::mag_type mag_type;
        
        const mag_type zero(0);
        const mag_type two(2);
        const mag_type one_half(1.5);
        const mag_type one(1);
        const mag_type minus_one(-1);

        /// compute the 2norm of x2
        mag_type norm_x2(0);
        for (int i=0;i<m_x2;++i) {
          const auto x2_at_i = x2[i*x2s];
          norm_x2 += x2_at_i*x2_at_i;
        }
        norm_x2 = Kokkos::Details::ArithTraits<mag_type>::sqrt(norm_x2);
        
        /// if norm_x2 is zero, return with trivial values
        if (norm_x2 == zero) {
          *chi1 = -(*chi1);
          *tau = one_half;
          
          return 0;
        }

        /// compute magnitude of chi1, equal to norm2 of chi1
        const mag_type norm_chi1 = Kokkos::Details::ArithTraits<value_type>::abs(*chi1);

        /// compute 2 norm of x using norm_chi1 and norm_x2
        const mag_type norm_x = Kokkos::Details::ArithTraits<mag_type>::sqrt(norm_x2*norm_x2 + norm_chi1*norm_chi1);

        /// compute alpha
        const mag_type alpha = (*chi1 > 0 ? one : minus_one)*norm_x;

        /// overwrite x2 with u2
        const value_type chi1_minus_alpha = *chi1 - *alpha;
        const value_type inv_chi1_minus_alpha = one/chi1_minus_alpha;
        for (int i=0;i<m_x2;++i) 
          x2[i*x2s] *= inv_chi1_minus_alpha;
        // later consider to use the following
        // SerialScaleInternal::invoke(m_x2, inv_chi1_minus_alpha, x2, x2s);

        /// compute tau
        const mag_type abs_chi1_minus_alpha = Kokkos::Details::ArithTraits<value_type>::abs(chi1_minus_alpha);
        const mag_type norm_x2_div_abs_chi1_minus_alpha = norm_x2 / abs_chi1_minus_alpha;
        *tau = one_half + one_half*(norm_x_2_div_abs_chi_1_minus_alpha*
                                    norm_x_2_div_abs_chi_1_minus_alpha); 

        /// overwrite chi1 with alpha
        *chi1 = alpha;

        return 0;
      }
    };

  }//  end namespace Experimental
} // end namespace KokkosBatched


#endif
