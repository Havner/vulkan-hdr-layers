// VK_LAYER_drop_PQ
//
// Drops both 10-bit PQ orders (A2R10G10B10 and A2B10G10R10) from the reported
// surface formats, so a game is forced off PQ and onto scRGB fp16 if it can use
// it. Intended to be combined with VK_LAYER_scRGB (which offers the scRGB
// format); on its own it will make a PQ-only game fall back to SDR. Useful to
// route a game through the scRGB path instead of PQ.

#include "vkroots.h"
#include "layer_common.h"

namespace {

constexpr bool dropFormat(VkFormat format) {
    return format == VK_FORMAT_A2R10G10B10_UNORM_PACK32
        || format == VK_FORMAT_A2B10G10R10_UNORM_PACK32;
}

class VkInstanceOverrides {
public:
    static VkResult GetPhysicalDeviceSurfaceFormatsKHR(
        const vkroots::VkPhysicalDeviceDispatch &dispatch,
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        uint32_t *pSurfaceFormatCount,
        VkSurfaceFormatKHR *pSurfaceFormats)
	{
        return hdrlayers::dropFormats(dispatch, physicalDevice, surface,
                                      pSurfaceFormatCount, pSurfaceFormats, dropFormat);
    }

    static VkResult GetPhysicalDeviceSurfaceFormats2KHR(
        const vkroots::VkPhysicalDeviceDispatch &dispatch,
        VkPhysicalDevice physicalDevice,
        const VkPhysicalDeviceSurfaceInfo2KHR *pSurfaceInfo,
        uint32_t *pSurfaceFormatCount,
        VkSurfaceFormat2KHR *pSurfaceFormats)
	{
        return hdrlayers::dropFormats2(dispatch, physicalDevice, pSurfaceInfo,
                                       pSurfaceFormatCount, pSurfaceFormats, dropFormat);
    }
};

} // namespace

VKROOTS_DEFINE_LAYER_INTERFACES(VkInstanceOverrides, vkroots::NoOverrides);
