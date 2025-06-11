#pragma once
 
#include "fs.device.hpp"
 
// std
#include <memory>
#include <unordered_map>
#include <vector>
 
namespace fs {
 
class FsDescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(FsDevice &device) : device{device} {}
 
    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<FsDescriptorSetLayout> build() const;
 
   private:
    FsDevice &device;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };
 
  FsDescriptorSetLayout(
      FsDevice &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~FsDescriptorSetLayout();
  FsDescriptorSetLayout(const FsDescriptorSetLayout &) = delete;
  FsDescriptorSetLayout &operator=(const FsDescriptorSetLayout &) = delete;
 
  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
 
 private:
  FsDevice &device;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
 
  friend class FsDescriptorWriter;
};
 
class FsDescriptorPool {
 public:
  class Builder {
   public:
    Builder(FsDevice &device) : device{device} {}
 
    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<FsDescriptorPool> build() const;
 
   private:
    FsDevice &device;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };
 
  FsDescriptorPool(
      FsDevice &device,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~FsDescriptorPool();
  FsDescriptorPool(const FsDescriptorPool &) = delete;
  FsDescriptorPool &operator=(const FsDescriptorPool &) = delete;
 
  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;
 
  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;
 
  void resetPool();
 
 private:
  FsDevice &device;
  VkDescriptorPool descriptorPool;
 
  friend class FsDescriptorWriter;
};
 
class FsDescriptorWriter {
 public:
  FsDescriptorWriter(FsDescriptorSetLayout &setLayout, FsDescriptorPool &pool);
 
  FsDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  FsDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);
 
  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);
 
 private:
  FsDescriptorSetLayout &setLayout;
  FsDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};
 
}