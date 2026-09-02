#ifndef RENDERER_H
#define RENDERER_H

#include "Logger.h"
#include <QWindow>
#include <vulkan/vulkan_core.h>
#include <vector>
#include "camera.h"
#include "gameobject.h"
#include "light.h"
#include "mesh.h"


struct SwapChainSupportDetails;
struct QueueFamilyIndices;

class Renderer : public QWindow
{
    Q_OBJECT
public:
    explicit Renderer(QWindow* parent = nullptr);
    ~Renderer();

    void initVulkan();

    void setMainWindow(class MainWindow *mainWindowIn);

    void setBackgroundColor(float r, float g, float b)
    {
        mClearColor.color.float32[0] = r;
        mClearColor.color.float32[1] = g;
        mClearColor.color.float32[2] = b;
        mClearColor.color.float32[3] = 1.0f;
        this -> requestUpdate();
    }

    const std::vector<GameObject*>& getGameObjects() const { return mGameObjects; }
    void clearGameObjects() {
        for (auto obj : mGameObjects) delete obj;
        mGameObjects.clear();
    }

    Camera* getCamera() { return &mCamera; }

    class GameObject* mPlayerObject{nullptr};
    class Light* getLight() const { return mLight; }


    void setLight(Light* light) { mLight = light; }

    class GameObject* spawnObject(
        std::string meshFilename,
        std::string textureFilename,
        glm::vec3 position = {0.0f, 0.0f, 0.0f},
        glm::vec3 scale = {1.0f, 1.0f, 1.0f},
        glm::vec3 rotation = {0.0f, 0.0f, 0.0f}
        );

protected:
    //Qt event handlers - called when requestUpdate(); is called
    void exposeEvent(QExposeEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool event(QEvent* event) override;

    //Qt captures keyPresses
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    class MainWindow *mMainWindow{nullptr};

    Camera mCamera;
    std::vector<VkSemaphore> imageInFlightSemaphores;
    std::vector<class Mesh*> mAllMeshes;
    std::vector<GameObject*> mGameObjects;


    std::vector<class Texture*> mAllTextures;

    VkClearValue mClearColor = {{{0.05f, 0.05f, 0.05f, 1.0f}}};


    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkDescriptorSetLayout descriptorSetLayoutUBO{VK_NULL_HANDLE};
    VkDescriptorSetLayout descriptorSetLayoutSampler{VK_NULL_HANDLE};
    VkDescriptorPool descriptorPool{VK_NULL_HANDLE};
    std::vector<VkDescriptorSet> descriptorSetsUBO;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    VkSampler textureSampler{VK_NULL_HANDLE};

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    // ---- Above From Vulkan-Tutorial.com ----

    class Logger& mLogger;
    LineEnd endl;

    // ---- Functions from Vulkan-Tutorial.com ----
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();

    void cleanupSwapChain();
    void recreateSwapChain();

    void drawFrame();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    std::vector<const char *> getRequiredExtensions();
    bool checkValidationLayerSupport();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                      VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFiler, VkMemoryPropertyFlags properties);
    void createMeshBuffers(class Mesh* mesh);

    Light* mLight{nullptr};

    Material* mFillMateral{nullptr};
    Material* mWireframeMaterial{nullptr};

    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void updateUniformBuffer(uint32_t currentImage);


    void createTextureImage(Texture* textureIn);
    void createTextureImageView(Texture* textureIn);
    void createTextureSampler();
    void createTextureDescriptor(Texture* textureIn);


    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void cleanup();
};

#endif // RENDERER_H
