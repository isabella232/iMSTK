/*=========================================================================

   Library: iMSTK

   Copyright (c) Kitware, Inc. & Center for Modeling, Simulation,
   & Imaging in Medicine, Rensselaer Polytechnic Institute.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

=========================================================================*/

#ifndef imstkVulkanMaterial_h
#define imstkVulkanMaterial_h

#include "vulkan/vulkan.h"
#include "glm/glm.hpp"

#include "imstkRenderMaterial.h"
#include "imstkVulkanUniformBuffer.h"
#include "imstkVulkanTextureDelegate.h"

#include <memory>
#include <vector>

namespace imstk
{
class VulkanRenderer;

// Large struct to contain pipeline components for later pipeline creation
struct VulkanMaterialPipelineComponents
{
    VkShaderModule fragmentShader;
    VkShaderModule tessellationControlShader;
    VkShaderModule tessellationEvaluationShader;
    VkShaderModule vertexShader;
    std::vector<VkPipelineShaderStageCreateInfo> shaderInfo;
    std::vector<VkSpecializationMapEntry> fragmentMapEntries;
    VkSpecializationInfo fragmentSpecializationInfo;
    std::vector<VkVertexInputBindingDescription> vertexBindingDescription;
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescription;
    VkPipelineVertexInputStateCreateInfo vertexInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineTessellationStateCreateInfo tessellationInfo;
    std::vector<VkViewport> viewports;
    std::vector<VkRect2D> scissors;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
};

struct VulkanMaterialConstants
{
    unsigned int numLights;
    bool tessellation;
    bool diffuseTexture;
    bool normalTexture;
    bool specularTexture;
    bool roughnessTexture;
    bool metalnessTexture;
    bool subsurfaceScatteringTexture;
    bool irradianceCubemapTexture;
};

class VulkanMaterialDelegate
{
public:
    ///
    /// \brief Constructor
    ///
    VulkanMaterialDelegate(
        std::shared_ptr<VulkanUniformBuffer> vertexUniformBuffer,
        std::shared_ptr<VulkanUniformBuffer> fragmentUniformBuffer,
        std::shared_ptr<RenderMaterial> material,
        VulkanMemoryManager& memoryManager);

protected:
    friend class VulkanRenderer;

    ///
    /// \brief Creates a parent pipeline object that gets inherited by other materials
    ///
    void createPipeline(VulkanRenderer * renderer);

    void initializeTextures(VulkanRenderer * renderer);

    void initialize(VulkanRenderer * renderer);

    void addSpecializationConstant(uint32_t size, uint32_t offset);
    uint32_t m_numConstants = 0;
    VulkanMaterialConstants m_constants;

    uint32_t m_numTextures = 0;

    void createDescriptors(VulkanRenderer * renderer);
    void createDescriptorSetLayouts(VulkanRenderer * renderer);
    void createDescriptorPool(VulkanRenderer * renderer);
    void createDescriptorSets(VulkanRenderer * renderer);

    std::shared_ptr<RenderMaterial> m_material;

    VkPipeline m_pipeline;
    VkGraphicsPipelineCreateInfo m_graphicsPipelineInfo;
    VkPipelineLayout m_pipelineLayout;
    VulkanMaterialPipelineComponents m_pipelineComponents;

    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
    std::vector<VkWriteDescriptorSet> m_writeDescriptorSets;

    std::shared_ptr<VulkanUniformBuffer> m_vertexUniformBuffer;
    std::shared_ptr<VulkanUniformBuffer> m_fragmentUniformBuffer;

    std::shared_ptr<VulkanTextureDelegate> m_diffuseTexture;
    std::shared_ptr<VulkanTextureDelegate> m_normalTexture;
    std::shared_ptr<VulkanTextureDelegate> m_roughnessTexture;
    std::shared_ptr<VulkanTextureDelegate> m_metalnessTexture;
    std::shared_ptr<VulkanTextureDelegate> m_subsurfaceScatteringTexture;
    std::shared_ptr<VulkanTextureDelegate> m_irradianceCubemapTexture;
    std::shared_ptr<VulkanTextureDelegate> m_radianceCubemapTexture;

    VulkanMemoryManager * m_memoryManager;
};
}

#endif