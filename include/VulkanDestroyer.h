#pragma once
#include <utility>
#include "VulkanFunctions.h"

//This class is almost entirely copied from
//VulkanCookbook. I still write it all myself
//in order to learn and also making some
//minor changes along the way.

//The class helps to dynamically destroy
//Vulkan objects whenever they go out of scope
//without the user having to initiate each individual
//destructor.

/*
namespace Raven
{
    //A wrapper for automatic object destruction.
    template<class OBJ>
    class VulkanDestroyer {
        public:
            VulkanDestroyer() : device(VK_NULL_HANDLE), object(VK_NULL_HANDLE){}
            VulkanDestroyer(VkDevice logicalDevice) : device(logicalDevice), object(VK_NULL_HANDLE){}
            VulkanDestroyer(VulkanDestroyer<VkDevice> const & logicalDevice) :
                device(*logicalDevice), object( VK_NULL_HANDLE ){}
            VulkanDestroyer(VkDevice logicalDevice, OBJ vulkanObject) :
                device(logicalDevice), object(vulkanObject) {}
            VulkanDestroyer(VulkanDestroyer<VkDevice> const &logicalDevice, OBJ vulkanObject) :
                       device(*logicalDevice), object(vulkanObject){}
            //Destructor for the objects.
            ~VulkanDestroyer()
            {
                if((device != VK_NULL_HANDLE) &&
                    object != VK_NULL_HANDLE)
                {
                    destroy();
                }
            }

            //Universal reference.
            VulkanDestroyer(VulkanDestroyer<OBJ> &&other) :
                VulkanDestroyer()
            {
                *this = std::move(other);
            }

            //Operator overloading.
            VulkanDestroyer& operator=(VulkanDestroyer<OBJ> &&other)
            {
                if(this != &other)
                {
                    VkDevice logicalDevice = device;
                    OBJ vulkanObject = object;
                    device = other.device;
                    object = other.object;
                    other.device = logicalDevice;
                    other.object = vulkanObject;
                }
                return *this;
            }

            OBJ & get()
            {
                return object;
            }

            OBJ * getPtr()
            {
                return &object;
            }

            OBJ & operator*()
            {
                return object;
            }

            OBJ const & operator*() const
            {
                return object;
            }

            bool operator!() const
            {
                return VK_NULL_HANDLE == object;
            }

            //The 'explicit' keyword specifies that a constructor or conversion function
            //doesn't allow implicit conversions or copy-initialization.
            explicit operator bool() const
            {
                return VK_NULL_HANDLE != object;
            }

        private:
            VulkanDestroyer(VulkanDestroyer<OBJ> const &);
            VulkanDestroyer& operator=(VulkanDestroyer<OBJ> const&);
            void destroy();

            VkDevice device;
            OBJ object;
    };

    //VkInstance specialization for VulkanDestroyer.
    template<>
    class VulkanDestroyer<VkInstance>
    {
        public:
            VulkanDestroyer():instance(VK_NULL_HANDLE){}
            VulkanDestroyer(VkInstance object) : instance(object){}
            //Destructor for the instance.
            ~VulkanDestroyer()
            {
                if(instance != VK_NULL_HANDLE)
                {
                    destroy();
                }
            }
            VulkanDestroyer(VulkanDestroyer<VkInstance> &&other)
            {
                *this = std::move(other);
            }

            VulkanDestroyer& operator=(VulkanDestroyer<VkInstance> &&other)
            {
                if(&other != this)
                {
                    instance = other.instance;
                    other.instance = VK_NULL_HANDLE;
                }
                return *this;
            }

            VkInstance &get()
            {
                return instance;
            }

            VkInstance *getPtr()
            {
                return &instance;
            }

            VkInstance & operator*()
            {
                return instance;
            }

            VkInstance const & operator*() const
            {
                return instance;
            }

            bool operator !() const
            {
                return VK_NULL_HANDLE == instance;
            }

            explicit operator bool() const
            {
                return VK_NULL_HANDLE != instance;
            }

        private:
            VulkanDestroyer(VulkanDestroyer<VkInstance> const&);
            VulkanDestroyer& operator=(VulkanDestroyer<VkInstance> const&);
            void destroy();
            VkInstance instance;
    };

    //VkDevice specialization for VulkanDestroyer.
    template<>
    class VulkanDestroyer<VkDevice>
    {
        public:
            VulkanDestroyer() : logicalDevice(VK_NULL_HANDLE){}
            VulkanDestroyer(VkDevice object) : logicalDevice(object){}

            ~VulkanDestroyer()
            {
                if(logicalDevice != VK_NULL_HANDLE)
                {
                    destroy();
                }
            }

            VulkanDestroyer(VulkanDestroyer<VkDevice> &&other)
            {
                *this = std::move(other);
            }

            VulkanDestroyer& operator=(VulkanDestroyer<VkDevice> &&other)
            {
                if(&other != this)
                {
                    logicalDevice = other.logicalDevice;
                    other.logicalDevice = VK_NULL_HANDLE;
                }
                return *this;
            }

            VkDevice &get()
            {
                return logicalDevice;
            }

            VkDevice *getPtr()
            {
                return &logicalDevice;
            }

            VkDevice & operator*()
            {
                return logicalDevice;
            }

            VkDevice const & operator*() const
            {
                return logicalDevice;
            }

            bool operator !() const
            {
                return VK_NULL_HANDLE == logicalDevice;
            }

            explicit operator bool() const
            {
                return VK_NULL_HANDLE != logicalDevice;
            }

        private:
            VulkanDestroyer(VulkanDestroyer<VkDevice> const&);
            VulkanDestroyer& operator=(VulkanDestroyer<VkDevice> const &);
            void destroy();

            VkDevice logicalDevice;
    };

    //VkSurfaceKHR specialization for VulkanDestroyer.
    template<>
    class VulkanDestroyer<VkSurfaceKHR>
    {
        public:
            VulkanDestroyer() : instance(VK_NULL_HANDLE), surface(VK_NULL_HANDLE){}
            VulkanDestroyer(VkInstance vulkanInstance) : instance(vulkanInstance), surface(VK_NULL_HANDLE){}
            VulkanDestroyer(VulkanDestroyer<VkInstance> const &vulkanInstance) :
                instance(*vulkanInstance), surface(VK_NULL_HANDLE){}
            VulkanDestroyer(VkInstance vulkanInstance, VkSurfaceKHR surfaceObject) :
                instance(vulkanInstance), surface(surfaceObject){}
            VulkanDestroyer(VulkanDestroyer<VkInstance> const &vulkanInstance, VkSurfaceKHR surfaceObject) :
                instance(*vulkanInstance), surface(surfaceObject){}
            ~VulkanDestroyer()
            {
                if((instance != VK_NULL_HANDLE) &&
                   (surface != VK_NULL_HANDLE))
                {
                    destroy();
                }
            }

            VulkanDestroyer(VulkanDestroyer<VkSurfaceKHR> &&other)
            {
                *this = std::move(other);
            }

            VulkanDestroyer& operator=(VulkanDestroyer<VkSurfaceKHR> &&other)
            {
                if(&other != this)
                {
                    instance = other.instance;
                    surface = other.surface;
                    other.instance = VK_NULL_HANDLE;
                    other.surface = VK_NULL_HANDLE;
                }
                return *this;
            }

            VkSurfaceKHR &get()
            {
                return surface;
            }

            VkSurfaceKHR *getPtr()
            {
                return &surface;
            }

            VkSurfaceKHR & operator*()
            {
                return surface;
            }

            VkSurfaceKHR const & operator*() const
            {
                return surface;
            }

            bool operator !() const
            {
                return VK_NULL_HANDLE == surface;
            }

            explicit operator bool() const
            {
                return VK_NULL_HANDLE != surface;
            }

        private:
            VulkanDestroyer(VulkanDestroyer<VkSurfaceKHR> const&);
            VulkanDestroyer & operator=(VulkanDestroyer<VkSurfaceKHR> const&);
            void destroy();

            VkInstance instance;
            VkSurfaceKHR surface;
    };

    template<class PARENT, class OBJ>
    void initVulkanDestroyer(PARENT const & parent, OBJ obj, VulkanDestroyer<OBJ> &wrapper)
    {
        wrapper = VulkanDestroyer<OBJ>(parent, obj);
    }

    template<class PARENT, class OBJ>
    void initVulkanDestroyer(PARENT const & parent, VulkanDestroyer<OBJ> & wrapper)
    {
        wrapper = VulkanDestroyer<OBJ>(parent);
    }
}

*/
