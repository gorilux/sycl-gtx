#pragma once

// 3.5.1 Device selection class

#include <memory>
#include "../common.h"

namespace cl {
namespace sycl {

// Forward declarations
class context;
class device;
class platform;

// The class device_selector is an abstract class which enables the SYCL runtime to choose the best device based
// on heuristics specified by the user, or by one of the built-in device selectors
class device_selector {
protected:
	friend class context;
	friend class queue;

	static platform get_platform();
	device select_device(vector_class<device> devices) const;

	const cl_device_type type;
	device_selector(cl_device_type type)
		: type(type)
	{}
public:
	static const std::unique_ptr<device_selector> default;

	device_selector() : device_selector(CL_DEVICE_TYPE_ALL) {}
	device select_device() const;
	virtual int operator()(device dev) const = 0;
};

// TODO: Built-in device selectors

// Devices selected by heuristics of the system.
// If no OpenCL device is found then the execution is executed on the SYCL Host Mode.
struct default_selector : device_selector {
	default_selector()
		: device_selector(CL_DEVICE_TYPE_DEFAULT) {}
	virtual int operator()(device dev) const override;
};

// Select devices according to device type CL_DEVICE_TYPE_GPU from all the available OpenCL devices.
// If no OpenCL GPU device is found the selector fails.
struct gpu_selector : device_selector {
	gpu_selector()
		: device_selector(CL_DEVICE_TYPE_GPU) {}
	virtual int operator()(device dev) const override;
};

// Select devices according to device type CL_DEVICE_TYPE_CPU from all the available devices and heuristics.
// If no OpenCL CPU device is found the selector fails.
struct cpu_selector : device_selector {
	cpu_selector()
		: device_selector(CL_DEVICE_TYPE_CPU) {}
	virtual int operator()(device dev) const override;
};

// Selects the SYCL host CPU device that does not require an OpenCL runtime.
struct host_selector : device_selector {
	host_selector()
		: device_selector(CL_DEVICE_TYPE_DEFAULT) {}
	virtual int operator()(device dev) const override;
};

} // namespace sycl
} // namespace cl
