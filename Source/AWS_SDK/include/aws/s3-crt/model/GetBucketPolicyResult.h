/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <aws/s3-crt/S3Crt_EXPORTS.h>
#include <aws/core/utils/stream/ResponseStream.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <utility>

namespace Aws
{
template<typename RESULT_TYPE>
class AmazonWebServiceResult;

namespace S3Crt
{
namespace Model
{
  class AWS_S3CRT_API GetBucketPolicyResult
  {
  public:
    GetBucketPolicyResult();
    //We have to define these because Microsoft doesn't auto generate them
    GetBucketPolicyResult(GetBucketPolicyResult&&);
    GetBucketPolicyResult& operator=(GetBucketPolicyResult&&);
    //we delete these because Microsoft doesn't handle move generation correctly
    //and we therefore don't trust them to get it right here either.
    GetBucketPolicyResult(const GetBucketPolicyResult&) = delete;
    GetBucketPolicyResult& operator=(const GetBucketPolicyResult&) = delete;


    GetBucketPolicyResult(Aws::AmazonWebServiceResult<Aws::Utils::Stream::ResponseStream>&& result);
    GetBucketPolicyResult& operator=(Aws::AmazonWebServiceResult<Aws::Utils::Stream::ResponseStream>&& result);



    /**
     * <p>The bucket policy as a JSON document.</p>
     */
    inline Aws::IOStream& GetPolicy() { return m_policy.GetUnderlyingStream(); }

    /**
     * <p>The bucket policy as a JSON document.</p>
     */
    inline void ReplaceBody(Aws::IOStream* body) { m_policy = Aws::Utils::Stream::ResponseStream(body); }

  private:

  Aws::Utils::Stream::ResponseStream m_policy;
  };

} // namespace Model
} // namespace S3Crt
} // namespace Aws
