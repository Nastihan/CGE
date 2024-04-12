#pragma once

// RHI
#include "RHI_Common.h"
#include "ImageSubresource.h"
#include "interval_map.h"

// std
#include <algorithm>

namespace CGE
{
	namespace RHI
	{
        // This class will be used to keep track of all sub-resource states of an image
        // The image aspect, array slice and mip level index will get flattened into a linear indexing scheme.
        // and the subresource range state will be inserted into the interval map (check interval_map.h)
        // Template used with D3D12_RESOURCE_STATES and VkImageLayout (check DX_Image.h)
        template<class T>
        class ImageProperty
        {
        public:
            // Describes the property value of one image subresource range.
            struct PropertyRange
            {
                ImageSubresourceRange m_range;
                T m_property;

                bool operator==(const PropertyRange& other) const
                {
                    return other.m_range == m_range && other.m_property == m_property;
                }
            };

            void Init(const ImageDescriptor& descriptor);
            bool IsInitialized() const;

            // Set the state of the subresource range passed in.
            void Set(const ImageSubresourceRange& range, const T& property);
            std::vector<PropertyRange> Get(const ImageSubresourceRange& range) const;
            void Reset();

        private:
            uint32_t ConvertSubresourceToIndex(ImageAspect aspect, uint16_t mipSlice, uint16_t arraySlice) const;
            ImageSubresource ConvertIndexToSubresource(uint32_t index) const;

            // Returns the interval of the subresource indices that the aspect covers.
            Interval GetAspectInterval(ImageAspect aspect) const;

            interval_map<uint32_t, T> m_intervalMap;
            ImageDescriptor m_imageDescriptor;
            bool m_initialized = false;
        };

        // Image aspect for the least significant bit set in "flags".
        inline ImageAspect GetMinAspect(ImageAspectFlags flags)
        {
            int flagsMask = static_cast<int>(flags);

            // Negating the integer will be its two's complement. (flip the bits and add one)
            // ANDing the number with its two's complement will only be the least significant bit.
            // The log2 operation will give the correct aspect.
            return static_cast<ImageAspect>(static_cast<uint32_t>(log2(flagsMask & -flagsMask)));
        }

        // Image aspect for the most significant bit set in "flags".
        inline ImageAspect GetMaxAspect(ImageAspectFlags flags)
        {
            uint32_t flagsMask = static_cast<uint32_t>(flags);
            return static_cast<ImageAspect>(static_cast<uint32_t>(log2(flagsMask)));
        }

        // Returns if the image aspects present in "aspectFlags" are consecutive.
        // (i.e. there's no disabled bits between the first and last enabled bit).
        inline bool IsContinousRange(ImageAspectFlags aspectFlags)
        {
            ImageAspect minAspect = GetMinAspect(aspectFlags);
            ImageAspect maxAspect = GetMaxAspect(aspectFlags);
            for(uint32_t i = static_cast<uint32_t>(minAspect); i < static_cast<uint32_t>(maxAspect); ++i)
            {
                if(!CheckBitsAll(static_cast<uint32_t>(aspectFlags), static_cast<uint32_t>(static_cast<ImageAspectFlags>(1u << i))))
                {
                    return false;
                }
            }
            return true;
        }

        template<class T>
        void ImageProperty<T>::Init(const ImageDescriptor& descriptor)
        {
            Reset();
            m_imageDescriptor = descriptor;
            m_initialized = true;
        }

        template<class T>
        bool ImageProperty<T>::IsInitialized() const
        {
            return m_initialized;
        }

