// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef GOOGLE_CLOUD_CPP_GOOGLE_CLOUD_STORAGE_INTERNAL_CURL_HANDLE_FACTORY_H
#define GOOGLE_CLOUD_CPP_GOOGLE_CLOUD_STORAGE_INTERNAL_CURL_HANDLE_FACTORY_H

#include "google/cloud/storage/internal/curl_handle.h"
#include "google/cloud/storage/internal/curl_wrappers.h"
#include "google/cloud/storage/version.h"
#include <deque>
#include <mutex>
#include <string>

namespace google {
namespace cloud {
namespace storage {
inline namespace STORAGE_CLIENT_NS {
namespace internal {
/**
 * Implements the Factory Pattern for CURL handles (and multi-handles).
 */
class CurlHandleFactory {
 public:
  virtual ~CurlHandleFactory() = default;

  virtual CurlPtr CreateHandle() = 0;
  virtual void CleanupHandle(CurlHandle&&) = 0;

  virtual CurlMulti CreateMultiHandle() = 0;
  virtual void CleanupMultiHandle(CurlMulti&&) = 0;

  virtual std::string LastClientIpAddress() const = 0;

 protected:
  // Only virtual for testing purposes.
  virtual void SetCurlStringOption(CURL* handle, CURLoption option_tag,
                                   char const* value);
  void SetCurlOptions(CURL* handle, ChannelOptions const& options);

  static CURL* GetHandle(CurlHandle& h) { return h.handle_.get(); }
  static void ResetHandle(CurlHandle& h) { h.handle_.reset(); }
  static void ReleaseHandle(CurlHandle& h) { (void)h.handle_.release(); }
};

std::shared_ptr<CurlHandleFactory> GetDefaultCurlHandleFactory(
    ChannelOptions const& options);
std::shared_ptr<CurlHandleFactory> GetDefaultCurlHandleFactory();

/**
 * Implements the default CurlHandleFactory.
 *
 * This implementation of the CurlHandleFactory does not save handles, it
 * creates a new handle on each call to `CreateHandle()` and releases the
 * handle on `CleanupHandle()`.
 */
class DefaultCurlHandleFactory : public CurlHandleFactory {
 public:
  DefaultCurlHandleFactory() = default;
  explicit DefaultCurlHandleFactory(ChannelOptions options)
      : options_(std::move(options)) {}

  CurlPtr CreateHandle() override;
  void CleanupHandle(CurlHandle&&) override;

  CurlMulti CreateMultiHandle() override;
  void CleanupMultiHandle(CurlMulti&&) override;

  std::string LastClientIpAddress() const override {
    std::lock_guard<std::mutex> lk(mu_);
    return last_client_ip_address_;
  }

 private:
  mutable std::mutex mu_;
  std::string last_client_ip_address_;
  ChannelOptions options_;
};

/**
 * Implements a CurlHandleFactory that pools handles.
 *
 * This implementation keeps up to N handles in memory, they are only released
 * when the factory is destructed.
 */
class PooledCurlHandleFactory : public CurlHandleFactory {
 public:
  PooledCurlHandleFactory(std::size_t maximum_size, ChannelOptions options);
  explicit PooledCurlHandleFactory(std::size_t maximum_size)
      : PooledCurlHandleFactory(maximum_size, {}) {}
  ~PooledCurlHandleFactory() override;

  CurlPtr CreateHandle() override;
  void CleanupHandle(CurlHandle&&) override;

  CurlMulti CreateMultiHandle() override;
  void CleanupMultiHandle(CurlMulti&&) override;

  std::string LastClientIpAddress() const override {
    std::lock_guard<std::mutex> lk(mu_);
    return last_client_ip_address_;
  }

 private:
  std::size_t maximum_size_;
  mutable std::mutex mu_;
  std::deque<CURL*> handles_;
  std::deque<CURLM*> multi_handles_;
  std::string last_client_ip_address_;
  ChannelOptions options_;
};

}  // namespace internal
}  // namespace STORAGE_CLIENT_NS
}  // namespace storage
}  // namespace cloud
}  // namespace google

#endif  // GOOGLE_CLOUD_CPP_GOOGLE_CLOUD_STORAGE_INTERNAL_CURL_HANDLE_FACTORY_H
