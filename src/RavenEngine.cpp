#include "RavenEngine.h"
#include "VulkanUtility.h"
#include "VulkanStructures.h"
#include "Settings.h"
#include "CommandBufferManager.h"
#include "VulkanDescriptorManager.h"

namespace Raven
{
    //List desired instance extensions. Instance level extensions
    //will be responsible for managing a presentation surface.
    std::vector<const char*> desiredInstanceExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        PLATFORM_SURFACE_EXTENSION
    };

    //List of desired device extensions
    std::vector<const char*> desiredDeviceExtensions =
    {
        //Enable swapchains to be able to display images on screen
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    RavenEngine::RavenEngine()
    {
        selectedInstance = VK_NULL_HANDLE;
        selectedPhysicalDevice = VK_NULL_HANDLE;
    }

    RavenEngine::~RavenEngine()
    {

        //Destroy the application window
        if(appWindow != nullptr)
        {
            //Destroy window swapchain.
            destroySwapchain(vulkanDevice->getLogicalDevice(), appWindow->getSwapchain());

            //Destroy the presentation surface.
            destroyPresentationSurface(selectedInstance, appWindow->getPresentationSurface());

            delete appWindow;
        }

        //Destroy the VulkanDevice
        if(vulkanDevice != nullptr)
        {
            //Wait until the device/devices are idle before proceeding to deletion.
            waitUntilDeviceIdle(vulkanDevice->getLogicalDevice());
            //vulkanDevice.reset();
            delete vulkanDevice;
        }

        //Destroy the vulkan instance
        if(selectedInstance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(selectedInstance, nullptr);
            selectedInstance = VK_NULL_HANDLE;
        }

        //Lastly free the dynamically loaded vulkan library
        if(libraryInitialized)
            freeVulkanLibrary(vulkanLibrary);
    }

    /**
     * @brief Starts the raven engine.
     * @param appName Name of the application.
     * @return Returns false if something went wrong, true when the app closes.
     */
    bool RavenEngine::start(const char* appName)
    {
        if(!appName || strlen(appName) == 0)
        {
            std::cerr << "Application requires a name!" << std::endl;
            return false;
        }

        //First initialize vulkan if it has not been initialized yet.
        if(!initializeVulkan())
            return false;

        //After vulkan dynamic library, exported- and global-level functions have been loaded,
        //create a new vulkan instance.
        if(!createVulkanInstance(desiredInstanceExtensions, appName, selectedInstance))
            return false;

        //After instance has been created, load instance level funcions
        if(!loadInstanceLevelVulkanFunctions(selectedInstance, desiredInstanceExtensions))
            return false;

        //Next it is time to choose which Vulkan device (usually a gpu) we are going to use.
        //First check what options we have
        if(!loadPhysicalDevices(selectedInstance, physicalDevices))
            return false;

        //Select the physical device that the program will be using. Test that
        //the physical device supports desired device extensions, which will be used
        //when creating a logical device.
        if(!selectPhysicalDevice(physicalDevices, selectedPhysicalDevice, desiredDeviceExtensions))
            return false;

        //Create a logical device out of the selected physical device.
        //Raven will be built to support multiple queues, that will all be used simultaneously
        //with added synchronization. This is to not tax one queue unecessarily.
        vulkanDevice = new VulkanDevice();

        if(!createVulkanDevice(selectedPhysicalDevice, desiredDeviceExtensions))
            return false;

        //After the vulkan device has been created we need to create a window
        //for the application. This window will display our rendering content.
        //A new window will also initialize a new swapchain for the window.
        VkPresentModeKHR presentationMode = VK_PRESENT_MODE_MAILBOX_KHR;
        VkImageUsageFlags desiredImageUsages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE;

        //Open a new window.
        appWindow = new VulkanWindow();
        if(!openNewWindow(windowWidth, windowHeight, desiredImageUsages, presentationMode, oldSwapchain, appWindow))
            return false;

        auto imageViews = appWindow->getImageViews();
        //Create an object which we want to render. In the future objects will be included
        //inside scenarios which can hold multiple objects but for now, just load a single object.
        std::string objectSource = "../Resources/Models/torus.obj";
        uint32_t stride;

        GraphicsObject* graphicsObject = new GraphicsObject();
        if(!graphicsObject->loadModel(vulkanDevice->getLogicalDevice(), objectSource, true, false, false, true, &stride))
            return false;

        //Create a command buffer for the vertex data transfer.
        VkCommandBuffer vertexCmdBuffer;
        std::vector<VkCommandBuffer> vertexCmdBufferVector = {vertexCmdBuffer};
        if(!CommandBufferManager::createCmdPoolAndBuffers(vulkanDevice->getLogicalDevice(),
                                                          vulkanDevice->getPrimaryQueueFamilyIndex(),
                                                          cmdPool, 1, vertexCmdBufferVector))
        {
                return false;
        }

        //Create vertex buffer from the object data.
        VulkanBuffer vertexBuffer;
        VkDeviceMemory vertexMemory;
        if(!buildVertexDataForShaders(*graphicsObject, vertexBuffer, vertexMemory, vertexCmdBufferVector[0]))
            return false;

        //Next create the uniform buffer that will hold the matrix data used for camera and model viewing.
        //The size of it will be 2x16x sizeof(float)
        VulkanBuffer uniformBufferObject;
        VkDeviceMemory uniformBufferMemory;
        if(!vulkanDevice->createUniformBuffer((2 * 16 * sizeof(float)),
                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                              uniformBufferObject, uniformBufferMemory))
        {
            return false;
        }

        //Describe the data that is sent to the graphics pipeline. Definition can be found in the function.
        //Handles for the descriptor data.
        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;
        VkDescriptorPool descriptorPool;

        if(!buildDescriptors(descriptorSetLayout, descriptorPool, uniformBufferObject.buffer, descriptorSets))
            return false;

        //Create a renderer.
        vulkanRenderer = new VulkanRenderer();

        //TODO: Not working yet, possibly a problem with swapchain.
        /*
        //Next build the render passes. Drawing commands are organized into render passes.
        VkRenderPass renderPass;
        if(!buildRenderPass(vulkanRenderer, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_D16_UNORM, renderPass))
            return false;


        //Build the graphics pipeline that processes the data described by descriptors and
        //sends output to the renderer which renders it on the swapchain/window.

        VulkanPipelineManager pipelineManager;
        VkPipeline graphicsPipeline;
        if(!buildGraphicsPipeline(pipelineManager, descriptorSetLayout, renderPass, graphicsPipeline))
            return false;
        //Build the command buffers.
        //if(!buildCommandBuffersForDrawingGeometry())
        //    return false;
        */
        //Start rendering.
        vulkanRenderer->render(appWindow);

        return true;
    }

    /**
     * @brief Renders to the application window.
     * @return False if something went wrong, true when the rendering has finished.
     */
    bool RavenEngine::render()
    {

        //This is just a test case for submitting commands to device queues.
        //This function does nothing of value other than works as an example for now.
        VkFence submitFence;
        createFence(vulkanDevice->getLogicalDevice(), VK_FALSE, submitFence);
        VkSemaphore signaledSemaphore;
        createSemaphore(vulkanDevice->getLogicalDevice(), signaledSemaphore);

        VkSubmitInfo submitInfo = VulkanStructures::submitInfo(drawBuffers, {},{},{signaledSemaphore});

        //Submit commands to the vulkan device for execution:
        if(!vulkanDevice->executeCommands(submitInfo, submitFence))
            return false;

        //Wait for the fence to be signaled and delete the synchronization objects.
        //Note that in a normal case the application shouldn't stop to wait for the fence
        //to be signaled but should instead do other tasks and check the fence status
        //every now and then and when the fence is signaled, then continue with the task.
        if(!waitForFences(vulkanDevice->getLogicalDevice(), 100000000, VK_TRUE, {submitFence}))
            return false;

        //Check the fence status:
        if(!isFenceSignaled(vulkanDevice->getLogicalDevice(), submitFence))
            return false;

        destroyFence(vulkanDevice->getLogicalDevice(), submitFence);
        destroySemaphore(vulkanDevice->getLogicalDevice(), signaledSemaphore);

        //Destroying the command pool will destroy all the command buffers allocated from it.
        CommandBufferManager::destroyCommandPool(vulkanDevice->getLogicalDevice(), cmdPool);

        return true;
    }

    /**
     * @brief Initializes the vulkan dynamic library.
     * @return Returns false if something went wrong.
     */
    bool RavenEngine::initializeVulkan()
    {
        if(!loadVulkanLibrary(vulkanLibrary))
        {
            std::cerr << "Failed to load vulkan dynamic library!" << std::endl;
            return false;
        }
        //Load vulkan functions that are defined in ListOfVulkanFunctions.inl
        if(!loadFunctionExportedFromVulkanLoaderLibrary(vulkanLibrary))
        {
            std::cerr << "Failed to load functions exported from vulkan loader library!" << std::endl;
            return false;
        }
        if(!loadGlobalLevelFunctions())
        {
            std::cerr << "Failed to load global level functions!" << std::endl;
            return false;
        }
        libraryInitialized = true;
        return true;
    }

    /**
     * @brief Selects the first physical device that fills all given requirements.
     * @param physicalDevices A vector containing all physical devices available.
     * @param selectedDevice The selected physical device.
     * @param desiredDeviceExtensions Desired device level extensions.
     * @return False if something went wrong.
     */
    bool RavenEngine::selectPhysicalDevice(std::vector<VkPhysicalDevice> &physicalDevices,
                                           VkPhysicalDevice &selectedDevice,
                                           std::vector<const char*> &desiredDeviceExtensions)
    {
        //Check if desired extensions are supported by the physical devices. Ditch those
        //that don't support the required extensions.
        for(int i = 0; i < physicalDevices.size(); i++)
        {
            if(arePhysicalDeviceExtensionsSupported(physicalDevices[i], desiredDeviceExtensions))
            {
                //We will be using the first physical device that passes all requirements given to it.
                selectedDevice = physicalDevices[i];
                return true;
            }
        }
        std::cerr << "Failed to find a physical device that "
                     "supports all the required device extensions!" << std::endl;
        return false;
    }

    /**
     * @brief Creates a VulkanDevice from given physical device.
     * @param physicalDevice The physical device used to create the logical device.
     * @param desiredExtensions Desired device extensions for the logical device.
     * @return Returns false if something went wrong.
     */
    bool RavenEngine::createVulkanDevice(VkPhysicalDevice &physicalDevice,
                                         std::vector<const char*>  &desiredExtensions)
    {
        if(!vulkanDevice->initializeDevice(physicalDevice, desiredExtensions))
            return false;
        return true;
    }

    /**
     * @brief Creates a new VulkanWindow with a swapchain.
     *        VulkanWindow will hold swapchain and all
     *        presentation related objects.
     * @param windowWidth
     * @param windowHeight
     * @param window Handle to the VulkanWindow holding all the window information
     *        including swapchains and surfaces.
     * @return Returns false if something went wrong.
     */
    bool RavenEngine::openNewWindow(uint16_t windowWidth,
                                    uint16_t windowHeight,
                                    VkImageUsageFlags desiredImageUsage,
                                    VkPresentModeKHR &presentationMode,
                                    VkSwapchainKHR &oldSwapchain,
                                    VulkanWindow *window)
    {
            //First create the window frame.
            if(!window->createWindowFrame(windowWidth, windowHeight))
                return false;

            //Next create the window surface.
            if(!window->createWindowSurface(selectedInstance))
                return false;

            //Create a swapchain for the window for rendering content.
            if(!window->createWindowSwapchain(vulkanDevice->getLogicalDevice(),
                                              vulkanDevice->getPrimaryQueueFamilyIndex(),
                                              selectedPhysicalDevice,
                                              desiredImageUsage,
                                              presentationMode,
                                              oldSwapchain))
            {
                return false;
            }

            return true;
    }

    /**
     * @brief Creates objects required by shaders to draw geometry.
     * @return False if vertex buffers could not be created.
     */
    bool RavenEngine::buildVertexDataForShaders(GraphicsObject &graphicsObject,
                                           VulkanBuffer &vertexBufferObject,
                                           VkDeviceMemory &vertexMemory,
                                           VkCommandBuffer &cmdBuffer)
    {
        /** This function describes parts of the process of creating a vertex buffer.
            However, it is not yet fully implemented due to the lack of data. */

        //Initializing the vertex buffer object:
        //Ccreate the buffer info. Buffer size should be the size of the data that it will hold.

        //vertexBuffer.size should be the size of the data.
        vertexBufferObject.size = sizeof(graphicsObject.getMesh()->data[0])*graphicsObject.getMesh()->data.size();
        VkBufferCreateInfo bufferInfo =
            VulkanStructures::bufferCreateInfo(vertexBufferObject.size,
                                               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                               VK_SHARING_MODE_EXCLUSIVE);

        //Create the actual buffer.
        if(!createBuffer(vulkanDevice->getLogicalDevice(), bufferInfo, vertexBufferObject.buffer))
            return false;

        //Buffers and images don't have a memory backing so we need to allocate the memory for them.
        //Get buffer memory requirements and allocate the memory.
        VkMemoryRequirements memReq;
        vkGetBufferMemoryRequirements(vulkanDevice->getLogicalDevice(), vertexBufferObject.buffer, &memReq);

        //Get the correct physical device memory type index for the buffer:
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(selectedPhysicalDevice, &memoryProperties);

        //Allocate the memory:
        if(!allocateMemory(vulkanDevice->getLogicalDevice(), memoryProperties,memReq,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexMemory))
        {
            return false;
        }

        if(!vertexBufferObject.bindMemoryObject(vulkanDevice->getLogicalDevice(), vertexMemory))
            return false;
        
        //Update the object data to the vertex buffer using a staging buffer.
        //Copy the data into the staging buffer and map it to the vertex buffer.
        if(!updateDeviceLocalMemoryBuffer(vulkanDevice->getLogicalDevice(), &graphicsObject.getMesh()->data[0],
                                          vertexBufferObject.size, memoryProperties, vertexBufferObject.buffer,
                                          0, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, vulkanDevice->getQueueHandles()[0],
                                          cmdBuffer, {}))
        {
            return false;
        }

        return true;
    }

    /**
     * @brief Allocates the application command pools, buffers and records the actions.
     * @return False if something went wrong.
     */
    bool RavenEngine::buildCommandBuffersForDrawingGeometry()
    {
        /**
         * Create a new command buffer for each framebuffer object. This could
         * possibly be optimized by creating multiple threads and command pools.
         * For future examples I am going to implement just one command pool
         * and three buffers for now to document how it is done. I will get back
         * to this function once I have implemented the framebuffers.
        */

        //Record actions to each individual buffer:
        /*for(int i = 0; i < commandBuffers.size(); i++)
        {
            if(!CommandBufferManager::beginCommandBuffer(commandBuffers[i],
                                                         VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
            {
                std::cerr << "Failed to start command buffer recording for buffer number: " <<i <<std::endl;
                return false;
            }

            //Commands that you want to record should be placed here.

            if(!CommandBufferManager::endCommandBuffer(commandBuffers[i]))
            {
                std::cerr << "Failed to end command buffer number: " << i << std::endl;
                return false;
            }
        }
        */
        return true;
    }

    /**
     * @brief Describes the data that is sent to the shaders.
     * @param descriptorSetLayout
     * @param descriptorPool
     * @param descriptorSets
     * @return
     */
    bool RavenEngine::buildDescriptors(VkDescriptorSetLayout &descriptorSetLayout,
                                       VkDescriptorPool &descriptorPool,
                                       VkBuffer &uniformBuffer,
                                       std::vector<VkDescriptorSet> &descriptorSets)
    {
        //Create descriptor info so that the uniform buffer can be accessed inside the vertex shader.
        VkDescriptorSetLayoutBinding descriptorSetLayoutBinding =
        {
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            1,
            VK_SHADER_STAGE_VERTEX_BIT,
            nullptr
        };

        //Create the descriptor set layout.
        if(!VulkanDescriptorManager::createDescriptorSetLayout(vulkanDevice->getLogicalDevice(),
                                                               {descriptorSetLayoutBinding},
                                                               descriptorSetLayout))
        {
            return false;
        }

        //Next create the descriptor pool
        VkDescriptorPoolSize descriptorPoolSize =
        {
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            1
        };

        if(!VulkanDescriptorManager::createDescriptorPool(vulkanDevice->getLogicalDevice(), VK_FALSE, 1,
                                                          {descriptorPoolSize}, descriptorPool))
        {
            return false;
        }

        //Now that the pool has been created, allocate descriptor sets from it.
        if(!VulkanDescriptorManager::allocateDescriptorSets(vulkanDevice->getLogicalDevice(), descriptorPool,
                                                            {descriptorSetLayout}, descriptorSets))
        {
            return false;
        }

        BufferDescriptorInfo bufferDescriptorUpdate =
        {
            descriptorSets[0],
            0,
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            {
                {
                    uniformBuffer,
                    0,
                    VK_WHOLE_SIZE
                }
            }
        };

        VulkanDescriptorManager::updateDescriptorSets(vulkanDevice->getLogicalDevice(),{},
                                                      {bufferDescriptorUpdate},{},{});

        return true;
    }

    /**
     * @brief RavenEngine::buildRenderPass
     * @param vulkanRenderer
     * @return
     */
    bool RavenEngine::buildRenderPass(VulkanRenderer *vulkanRenderer, VkFormat swapchainFormat,
                                      VkFormat depthFormat, VkRenderPass &renderPass)
    {
        //Describe attachments. We will be using color and depth images
        //so we need two attachments.
        VkAttachmentDescription attachment1 =
        {
            0,                                  //Flags.
            swapchainFormat,                    //Format.
            VK_SAMPLE_COUNT_1_BIT,              //Samples.
            VK_ATTACHMENT_LOAD_OP_CLEAR,        //LoadOperation.
            VK_ATTACHMENT_STORE_OP_STORE,       //StoreOp.
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,    //StencilLoadOp.
            VK_ATTACHMENT_STORE_OP_DONT_CARE,   //StencilStoreOp.
            VK_IMAGE_LAYOUT_UNDEFINED,          //InitialLayout.
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR     //FinalLayout.
        };

        VkAttachmentDescription attachment2 =
        {
            0,
            depthFormat,
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };
        std::vector<VkAttachmentDescription> attachmentDescriptions = {attachment1, attachment2};

        VkAttachmentReference depthAttachment =
        {
          1,
          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };

        //Next describe the subpass parameters.
        std::vector<SubpassParameters> subpassParameters = {
        {
            VK_PIPELINE_BIND_POINT_GRAPHICS,              //VkPipelineBindPoint.
            {},                                           //InputAttachments.
            {                                             //ColorAttachments.
              {
                0,                                          //Attachment.
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL    //Layout.
              }
            },
            {},                                           //ResolveAttachments.
            &depthAttachment,                            //DepthStencilAttachment.
            {}                                            //PreserveAttachments.
          }
        };

        //Then the subpass dependencies.
        std::vector<VkSubpassDependency> subpassDependencies =
        {
            {
                VK_SUBPASS_EXTERNAL,                            //Source subpass.
                0,                                              //Destination subpass.
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              //Source stage mask.
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  //Destination stage mask.
                VK_ACCESS_MEMORY_READ_BIT,                      //Source access mask.
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           //Destination access mask.
                VK_DEPENDENCY_BY_REGION_BIT                     //Dependency flags.
            },
            {
                0,
                VK_SUBPASS_EXTERNAL,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_ACCESS_MEMORY_READ_BIT,
                VK_DEPENDENCY_BY_REGION_BIT
            }
        };

        //TODO: THIS FAILS.
        //Create the render pass.
        if(!vulkanRenderer->createRenderPass(vulkanDevice->getLogicalDevice(),
                                             attachmentDescriptions,
                                             subpassParameters,
                                             subpassDependencies,
                                             renderPass))
        {
                return false;
        }

        return true;
    }

    bool RavenEngine::buildGraphicsPipeline(VulkanPipelineManager &basicGraphicsPipeline,
                                            VkDescriptorSetLayout &descriptorSetLayout,
                                            VkRenderPass &renderPass,
                                            VkPipeline& graphicsPipeline)
    {
        VkPipelineLayout pipelineLayout;
        if(!createPipelineLayout(vulkanDevice->getLogicalDevice(),
                                 {descriptorSetLayout}, {}, pipelineLayout))
        {
            return false;
        }

        std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions =
        {
            {
                0,                              //Binding.
                6 * sizeof(float),              //Stride.
                VK_VERTEX_INPUT_RATE_VERTEX     //Input rate.
            }
        };

        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions =
        {
            {
                0,                          //Location.
                0,                          //Binding.
                VK_FORMAT_R32G32B32_SFLOAT, //Format.
                0                           //Offset.
            },
            {
                1,
                0,
                VK_FORMAT_R32G32B32_SFLOAT,
                3 * sizeof(float)
            }
        };

        std::vector<VkPipelineColorBlendAttachmentState> attachmentBlendStates =
        {
            {
                false,                          // VkBool32                 blendEnable
                VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcColorBlendFactor
                VK_BLEND_FACTOR_ONE,            // VkBlendFactor            dstColorBlendFactor
                VK_BLEND_OP_ADD,                // VkBlendOp                colorBlendOp
                VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcAlphaBlendFactor
                VK_BLEND_FACTOR_ONE,            // VkBlendFactor            dstAlphaBlendFactor
                VK_BLEND_OP_ADD,                // VkBlendOp                alphaBlendOp
                VK_COLOR_COMPONENT_R_BIT |      // VkColorComponentFlags    colorWriteMask
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT
            }
        };

        std::array<float,4> blendConstants = { 1.0f, 1.0f, 1.0f, 1.0f };

        VkPipelineCache pipelineCache;
        if(!createPipelineCache(vulkanDevice->getLogicalDevice(), {}, pipelineCache))
            return false;

        std::vector<VkPipeline> pipelines = {graphicsPipeline};
        if(!basicGraphicsPipeline.createBasicGraphicsPipelines(vulkanDevice->getLogicalDevice(),
                                                               0, "../Resources/Shaders/diffuse/diffuse-vert.spv",
                                                               "../Resources/Shaders/diffuse/diffuse-frag.spv",
                                                               vertexInputBindingDescriptions,
                                                               vertexAttributeDescriptions,
                                                               VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE,
                                                               VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT,
                                                               VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE,
                                                               VK_LOGIC_OP_COPY, attachmentBlendStates,
                                                               blendConstants, pipelineLayout, renderPass,
                                                               0, VK_NULL_HANDLE, pipelineCache,
                                                               pipelines))
        {
            return false;
        }
        return true;
    }

    /**
     * @brief Builds the shader modules used by the program.
     * @param shaderModule
     * @return False if anything went wrong.
     */
    bool RavenEngine::buildShaderModules(VkShaderModule &shaderModule)
    {
		return true;
    }

}
