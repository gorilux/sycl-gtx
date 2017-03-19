#pragma once

// Device buffer accessors
// 3.4.6 Accessors and 3.4.6.4 Buffer accessors

#include "SYCL/access.h"
#include "SYCL/accessor.h"
#include "SYCL/accessors/buffer_base.h"
#include "SYCL/accessors/device_reference.h"
#include "SYCL/detail/common.h"
#include "SYCL/detail/data_ref.h"
#include "SYCL/detail/src_handlers/register_resource.h"
#include "SYCL/ranges/id.h"

namespace cl {
namespace sycl {
namespace detail {

SYCL_ACCESSOR_CLASS(target == access::target::constant_buffer ||
                    target == access::target::global_buffer)
, public accessor_buffer<DataType, dimensions>,
    public accessor_device_ref<dimensions, DataType, dimensions, mode, target> {
 private:
  template <int, typename, int, access::mode, access::target>
  friend class accessor_device_ref;

  using return_t = typename acc_device_return<DataType>::type;
  using base_acc_buffer = accessor_buffer<DataType, dimensions>;
  using base_acc_device_ref =
      accessor_device_ref<dimensions, DataType, dimensions, mode, target>;

 public:
  accessor_detail(cl::sycl::buffer<DataType, dimensions> & bufferRef,
                  handler & commandGroupHandler, range<dimensions> offset,
                  range<dimensions> range)
      : base_acc_buffer(bufferRef, &commandGroupHandler, offset, range),
        base_acc_device_ref(this, {}) {}
  accessor_detail(cl::sycl::buffer<DataType, dimensions> & bufferRef,
                  handler & commandGroupHandler)
      : accessor_detail(bufferRef, commandGroupHandler,
                        detail::empty_range<dimensions>(),
                        bufferRef.get_range()) {}
  accessor_detail(const accessor_detail& copy)
      : base_acc_buffer(static_cast<const base_acc_buffer&>(copy)),
        base_acc_device_ref(this, copy) {}
  accessor_detail(accessor_detail && move) noexcept
      : base_acc_buffer(std::move(static_cast<base_acc_buffer&&>(move))),
        base_acc_device_ref(
            this, std::move(static_cast<base_acc_device_ref&&>(move))) {}

  virtual cl_mem get_cl_mem_object() const override {
    return base_acc_buffer::get_buffer_object();
  }

  return_t operator[](id<dimensions> index) const {
    auto resource_name = kernel_ns::register_resource(*this);
    return return_t(resource_name + "[" + data_ref::get_name(index) + "]");
  }

 private:
  using subscript_return_t =
      typename subscript_helper<dimensions, DataType, dimensions, mode,
                                target>::type;

 public:
  SYCL_DEVICE_REF_SUBSCRIPT_OPERATORS(base_acc_device_ref::);

 protected:
  virtual void* resource() const override { return base_acc_buffer::buf; }

  virtual ::size_t argument_size() const override { return sizeof(cl_mem); }
};

}  // namespace detail
}  // namespace sycl
}  // namespace cl
