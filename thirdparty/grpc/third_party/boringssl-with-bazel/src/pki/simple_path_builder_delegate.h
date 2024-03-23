// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BSSL_PKI_SIMPLE_PATH_BUILDER_DELEGATE_H_
#define BSSL_PKI_SIMPLE_PATH_BUILDER_DELEGATE_H_

#include <stddef.h>

#include <openssl/base.h>

#include "path_builder.h"
#include "signature_algorithm.h"
#include "signature_verify_cache.h"

namespace bssl {

class CertErrors;

// SimplePathBuilderDelegate is an implementation of CertPathBuilderDelegate
// that uses some default policies:
//
//   * RSA public keys must be >= |min_rsa_modulus_length_bits|.
//   * Signature algorithm can be RSA PKCS#1, RSASSA-PSS or ECDSA
//   * Digest algorithm can be SHA256, SHA348 or SHA512.
//       * If the |digest_policy| was set to kAllowSha1, then SHA1 is
//         additionally accepted.
//   * EC named curve can be P-256, P-384, P-521.
class OPENSSL_EXPORT SimplePathBuilderDelegate
    : public CertPathBuilderDelegate {
 public:
  enum class DigestPolicy {
    // Accepts digests of SHA256, SHA348 or SHA512
    kStrong,

    // Accepts everything that kStrong does, plus SHA1.
    kWeakAllowSha1,

    kMaxValue = kWeakAllowSha1
  };

  // Error emitted when a public key is rejected because it is an RSA key with a
  // modulus size that is too small.
  static const CertErrorId kRsaModulusTooSmall;

  SimplePathBuilderDelegate(size_t min_rsa_modulus_length_bits,
                            DigestPolicy digest_policy);

  // Accepts RSA PKCS#1, RSASSA-PSS or ECDA using any of the SHA* digests
  // (including SHA1).
  bool IsSignatureAlgorithmAcceptable(SignatureAlgorithm signature_algorithm,
                                      CertErrors *errors) override;

  // Requires RSA keys be >= |min_rsa_modulus_length_bits_|.
  bool IsPublicKeyAcceptable(EVP_PKEY *public_key, CertErrors *errors) override;

  // No-op implementation.
  void CheckPathAfterVerification(const CertPathBuilder &path_builder,
                                  CertPathBuilderResultPath *path) override;

  // No-op implementation.
  bool IsDeadlineExpired() override;

  // No-op implementation.
  SignatureVerifyCache *GetVerifyCache() override;

  // No-op implementation.
  bool IsDebugLogEnabled() override;

  // No-op implementation.
  void DebugLog(std::string_view msg) override;

 private:
  const size_t min_rsa_modulus_length_bits_;
  const DigestPolicy digest_policy_;
};

}  // namespace bssl

#endif  // BSSL_PKI_SIMPLE_PATH_BUILDER_DELEGATE_H_
