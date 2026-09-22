// VK_LAYER_drop_AR30
//
// Drops VK_FORMAT_A2R10G10B10_UNORM_PACK32 from the reported surface formats so
// a game is forced onto VK_FORMAT_A2B10G10R10_UNORM_PACK32. Some games (e.g.
// DOOM Eternal) pick the reversed A2R10G10B10 order, which the NVIDIA WSI /
// compositor scanout path mishandles as A2B10G10R10, producing a red<->blue
// channel swap. The standard order is always offered alongside, so removing the
// reversed one forces the correctly-handled format.

#include "vkroots.h"
#include "layer_common.h"

namespace {

constexpr bool dropFormat(VkFormat format) {
    return format == VK_FORMAT_A2R10G10B10_UNORM_PACK32;
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