        template<class T>
        void ImageProperty<T>::Set(const ImageSubresourceRange& range, const T& property)
        {
            assert(m_initialized, "ImageProperty has not being initialized");

            // Filter ranges and aspect image flags to what the image support.
            ImageSubresourceRange subResourceRange = range;
            subResourceRange.m_mipSliceMax = std::min(static_cast<uint16_t>(m_imageDescriptor.m_mipLevels - 1), subResourceRange.m_mipSliceMax);
            subResourceRange.m_arraySliceMax = std::min(static_cast<uint16_t>(m_imageDescriptor.m_arraySize - 1), subResourceRange.m_arraySliceMax);
            subResourceRange.m_aspectFlags = static_cast<ImageAspectFlags>(FilterBits(static_cast<uint32_t>(GetImageAspectFlags(m_imageDescriptor.m_format)), static_cast<uint32_t>(subResourceRange.m_aspectFlags)));
            const uint32_t mipSize = subResourceRange.m_mipSliceMax - subResourceRange.m_mipSliceMin + 1;
            const uint32_t arraySize = subResourceRange.m_arraySliceMax - subResourceRange.m_arraySliceMin + 1;

            // Check if the ImageSubresourceRange is continuous so we can just make one call for one interval.
            if (IsContinousRange(subResourceRange.m_aspectFlags) && m_imageDescriptor.m_mipLevels == mipSize && m_imageDescriptor.m_arraySize == arraySize)
            {
                m_intervalMap.assign(
                    ConvertSubresourceToIndex(GetMinAspect(subResourceRange.m_aspectFlags), subResourceRange.m_mipSliceMin, subResourceRange.m_arraySliceMin),
                    ConvertSubresourceToIndex(GetMaxAspect(subResourceRange.m_aspectFlags), subResourceRange.m_mipSliceMax, subResourceRange.m_arraySliceMax) + 1,
                    property);
            }
            else
            {
                // The range is not continuous so we have to go by ImageAspect.
                for (uint32_t i = 0; i < ImageAspectCount; ++i)
                {
                    if (!CheckBitsAll(static_cast<uint32_t>(subResourceRange.m_aspectFlags), static_cast<uint32_t>(static_cast<ImageAspectFlags>(1u << i))))
                    {
                        continue;
                    }

                    // Check if we can make one call for all miplevels of the image aspect.
                    ImageAspect aspect = static_cast<ImageAspect>(i);
                    if (m_imageDescriptor.m_arraySize == arraySize)
                    {
                        m_intervalMap.assign(
                            ConvertSubresourceToIndex(aspect, subResourceRange.m_mipSliceMin, subResourceRange.m_arraySliceMin),
                            ConvertSubresourceToIndex(aspect, subResourceRange.m_mipSliceMax, subResourceRange.m_arraySliceMax) + 1,
                            property);
                    }
                    else
                    {
                        // Insert intervals by mip level.
                        for (uint16_t mipLevel = subResourceRange.m_mipSliceMin; mipLevel <= subResourceRange.m_mipSliceMax; ++mipLevel)
                        {
                            m_intervalMap.assign(
                                ConvertSubresourceToIndex(aspect, mipLevel, subResourceRange.m_arraySliceMin),
                                ConvertSubresourceToIndex(aspect, mipLevel, subResourceRange.m_arraySliceMax) + 1,
                                property);
                        }
                    }
                }
            }
        }

