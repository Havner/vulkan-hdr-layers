vkroots_dep = dependency('vkroots')

out_lib_dir  = join_paths(prefix, lib_dir)
manifest_dir = join_paths(data_dir, 'vulkan', 'implicit_layer.d')

layers = [
  'VkLayer_scRGB',
  'VkLayer_drop_AR30',
  'VkLayer_drop_PQ',
]

foreach layer : layers
  shared_library(
    layer,
    layer + '.cpp',
    dependencies : [ vkroots_dep, vulkan_dep ],
    install      : true,
  )

  configure_file(
    input         : layer + '.json.in',
    output        : layer + '.' + build_machine.cpu_family() + '.json',
    configuration : { 'family' : build_machine.cpu_family(), 'lib_dir' : out_lib_dir },
    install       : true,
    install_dir   : manifest_dir,
  )
endforeach
