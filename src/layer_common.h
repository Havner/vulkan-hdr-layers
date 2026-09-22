#pragma once

#include "vkroots.h"

#include <algorithm>
#include <vector>

// Shared helpers for the surface-format-manipulating layers.
//
// All three layers do the same thing: fetch the surface format list from the
// next layer/driver, transform it (add or drop entries), and hand it back to
// the caller honouring the two-call (count-query then fill) protocol.

namespace hdrlayers {

// Fetch the full VkSurfaceFormatKHR list from the next layer/driver.
inline VkResult queryFormats(const vkroots::VkPhysicalDeviceDispatch &dispatch,
                             VkPhysicalDevice physicalDevice,
                             VkSurfaceKHR surface,
                             std::vector<VkSurfaceFormatKHR> &out)
{
    uint32_t count = 0;
    VkResult res = dispatch.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
    if (res != VK_SUCCESS)
        return res;

    out.resize(count);
    res = dispatch.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, out.data());
    if (res == VK_SUCCESS || res == VK_INCOMPLETE)
        out.resize(count);
    return res;
}

// Fetch the full VkSurfaceFormat2KHR list from the next layer/driver.
inline VkResult queryFormats2(const vkroots::VkPhysicalDeviceDispatch &dispatch,
                              VkPhysicalDevice physicalDevice,
                              const VkPhysicalDeviceSurfaceInfo2KHR *pSurfaceInfo,
                              std::vector<VkSurfaceFormat2KHR> &out)
{
    uint32_t count = 0;
    VkResult res = dispatch.GetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, &count, nullptr);
    if (res != VK_SUCCESS)
        return res;

    out.assign(count, VkSurfaceFormat2KHR{ VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR, nullptr, {} });
    res = dispatch.GetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, &count, out.data());
    if (res == VK_SUCCESS || res == VK_INCOMPLETE)
        out.resize(count);
    return res;
}

// Write a prepared list back to the caller, honouring the two-call protocol.
template <typename T>
inline VkResult writeFormats(const std::vector<T> &items, uint32_t *pCount, T *pOut)
{
    if (!pOut) {
        *pCount = static_cast<uint32_t>(items.size());
        return VK_SUCCESS;
    }

    const uint32_t n = std::min<uint32_t>(*pCount, static_cast<uint32_t>(items.size()));
    for (uint32_t i = 0; i < n; i++)
        pOut[i] = items[i];
    *pCount = n;
    return n < items.size() ? VK_INCOMPLETE : VK_SUCCESS;
}

// Fetch, drop every format for which drop(format) is true, and write back.
template <typename Pred>
inline VkResult dropFormats(const vkroots::VkPhysicalDeviceDispatch &dispatch,
                            VkPhysicalDevice physicalDevice,
                            VkSurfaceKHR surface,
                            uint32_t *pCount,
                            VkSurfaceFormatKHR *pOut,
                            Pred drop)
{
    std::vector<VkSurfaceFormatKHR> all;
    VkResult res = queryFormats(dispatch, physicalDevice, surface, all);
    if (res != VK_SUCCESS)
        return res;

    std::vector<VkSurfaceFormatKHR> kept;
    kept.reserve(all.size());
    for (const auto &f : all)
        if (!drop(f.format))
            kept.push_back(f);

    return writeFormats(kept, pCount, pOut);
}

template <typename Pred>
inline VkResult dropFormats2(const vkroots::VkPhysicalDeviceDispatch &dispatch,
                             VkPhysicalDevice physicalDevice,
                             const VkPhysicalDeviceSurfaceInfo2KHR *pSurfaceInfo,
                             uint32_t *pCount,
                             VkSurfaceFormat2KHR *pOut,
                             Pred drop)
{
    std::vector<VkSurfaceFormat2KHR> all;
    VkResult res = queryFormats2(dispatch, physicalDevice, pSurfaceInfo, all);
    if (res != VK_SUCCESS)
        return res;

    std::vector<VkSurfaceFormat2KHR> kept;
    kept.reserve(all.size());
    for (const auto &f : all)
        if (!drop(f.surfaceFormat.format))
            kept.push_back(f);

    return writeFormats(kept, pCount, pOut);
}

} // namespace hdrlayers