        template<class T>
        std::vector<typename ImageProperty<T>::PropertyRange> ImageProperty<T>::Get(const ImageSubresourceRange& range) const
        {
            assert(m_initialized, "ImageProperty has not being initialized");

            // Filter ranges and aspect image flags to what the image support.
            std::vector<PropertyRange> intervals;
            ImageSubresourceRange subResourceRange = range;
            subResourceRange.m_mipSliceMax = std::min(static_cast<uint16_t>(m_imageDescriptor.m_mipLevels - 1), subResourceRange.m_mipSliceMax);
            subResourceRange.m_arraySliceMax = std::min(static_cast<uint16_t>(m_imageDescriptor.m_arraySize - 1), subResourceRange.m_arraySliceMax);
            subResourceRange.m_aspectFlags = static_cast<ImageAspectFlags>(FilterBits(static_cast<uint32_t>(GetImageAspectFlags(m_imageDescriptor.m_format)), static_cast<uint32_t>(subResourceRange.m_aspectFlags)));
            const uint32_t arraySize = subResourceRange.m_arraySliceMax - subResourceRange.m_arraySliceMin + 1;
            const uint32_t mipSize = subResourceRange.m_mipSliceMax - subResourceRange.m_mipSliceMin + 1;
            const uint32_t subresourcesPerAspect = m_imageDescriptor.m_mipLevels * m_imageDescriptor.m_arraySize;

            auto getIntervals = [&](uint32_t beginIndex, uint32_t endIndex)
            {
                auto overlapInterval = m_intervalMap.overlap(beginIndex, endIndex);
                for (auto it = overlapInterval.first; it != overlapInterval.second; ++it)
                {
                    // We may need to split the interval into multiple subresource ranges if it includes
                    // multiple image aspects but different subresource count per image aspect.
                    // For example, if the interval contains from mip 0 to 5 of aspect 'Plane1' and mip 0 to 3
                    // of 'Plane2' (which is a continuous range) we need to split into 2 ranges:
                    // Range 1 = 'Plane1' mip 0-5
                    // Range 2 = 'Plane2' mip 0-3.
                    uint32_t minIndex = std::max(beginIndex, it.interval_begin());
                    uint32_t maxIndex = std::min(endIndex, it.interval_end());
                    // Traverse the interval by image aspect level.
                    for (uint32_t index = minIndex; index < maxIndex;)
                    {
                        uint32_t aspectIndex = index / subresourcesPerAspect;
                        Interval aspectInterval = GetAspectInterval(static_cast<ImageAspect>(aspectIndex));
                        uint32_t aspectEndIndex = std::min(aspectInterval.m_max, static_cast<uint32_t>(maxIndex - 1));
                        intervals.emplace_back();
                        PropertyRange& propRange = intervals.back();
                        auto min = ConvertIndexToSubresource(index);
                        auto max = ConvertIndexToSubresource(aspectEndIndex);
                        propRange.m_range.m_mipSliceMin = std::max(min.m_mipSlice, subResourceRange.m_mipSliceMin);
                        propRange.m_range.m_mipSliceMax = std::min(max.m_mipSlice, subResourceRange.m_mipSliceMax);
                        propRange.m_range.m_arraySliceMin = std::max(min.m_arraySlice, subResourceRange.m_arraySliceMin);
                        propRange.m_range.m_arraySliceMax = std::min(max.m_arraySlice, subResourceRange.m_arraySliceMax);
                        propRange.m_range.m_aspectFlags = static_cast<ImageAspectFlags>(1u << aspectIndex);
                        propRange.m_property = static_cast<const T>(it.value());
                        index = aspectEndIndex + 1;
                    }
                }
            };

            // Check if the ImageSubresourceRange is continuous so we can just make one call for one interval.
            if (IsContinousRange(subResourceRange.m_aspectFlags) && m_imageDescriptor.m_mipLevels == mipSize && m_imageDescriptor.m_arraySize == arraySize)
            {
                getIntervals(
                    ConvertSubresourceToIndex(GetMinAspect(subResourceRange.m_aspectFlags), subResourceRange.m_mipSliceMin, subResourceRange.m_arraySliceMin),
                    ConvertSubresourceToIndex(GetMaxAspect(subResourceRange.m_aspectFlags), subResourceRange.m_mipSliceMax, subResourceRange.m_arraySliceMax) + 1);
            }
            else
            {
                // Traverse one image aspect at a time.
                for (uint32_t i = 0; i < ImageAspectCount; ++i)
                {
                    if (!CheckBitsAll(static_cast<uint32_t>(subResourceRange.m_aspectFlags), static_cast<uint32_t>(static_cast<ImageAspectFlags>(1u << i))))
                    {
                        continue;
                    }

                    // Check if we can make one call for all miplevels of the image aspect.
                    ImageAspect aspect = static_cast<ImageAspect>(i);
                    if (m_imageDescriptor.m_arraySize == arraySize)
                    {
                        getIntervals(
                            ConvertSubresourceToIndex(aspect, subResourceRange.m_mipSliceMin, subResourceRange.m_arraySliceMin),
                            ConvertSubresourceToIndex(aspect, subResourceRange.m_mipSliceMax, subResourceRange.m_arraySliceMax) + 1);
                    }
                    else
                    {
                        // Traverse one mip level at a time.
                        for (uint16_t mipLevel = subResourceRange.m_mipSliceMin; mipLevel <= subResourceRange.m_mipSliceMax; ++mipLevel)
                        {
                            getIntervals(
                                ConvertSubresourceToIndex(aspect, mipLevel, subResourceRange.m_arraySliceMin),
                                ConvertSubresourceToIndex(aspect, mipLevel, subResourceRange.m_arraySliceMax) + 1);
                        }
                    }
                }
            }

            if (intervals.size() > 1)
            {
                // Merge Image Aspects for same intervals per aspect with same property values.
                auto sortFunc = [this](const PropertyRange& lhs, const PropertyRange& rhs)
                {
                    uint32_t lhsIndex = ConvertSubresourceToIndex(static_cast<ImageAspect>(0), lhs.m_range.m_mipSliceMin, lhs.m_range.m_arraySliceMin);
                    uint32_t rhsIndex = ConvertSubresourceToIndex(static_cast<ImageAspect>(0), rhs.m_range.m_mipSliceMin, rhs.m_range.m_arraySliceMin);
                    return lhsIndex < rhsIndex;
                };
                std::sort(intervals.begin(), intervals.end(), sortFunc);
                for (uint32_t i = 0; i < intervals.size() - 1;)
                {
                    auto& current = intervals[i];
                    const auto& next = intervals[i + 1u];
                    if (current.m_range.m_mipSliceMin == next.m_range.m_mipSliceMin &&
                        current.m_range.m_mipSliceMax == next.m_range.m_mipSliceMax &&
                        current.m_range.m_arraySliceMin == next.m_range.m_arraySliceMin &&
                        current.m_range.m_arraySliceMax == next.m_range.m_arraySliceMax &&
                        current.m_property == next.m_property)
                    {
                        current.m_range.m_aspectFlags = static_cast<ImageAspectFlags>(current.m_range.m_aspectFlags | next.m_range.m_aspectFlags);
                        intervals.erase(intervals.begin() + i + 1);
                    }
                    else
                    {
                        ++i;
                    }
                }
            }
            return intervals;
        }

