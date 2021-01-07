#include "stdafx.h"
#include "NodeBit.h"

namespace Utilities
{
	namespace Node
	{
		void NodeBit::Refresh(void)
		{
			if(m_rValueInit)
			{
				m_rValueInit = false;
				m_rValueLast = m_rValue;
			}
			else
			{
				if(m_rValue==m_rValueLast)
					return;
				
				{
					CAutoLock al(m_CounterLock);
					if(m_Changes==MAXUINT32)
						m_Changes = 0;
					else
						m_Changes++;
				}

				m_rValueLast = m_rValue;				
				OnValueChanged(m_rValue);
			}
		}
	}
}
