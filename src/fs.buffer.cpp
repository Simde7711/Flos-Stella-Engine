/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */
 
 #include "fs.buffer.hpp"
 #include "fs.device.hpp"
 
 #include <vulkan/vulkan.h>

 // std
 #include <cassert>
 #include <cstring>
 #include <iostream>
  
 namespace fs {
  
 /**
  * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
  *
  * @param instanceSize The size of an instance
  * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
  * minUniformBufferOffsetAlignment)
  *
  * @return VkResult of the buffer mapping call
  */
 VkDeviceSize FsBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
   if (minOffsetAlignment > 0) {
     return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
   }
   return instanceSize;
 }
  
 FsBuffer::FsBuffer(
     FsDevice &device,
     VkDeviceSize instanceSize,
     uint32_t instanceCount,
     VkBufferUsageFlags usageFlags,
     VkMemoryPropertyFlags memoryPropertyFlags,
     VkDeviceSize minOffsetAlignment)
     : device{device},
       instanceSize{instanceSize},
       instanceCount{instanceCount},
       usageFlags{usageFlags},
       memoryPropertyFlags{memoryPropertyFlags} {

   buffer = VK_NULL_HANDLE;     
   memory = VK_NULL_HANDLE;     
   alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
   bufferSize = alignmentSize * instanceCount;
   device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
   
   bufferID = globalBufferID++;
   std::cout << "[FsBuffer] Created buffer ID: " << bufferID << " (" << buffer << ")\n";

   device.TrackBuffer(buffer);
   device.TrackMemory(memory);
 }
  
 FsBuffer::~FsBuffer() 
 {
   unmap();

  if (buffer != VK_NULL_HANDLE) 
  {
    vkDestroyBuffer(device.device(), buffer, nullptr);
    device.UntrackBuffer(buffer);
    buffer = VK_NULL_HANDLE;
  }

  if (memory != VK_NULL_HANDLE) 
  {
    vkFreeMemory(device.device(), memory, nullptr);
    device.UntrackMemory(memory);
    memory = VK_NULL_HANDLE;
  }

  std::cout << "[FsBuffer] Destroying buffer ID: " << bufferID << " (" << buffer << ")\n";
 }
  
 /**
  * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
  *
  * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
  * buffer range.
  * @param offset (Optional) Byte offset from beginning
  *
  * @return VkResult of the buffer mapping call
  */
 VkResult FsBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
   assert(buffer && memory && "Called map on buffer before create");
   return vkMapMemory(device.device(), memory, offset, size, 0, &mapped);
 }
  
 /**
  * Unmap a mapped memory range
  *
  * @note Does not return a result as vkUnmapMemory can't fail
  */
 void FsBuffer::unmap() {
   if (mapped) {
     vkUnmapMemory(device.device(), memory);
     mapped = nullptr;
   }
 }
  
 /**
  * Copies the specified data to the mapped buffer. Default value writes whole buffer range
  *
  * @param data Pointer to the data to copy
  * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
  * range.
  * @param offset (Optional) Byte offset from beginning of mapped region
  *
  */
 void FsBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
   assert(mapped && "Cannot copy to unmapped buffer");
  
   if (size == VK_WHOLE_SIZE) {
     memcpy(mapped, data, bufferSize);
   } else {
     char *memOffset = (char *)mapped;
     memOffset += offset;
     memcpy(memOffset, data, size);
   }
 }
  
 /**
  * Flush a memory range of the buffer to make it visible to the device
  *
  * @note Only required for non-coherent memory
  *
  * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
  * complete buffer range.
  * @param offset (Optional) Byte offset from beginning
  *
  * @return VkResult of the flush call
  */
 VkResult FsBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
   VkMappedMemoryRange mappedRange = {};
   mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
   mappedRange.memory = memory;
   mappedRange.offset = offset;
   mappedRange.size = size;
   return vkFlushMappedMemoryRanges(device.device(), 1, &mappedRange);
 }
  
 /**
  * Invalidate a memory range of the buffer to make it visible to the host
  *
  * @note Only required for non-coherent memory
  *
  * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
  * the complete buffer range.
  * @param offset (Optional) Byte offset from beginning
  *
  * @return VkResult of the invalidate call
  */
 VkResult FsBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
   VkMappedMemoryRange mappedRange = {};
   mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
   mappedRange.memory = memory;
   mappedRange.offset = offset;
   mappedRange.size = size;
   return vkInvalidateMappedMemoryRanges(device.device(), 1, &mappedRange);
 }
  
 /**
  * Create a buffer info descriptor
  *
  * @param size (Optional) Size of the memory range of the descriptor
  * @param offset (Optional) Byte offset from beginning
  *
  * @return VkDescriptorBufferInfo of specified offset and range
  */
 VkDescriptorBufferInfo FsBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
   return VkDescriptorBufferInfo{
       buffer,
       offset,
       size,
   };
 }
  
 /**
  * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
  *
  * @param data Pointer to the data to copy
  * @param index Used in offset calculation
  *
  */
 void FsBuffer::writeToIndex(void *data, int index) {
   writeToBuffer(data, instanceSize, index * alignmentSize);
 }
  
 /**
  *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
  *
  * @param index Used in offset calculation
  *
  */
 VkResult FsBuffer::flushIndex(int index) { return flush(alignmentSize, index * alignmentSize); }
  
 /**
  * Create a buffer info descriptor
  *
  * @param index Specifies the region given by index * alignmentSize
  *
  * @return VkDescriptorBufferInfo for instance at index
  */
 VkDescriptorBufferInfo FsBuffer::descriptorInfoForIndex(int index) {
   return descriptorInfo(alignmentSize, index * alignmentSize);
 }
  
 /**
  * Invalidate a memory range of the buffer to make it visible to the host
  *
  * @note Only required for non-coherent memory
  *
  * @param index Specifies the region to invalidate: index * alignmentSize
  *
  * @return VkResult of the invalidate call
  */
 VkResult FsBuffer::invalidateIndex(int index) {
   return invalidate(alignmentSize, index * alignmentSize);
 }
  
 } 