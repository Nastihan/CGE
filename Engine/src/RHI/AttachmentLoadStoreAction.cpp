
// RHI
#include "AttachmentLoadStoreAction.h"

namespace CGE
{
	namespace RHI
	{
        AttachmentLoadStoreAction::AttachmentLoadStoreAction(
            const ClearValue& clearValue,
            AttachmentLoadAction loadAction,
            AttachmentStoreAction storeAction,
            AttachmentLoadAction loadActionStencil,
            AttachmentStoreAction storeActionStencil)
            : m_clearValue{ clearValue }
            , m_loadAction{ loadAction }
            , m_storeAction{ storeAction }
            , m_loadActionStencil{ loadActionStencil }
            , m_storeActionStencil{ storeActionStencil } {}

        bool AttachmentLoadStoreAction::operator==(const AttachmentLoadStoreAction& other) const
        {
            return
                m_clearValue == other.m_clearValue &&
                m_loadAction == other.m_loadAction &&
                m_storeAction == other.m_storeAction &&
                m_loadActionStencil == other.m_loadActionStencil &&
                m_storeActionStencil == other.m_storeActionStencil;
        }
	}
}