        template<class T>
        void ImageProperty<T>::Reset()
        {
            m_intervalMap.clear();
        }

        template<class T>
        uint32_t ImageProperty<T>::ConvertSubresourceToIndex(ImageAspect aspect, uint16_t mipSlice, uint16_t arraySlice) const
        {
            return (static_cast<uint32_t>(aspect) * m_imageDescriptor.m_arraySize * m_imageDescriptor.m_mipLevels) + mipSlice * m_imageDescriptor.m_arraySize + arraySlice;
        }

        template<class T>
        ImageSubresource ImageProperty<T>::ConvertIndexToSubresource(uint32_t index) const
        {
            const uint32_t subresourcesPerAspect = m_imageDescriptor.m_mipLevels * m_imageDescriptor.m_arraySize;
            return ImageSubresource(
                static_cast<uint16_t>((index % subresourcesPerAspect) / m_imageDescriptor.m_arraySize),
                static_cast<uint16_t>((index % subresourcesPerAspect) % m_imageDescriptor.m_arraySize),
                static_cast<ImageAspect>(index / subresourcesPerAspect));
        }

        template<class T>
        Interval ImageProperty<T>::GetAspectInterval(ImageAspect aspect) const
        {
            const uint32_t aspectIndex = static_cast<uint32_t>(aspect);
            const uint32_t subresourcesPerAspect = m_imageDescriptor.m_mipLevels * m_imageDescriptor.m_arraySize;
            const uint32_t beginIndex = aspectIndex * subresourcesPerAspect;
            return Interval(beginIndex, beginIndex + subresourcesPerAspect - 1);
        }
	}
}