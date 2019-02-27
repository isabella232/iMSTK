#-----------------------------------------------------------------------------
# Compile Shaders
#-----------------------------------------------------------------------------
function(compileShaders sourceShader binaryShader)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    COMMAND glslangvalidator -V ${PROJECT_BINARY_DIR}/Shaders/VulkanShaders/${sourceShader} -o ${PROJECT_BINARY_DIR}/Shaders/VulkanShaders/${binaryShader})
endfunction()

function(CopyAndCompileShaders)
    if( iMSTK_USE_Vulkan )
      file(COPY ${CMAKE_SOURCE_DIR}/Source/Rendering/VulkanRenderer/VulkanShaders
        DESTINATION ${PROJECT_BINARY_DIR}/Shaders)

      # Mesh shaders
      compileShaders(Mesh/mesh_vert.vert Mesh/mesh_vert.spv)
      compileShaders(Mesh/mesh_tesc.tesc Mesh/mesh_tesc.spv)
      compileShaders(Mesh/mesh_tese.tese Mesh/mesh_tese.spv)
      compileShaders(Mesh/mesh_frag.frag Mesh/mesh_frag.spv)
      compileShaders(Mesh/decal_vert.vert Mesh/decal_vert.spv)
      compileShaders(Mesh/decal_frag.frag Mesh/decal_frag.spv)
      compileShaders(Mesh/particle_vert.vert Mesh/particle_vert.spv)
      compileShaders(Mesh/particle_frag.frag Mesh/particle_frag.spv)
      compileShaders(Mesh/shadow_vert.vert Mesh/shadow_vert.spv)
      compileShaders(Mesh/shadow_frag.frag Mesh/shadow_frag.spv)
      compileShaders(Mesh/depth_frag.frag Mesh/depth_frag.spv)

      # Post processing shaders
      compileShaders(PostProcessing/HDR_tonemap_frag.frag PostProcessing/HDR_tonemap_frag.spv)
      compileShaders(PostProcessing/postprocess_vert.vert PostProcessing/postprocess_vert.spv)
      compileShaders(PostProcessing/postprocess_frag.frag PostProcessing/postprocess_frag.spv)
      compileShaders(PostProcessing/sss_frag.frag PostProcessing/sss_frag.spv)
      compileShaders(PostProcessing/lens_distortion_frag.frag PostProcessing/lens_distortion_frag.spv)
      compileShaders(PostProcessing/composite_frag.frag PostProcessing/composite_frag.spv)
      compileShaders(PostProcessing/bloom_threshold_frag.frag PostProcessing/bloom_threshold_frag.spv)
      compileShaders(PostProcessing/blur_horizontal_frag.frag PostProcessing/blur_horizontal_frag.spv)
      compileShaders(PostProcessing/blur_vertical_frag.frag PostProcessing/blur_vertical_frag.spv)
      compileShaders(PostProcessing/bloom_threshold_frag.frag PostProcessing/bloom_threshold_frag.spv)
      compileShaders(PostProcessing/ao_frag.frag PostProcessing/ao_frag.spv)
      compileShaders(PostProcessing/bilateral_blur_horizontal_frag.frag PostProcessing/bilateral_blur_horizontal_frag.spv)
      compileShaders(PostProcessing/bilateral_blur_vertical_frag.frag PostProcessing/bilateral_blur_vertical_frag.spv)
      compileShaders(PostProcessing/depth_downscale_frag.frag PostProcessing/depth_downscale_frag.spv)

      file(COPY ${PROJECT_BINARY_DIR}/Shaders/VulkanShaders
        DESTINATION ${CMAKE_PROGRAM_PATH}/Shaders)
    else( iMSTK_USE_Vulkan )
      file(COPY ${CMAKE_SOURCE_DIR}/Source/Rendering/VTKRenderer/VTKShaders
        DESTINATION ${PROJECT_BINARY_DIR}/Shaders)
      file(COPY ${CMAKE_SOURCE_DIR}/Source/Rendering/VTKRenderer/VTKShaders
        DESTINATION ${CMAKE_PROGRAM_PATH}/Shaders)
    endif()
endfunction()