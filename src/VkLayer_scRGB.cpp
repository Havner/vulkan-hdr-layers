// VK_LAYER_scRGB
//
// Adds the VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT (scRGB) surface format that
// the NVIDIA driver never advertises on a Wayland surface. Games (e.g. Control)
// can then select scRGB; Proton's winewayland maps it to PASS_THROUGH and tags
// the surface with the windows_scrgb image description, so no scRGB colorspace
// needs to exist in the driver itself. Requires a compositor that supports the
// windows_scrgb feature (mutter >= our patch, KWin >= 6.7).

#include "vkroots.h"
#include "layer_common.h"

namespace {

constexpr VkSurfaceFormatKHR kScrgbFormat = {
    VK_FORMAT_R16G16B16A16_SFLOAT,
    VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT,
};

class VkInstanceOverrides {
public:
    static VkResult GetPhysicalDeviceSurfaceFormatsKHR(
        const vkroots::VkPhysicalDeviceDispatch &dispatch,
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        uint32_t *pSurfaceFormatCount,
        VkSurfaceFormatKHR *pSurfaceFormats)
	{
        std::vector<VkSurfaceFormatKHR> formats;
        VkResult res = hdrlayers::queryFormats(dispatch, physicalDevice, surface, formats);
        if (res != VK_SUCCESS)
            return res;

        const bool present = std::any_of(formats.begin(), formats.end(), [](const VkSurfaceFormatKHR &f) {
            return f.format == kScrgbFormat.format && f.colorSpace == kScrgbFormat.colorSpace;
        });
        if (!present)
            formats.push_back(kScrgbFormat);

        return hdrlayers::writeFormats(formats, pSurfaceFormatCount, pSurfaceFormats);
    }

    static VkResult GetPhysicalDeviceSurfaceFormats2KHR(
        const vkroots::VkPhysicalDeviceDispatch &dispatch,
        VkPhysicalDevice physicalDevice,
        const VkPhysicalDeviceSurfaceInfo2KHR *pSurfaceInfo,
        uint32_t *pSurfaceFormatCount,
        VkSurfaceFormat2KHR *pSurfaceFormats)
	{
        std::vector<VkSurfaceFormat2KHR> formats;
        VkResult res = hdrlayers::queryFormats2(dispatch, physicalDevice, pSurfaceInfo, formats);
        if (res != VK_SUCCESS)
            return res;

        const bool present = std::any_of(formats.begin(), formats.end(), [](const VkSurfaceFormat2KHR &f) {
            return f.surfaceFormat.format == kScrgbFormat.format
                && f.surfaceFormat.colorSpace == kScrgbFormat.colorSpace;
        });
        if (!present)
            formats.push_back(VkSurfaceFormat2KHR{ VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR, nullptr, kScrgbFormat });

        return hdrlayers::writeFormats(formats, pSurfaceFormatCount, pSurfaceFormats);
    }
};

} // namespace

VKROOTS_DEFINE_LAYER_INTERFACES(VkInstanceOverrides, vkroots::NoOverrides);
