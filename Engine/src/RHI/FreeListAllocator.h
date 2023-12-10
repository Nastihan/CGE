#pragma once

// RHI
#include "Allocator.h"
#include "Handle.h"

// std
#include <vector>
#include <queue>

namespace CGE
{
	namespace RHI
	{
		enum class FreeListAllocatorPolicy
		{
			// find the first match
			FirstFit = 0,
			// find the best match
			BestFit
		};

        // Free List Allocator (https://youtu.be/b1vv-z_Otr4?si=zElm00blSkBZtMY6)
        // Add this virtual address allocator to your pool class to manage allocation.
        class FreeListAllocator final : public Allocator
        {
        public:
            struct Descriptor : public Allocator::Descriptor
            {
                /// The allocation policy to use when allocating blocks.
                FreeListAllocatorPolicy m_policy = FreeListAllocatorPolicy::BestFit;
            };

            FreeListAllocator() = default;

            void Init(const Descriptor& descriptor);

            // RHI::Allocator
            void Shutdown() override;
            VirtualAddress Allocate(size_t byteCount, size_t byteAlignment) override;
            void DeAllocate(VirtualAddress allocation) override;
            void GarbageCollect() override;
            void GarbageCollectForce() override;
            size_t GetAllocationCount() const override;
            size_t GetAllocatedByteCount() const override;
            const Descriptor& GetDescriptor() const override;
            float ComputeFragmentation() const override;
            void Clone(RHI::Allocator* newAllocator)  override;

        private:
            Descriptor m_descriptor;

            struct Garbage
            {
                size_t m_addressOffset;
                size_t m_garbageCollectCycle;
            };

            bool IsGarbageReady(Garbage& garbage) const;

            void GarbageCollectInternal(const Garbage& garbage);

            // This will serve as our index
            using NodeHandle = Handle<size_t>;

            struct Node
            {
                bool IsValid() const
                {
                    return m_size != 0;
                }

                NodeHandle m_nextFree;
                size_t m_addressOffset = 0;
                size_t m_size = 0;
            };

            NodeHandle CreateNode();
            void ReleaseNode(NodeHandle handle);

            NodeHandle InsertNode(NodeHandle handlePrevious, size_t address, size_t size);
            void RemoveNode(NodeHandle handlePrevious, NodeHandle handleCurrent);

            struct FindNodeRequest
            {
                size_t m_requestedSize = 0;
                size_t m_requestedAlignment = 0;
            };

            struct FindNodeResponse
            {
                size_t m_leftoverSize = 0;
                NodeHandle m_handlePrevious;
                NodeHandle m_handleFound;
            };

            void FindNode(const FindNodeRequest& request, FindNodeResponse& response);
            void FindNodeBest(const FindNodeRequest& request, FindNodeResponse& response);
            void FindNodeFirst(const FindNodeRequest& request, FindNodeResponse& response);

            size_t GetRequiredSize(size_t requestedSize, size_t requestedAlignment, size_t addressOffset);

            NodeHandle GetFirstFreeHandle() const;
            const Node& GetNode(NodeHandle handle) const;
            Node& GetNode(NodeHandle handle);

            struct Allocation
            {
                size_t m_byteCount;
                size_t m_byteCountAlignmentPad;
            };

            void FreeInternal(size_t address, const Allocation& allocation);

        private:
            // First index
            NodeHandle m_headHandle;
            std::vector<NodeHandle> m_nodeFreeList;
            std::vector<Node> m_nodes;
            std::unordered_map<size_t, Allocation> m_allocations;
            std::queue<Garbage> m_garbage;
            size_t m_garbageCollectCycle = 0;
            size_t m_byteCountTotal = 0;
        };
	}
}