# Introduction

This repository is not a fork of:

- https://github.com/Zamundaaa/VK_hdr_layer
- https://github.com/Drakulix/VK_hdr_layer

These are separate layers, written from scratch, that solve some issues related
mostly to the NVIDIA driver. They do not implement HDR per se, only extend or
remove the driver's reported capabilities (to force games into proper
behaviour).

# Requirements

If you're on AMD you most likely don't need any of this. For NVIDIA I already
use NVIDIA 595+ which implements HDR, but it lacks a few things anyway or has
some bugs.

The `scRGB` should work just fine on KDE 6.7 (which already implements
`windows_scrgb`, although not tested by me). For a fully working `PQ/HDR10` you
might need an upcoming KDE 6.8 (for `windows_bt2100`).

For GNOME you need both those extensions which mutter doesn't implement at
all. A quick implementation of them can be found here:

https://github.com/Havner/mutter/commits/xwayland-scaling-and-hdr/

> [!NOTE]
> AI disclosure. This project has been written by Claude Opus 4.8. Contrary to
> other work I may perform with AI, this is just a quick fix. I do not claim to
> understand it. I do not claim it's written properly or that it won't blow up
> your computer. I'm not an expert on HDR in any way. I just wanted to be able
> to use HDR successfully under GNOME+NVIDIA so I "made" this. Sharing this
> cause someone might find that useful. Use at your own discretion.

# The layers

## VK\_LAYER\_scRGB

Adds `EXTENDED_SRGB_LINEAR` color space (NVIDIA Vulkan doesn't
advertise/implement it) which is then implemented using `PASS_THROUGH` and
`windows_scrgb` extension by your compositor.

Gated on `ENABLE_SCRGB_WSI=1`

This might be required for games that use `scRGB` exclusively (instead of
`PQ`). E.g. Control by Remedy.

## VK\_LAYER\_drop\_AR30

Drops `A2R10G10B10` surface format so a game is forced to choose `A2B10G10R10`.

Gated on `ENABLE_DROP_AR30_WSI=1`

I've found that something in DOOM Eternal mixes `RGB` and `BGR` (probably some
bug in NVIDIA driver). By disabling one, you can force it to exclusively use the
other one.

## VK\_LAYER\_drop\_PQ

Drops both, `A2R10G10B10` and `A2B10G10R10` surface formats so a game is forced
to use `scRGB` if it can.

Gated on `ENABLE_DROP_PQ_WSI=1`

This together with the `scRGB` can force a game into `scRGB` if it prioritizes
`PQ`, although I've yet to find a use case for that. Made for debugging DOOM
Eternal.
