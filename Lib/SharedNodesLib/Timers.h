#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

namespace Utilities
{
	class Timer
	{
	public:
		Timer(void) 
			: m_LastVal(false), m_CounterInit(false), m_DelayON_MS(0), m_DelayOFF_MS(0)
		{
			QueryPerformanceFrequency(&m_Frequency);
			QueryPerformanceCounter(&m_Start);
		};

		Timer(UINT32 DelayON_MS, UINT32 DelayOFF_MS, bool InitVal = false) 
			: m_LastVal(InitVal), m_CounterInit(false), m_DelayON_MS(DelayON_MS), m_DelayOFF_MS(DelayOFF_MS)
		{ 
			QueryPerformanceFrequency(&m_Frequency);
			QueryPerformanceCounter(&m_Start);
		};

		~Timer(void) 
		{ };
	
		UINT32 GetMS(void)
		{
			if(m_CounterInit)
			{
				LARGE_INTEGER now;

				QueryPerformanceCounter(&now);
				double ms = static_cast<double>((now.QuadPart - m_Start.QuadPart)*1000) / m_Frequency.QuadPart;

				return static_cast<UINT32>(ms);
			}

			return 0;
		};

		void Set(void)
		{
			if(!m_CounterInit)
			{
				QueryPerformanceCounter(&m_Start);
				m_CounterInit = true;
			}
		}

		void Reset(void)
		{
			m_CounterInit = false;
		}

		bool Delayed(bool val) 
		{
			//delayON
			if(val && !m_LastVal)
			{
				if(m_DelayON_MS==0)
					m_LastVal = val;
				else
				{
					Set();

					if(GetMS() >= m_DelayON_MS)
						m_LastVal = val;
				}
			}

			//delayOFF
			if(!val && m_LastVal)
			{
				if(m_DelayOFF_MS==0)
					m_LastVal = val;
				else
				{
					Set();

					if(GetMS() >= m_DelayOFF_MS)
						m_LastVal = val;
				}
			}

			//reset
			if(val==m_LastVal)
				Reset();

			return m_LastVal;
		};

		void SetDelayON(UINT32 ms) { m_DelayON_MS = ms; }
		void SetDelayOFF(UINT32 ms) { m_DelayOFF_MS = ms; }
		void SetVal(bool val) { m_LastVal = val; }
	
	private:
		bool m_LastVal;
		bool m_CounterInit;

		UINT32 m_DelayON_MS;
		UINT32 m_DelayOFF_MS;

		LARGE_INTEGER m_Frequency;
		LARGE_INTEGER m_Start;
	};
}