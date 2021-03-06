#pragma once

#include <utility>

#include "drake/common/drake_copyable.h"
#include "drake/common/drake_deprecated.h"
#include "drake/common/eigen_types.h"
#include "drake/geometry/geometry_ids.h"

namespace drake {
namespace geometry {

/** The data for reporting the signed distance between two geometries, A and B.
 It provides the id's of the two geometries, the witness points Ca and Cb on
 the surfaces of A and B, the signed distance, and nhat_BA_W a direction of
 fastest increasing distance (always unit length and always point outward
 from B's surface).

 - When A and B are separated, distance > 0.
 - When A and B are touching or penetrating, distance <= 0.
 - By definition, nhat_AB_W must be in the opposite direction of nhat_BA_W.
 - (p_WCa - p_Wcb) = distance · nhat_BA_W.
 - In some cases, nhat_BA_W is not unique, and is_nhat_BA_W_unique is false.
 @warning For two geometries that are just touching (i.e., distance = 0), the
          underlying code can guarantee a correct value for nhat_BA_W only
          when one geometry is a sphere, and the other geometry is a sphere, a
          box, or a cylinder. Otherwise, the underlying code is not in place yet
          to guarantee a correct value for nhat_BA_W when surfaces are just
          touching, and the vector will be populated by NaN values.
 @tparam T The underlying scalar type. Must be a valid Eigen scalar.
 */
template <typename T>
struct SignedDistancePair{
  // 2021-08-01 deprecation note: While deprecating the is_grad_W_unique, we
  // cannot use the standard Drake mechanism for declaring copy and move
  // semantics. The default implementations insist on writing to the deprecated
  // member and the macro rebels against being enclosed in the
  // deprecation-suppressing pragma. Once deprecation is complete restore the
  // standard boilerplate:
  //
  //  DRAKE_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(SignedDistancePair)
  //  SignedDistancePair() = default;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  SignedDistancePair() = default;
  /** @name Implements CopyConstructible, CopyAssignable, MoveConstructible,
   MoveAssignable */
  //@{
  SignedDistancePair(const SignedDistancePair& other) = default;
  SignedDistancePair(SignedDistancePair&& other) = default;
  SignedDistancePair& operator=(const SignedDistancePair& other) =
      default;
  SignedDistancePair& operator=(SignedDistancePair&& other) =
      default;
  //@}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
/* 2021-08-01 deprecation note: This constructor is wrapped in a deprecation
 silencer because gcc will implicitly try to write to the deprecated member and
 throw a build error. When the deprecated member is removed, we *keep* this
 constructor, but lose the pragma manipulations (in contrast to the other
 constructors which will just be deleted as noted). */
  /** Constructor
   @param a             The id of the first geometry (A).
   @param b             The id of the second geometry (B).
   @param p_ACa_in      The witness point on geometry A's surface, in A's frame.
   @param p_BCb_in      The witness point on geometry B's surface, in B's frame.
   @param dist          The signed distance between p_A and p_B.
   @param nhat_BA_W_in  A direction of fastest increasing distance.
   @pre nhat_BA_W_in is unit-length. */
  SignedDistancePair(GeometryId a, GeometryId b, const Vector3<T>& p_ACa_in,
                     const Vector3<T>& p_BCb_in, const T& dist,
                     const Vector3<T>& nhat_BA_W_in)
      : id_A(a),
        id_B(b),
        p_ACa(p_ACa_in),
        p_BCb(p_BCb_in),
        distance(dist),
        nhat_BA_W(nhat_BA_W_in) {}
#pragma GCC diagnostic pop

  /** Constructor
   @param a             The id of the first geometry (A).
   @param b             The id of the second geometry (B).
   @param p_ACa_in      The witness point on geometry A's surface, in A's frame.
   @param p_BCb_in      The witness point on geometry B's surface, in B's frame.
   @param dist          The signed distance between p_A and p_B.
   @param nhat_BA_W_in  A direction of fastest increasing distance.
   @param is_nhat_BA_W_unique_in  True if nhat_BA_W is unique.
   @pre nhat_BA_W_in is unit-length. */
  DRAKE_DEPRECATED("2021-08-01",
                   "SignedDistancePair will no longer report uniqueness. "
                   "If you require knowledge of uniqueness, please contact the "
                   "Drake team.")
  SignedDistancePair(GeometryId a, GeometryId b, const Vector3<T>& p_ACa_in,
                     const Vector3<T>& p_BCb_in, const T& dist,
                     const Vector3<T>& nhat_BA_W_in,
                     bool is_nhat_BA_W_unique_in)
      : id_A(a),
        id_B(b),
        p_ACa(p_ACa_in),
        p_BCb(p_BCb_in),
        distance(dist),
        nhat_BA_W(nhat_BA_W_in),
        is_nhat_BA_W_unique(is_nhat_BA_W_unique_in)
  // TODO(DamrongGuoy): When we have a full implementation of computing
  //  nhat_BA_W in ComputeSignedDistancePairwiseClosestPoints, check a
  //  condition like this (within epsilon):
  //      DRAKE_DEMAND(nhat_BA_W.norm() == T(1.));
  {}

  /** Constructor.
   We keep this constructor temporarily for backward compatibility.
   @param a         The id of the first geometry (A).
   @param b         The id of the second geometry (B).
   @param p_ACa_in  The witness point on geometry A's surface, in A's frame.
   @param p_BCb_in  The witness point on geometry B's surface, in B's frame.
   @param dist      The signed distance between p_A and p_B. */
  DRAKE_DEPRECATED("2021-08-01",
                   "Please use default or full constructor instead.")
  SignedDistancePair(GeometryId a, GeometryId b, const Vector3<T>& p_ACa_in,
                     const Vector3<T>& p_BCb_in, const T& dist)
      : id_A(a),
        id_B(b),
        p_ACa(p_ACa_in),
        p_BCb(p_BCb_in),
        distance(dist),
        nhat_BA_W(Vector3<T>::Zero()),
        is_nhat_BA_W_unique(false) {}

  /** Swaps the interpretation of geometries A and B. */
  void SwapAAndB() {
    // Leave distance and is_nhat_BA_W_unique alone; swapping A and B
    // doesn't change them.
    std::swap(id_A, id_B);
    std::swap(p_ACa, p_BCb);
    nhat_BA_W = -nhat_BA_W;
  }

  /** The id of the first geometry in the pair. */
  GeometryId id_A;
  /** The id of the second geometry in the pair. */
  GeometryId id_B;
  // TODO(SeanCurtis-TRI): Determine if this is the *right* API. Should we
  // really be returning the points in geometry frame and not the world frame?
  //  1. Penetration as point pair returns the points in world frame.
  //  2. FCL computes it in world frame.
  //  3. Generally, MBP would want the points in *body* frame. MBP has access
  //     to X_WB but does *not* generally have access to X_BG (it would have to
  //     query QueryObject for that information). Although, such a query can
  //     easily be provided.
  /** The witness point on geometry A's surface, expressed in A's frame. */
  Vector3<T> p_ACa;
  /** The witness point on geometry B's surface, expressed in B's frame. */
  Vector3<T> p_BCb;
  /** The signed distance between p_ACa and p_BCb. */
  T distance{};
  /** A direction of fastest increasing distance. */
  Vector3<T> nhat_BA_W;
  DRAKE_DEPRECATED("2021-08-01",
                   "SignedDistancePair will no longer report uniqueness. "
                   "If you require knowledge of uniqueness, please contact the "
                   "Drake team.")
  bool is_nhat_BA_W_unique{false};
};

}  // namespace geometry
}  // namespace drake
