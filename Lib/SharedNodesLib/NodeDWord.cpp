#include "stdafx.h"
#include "NodeDWord.h"

namespace Utilities
{
	namespace Node
	{
		INode* NodeDWord::CreateNode(const char* Id, NodeType Type, UINT16 Offset)
		{
			return CreateNode(Id, Type, Offset, m_Access);
		}

		INode* NodeDWord::CreateNode(const char* Id, NodeType type, UINT16 Offset, NodeAccess Access)
		{
			string id(Id);
			string path(m_Id);
			path += ".";
			path += Id;

			if(id.length()==0)
			{
				throw Exc(this->Id(), "Empty id string");
			}

			switch(type)
			{
			case NodeType_Bit:
				if(Offset>31)
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg), "Bit offset >31 for node '%s'", Id);
					throw Exc(this->Id(), msg);					
				}
				break;
			case NodeType_Byte:
				if(Offset>3)
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg), "Byte offset >3 for node '%s'", Id);
					throw Exc(this->Id(), msg);
				}
				break;
			case NodeType_Word:
				if(Offset>1)
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg), "Word offset >1 for node '%s'", Id);
					throw Exc(this->Id(), msg);
				}
				break;
			case NodeType_DWord:
				if (Offset>0)
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg), "DWord offset >0 for node '%s'", Id);
					throw Exc(this->Id(), msg);
				}
				break;
			case NodeType_Real32:
				if (Offset>0)
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg), "Real32 offset >0 for node '%s'", Id);
					throw Exc(this->Id(), msg);
				}
				break;
			}

			NodeDWord::NodeDWordPtr node(new NodeDWord(id, 
													type, 
													m_Access,
													Offset, 
													m_rValue, 
													m_wValue, 
													m_rFlag,
													m_wFlag, 
													m_rLock, 
													m_wLock,
													m_rTime,
													m_wTime,
													m_rCounter,
													m_wCounter));

			m_Nodes[id] = node;

			return dynamic_cast<INode*>(node.get());
		}

		NodeDWord* NodeDWord::GetNode(const char* Id)
		{
			return dynamic_cast<NodeDWord*>(GetNodeInterface(Id));
		}

		double NodeDWord::GetNodeReadRate()
		{
			CAutoLock al(m_rLock);

			UINT64 tick = GetTickCount();

			if (m_rTick == 0)
			{
				//store values
				m_rCnt = m_rCounter;
				m_rTick = tick;
				m_rRate = 0.0;
			}
			else if ((tick - m_rTick) > 10000)
			{
				m_rRate = (double)(m_rCounter - m_rCnt) / (double)(tick - m_rTick) * 1000.0;

				//store values
				m_rCnt = m_rCounter;
				m_rTick = tick;
			}

			return m_rRate;
		}

		void NodeDWord::Read(UINT32_UT Value, UINT16 offset, bool* val)
		{
			switch(offset)
			{
			case 0:
				*val = Value.bit.b0;
				break;
			case 1:
				*val = Value.bit.b1;
				break;
			case 2:
				*val = Value.bit.b2;
				break;
			case 3:
				*val = Value.bit.b3;
				break;
			case 4:
				*val = Value.bit.b4;
				break;
			case 5:
				*val = Value.bit.b5;
				break;
			case 6:
				*val = Value.bit.b6;
				break;
			case 7:
				*val = Value.bit.b7;
				break;
			case 8:
				*val = Value.bit.b8;
				break;
			case 9:
				*val = Value.bit.b9;
				break;
			case 10:
				*val = Value.bit.b10;
				break;
			case 11:
				*val = Value.bit.b11;
				break;
			case 12:
				*val = Value.bit.b12;
				break;
			case 13:
				*val = Value.bit.b13;
				break;
			case 14:
				*val = Value.bit.b14;
				break;
			case 15:
				*val = Value.bit.b15;
				break;
			case 16:
				*val = Value.bit.b16;
				break;
			case 17:
				*val = Value.bit.b17;
				break;
			case 18:
				*val = Value.bit.b18;
				break;
			case 19:
				*val = Value.bit.b19;
				break;
			case 20:
				*val = Value.bit.b20;
				break;
			case 21:
				*val = Value.bit.b21;
				break;
			case 22:
				*val = Value.bit.b22;
				break;
			case 23:
				*val = Value.bit.b23;
				break;
			case 24:
				*val = Value.bit.b24;
				break;
			case 25:
				*val = Value.bit.b25;
				break;
			case 26:
				*val = Value.bit.b26;
				break;
			case 27:
				*val = Value.bit.b27;
				break;
			case 28:
				*val = Value.bit.b28;
				break;
			case 29:
				*val = Value.bit.b29;
				break;
			case 30:
				*val = Value.bit.b30;
				break;
			case 31:
				*val = Value.bit.b31;
				break;
			default:
				throw Exc(this->Id(), "Bit offset >31");
			}
		}

		void NodeDWord::Read(bool* val)
		{
			UINT32_UT Value;

			{
				CAutoLock al(m_rLock);
				Value.dword = m_rValue;
				m_rFlag = 0;
			}

			switch(m_Type)
			{
			case NodeType_Bit:
				Read(Value, m_Offset, val);
				break;
			default:
				throw Exc(this->Id(), "Read boolean not supported function");
			}
		}

		void NodeDWord::Write(bool* val)
		{
			UINT32_UT Value;

			{
				CAutoLock al(m_wLock);
				Value.dword = m_wValue;
			}

			switch(m_Type)
			{
			case NodeType_Bit:
				Read(Value, m_Offset, val);
				break;
			default:
				throw Exc(this->Id(), "Write boolean not supported function");
			}
		}

		void NodeDWord::Write(bool val)
		{
			UINT32_UT Value;

			CAutoLock al(m_wLock);
			
			Value.dword = m_wValue;

			switch(m_Type)
			{
			case NodeType_Bit:
				switch(m_Offset)
				{
				case 0:
					Value.bit.b0 = val;
					break;
				case 1:
					Value.bit.b1 = val;
					break;
				case 2:
					Value.bit.b2 = val;
					break;
				case 3:
					Value.bit.b3 = val;
					break;
				case 4:
					Value.bit.b4 = val;
					break;
				case 5:
					Value.bit.b5 = val;
					break;
				case 6:
					Value.bit.b6 = val;
					break;
				case 7:
					Value.bit.b7 = val;
					break;
				case 8:
					Value.bit.b8 = val;
					break;
				case 9:
					Value.bit.b9 = val;
					break;
				case 10:
					Value.bit.b10 = val;
					break;
				case 11:
					Value.bit.b11 = val;
					break;
				case 12:
					Value.bit.b12 = val;
					break;
				case 13:
					Value.bit.b13 = val;
					break;
				case 14:
					Value.bit.b14 = val;
					break;
				case 15:
					Value.bit.b15 = val;
					break;
				case 16:
					Value.bit.b16 = val;
					break;
				case 17:
					Value.bit.b17 = val;
					break;
				case 18:
					Value.bit.b18 = val;
					break;
				case 19:
					Value.bit.b19 = val;
					break;
				case 20:
					Value.bit.b20 = val;
					break;
				case 21:
					Value.bit.b21 = val;
					break;
				case 22:
					Value.bit.b22 = val;
					break;
				case 23:
					Value.bit.b23 = val;
					break;
				case 24:
					Value.bit.b24 = val;
					break;
				case 25:
					Value.bit.b25 = val;
					break;
				case 26:
					Value.bit.b26 = val;
					break;
				case 27:
					Value.bit.b27 = val;
					break;
				case 28:
					Value.bit.b28 = val;
					break;
				case 29:
					Value.bit.b29 = val;
					break;
				case 30:
					Value.bit.b30 = val;
					break;
				case 31:
					Value.bit.b31 = val;
					break;
				default:
					throw Exc(this->Id(), "Bit offset >31");
				}
				break;
			default:
				break;
			}

			m_wValue = Value.dword;
			m_wFlag++;
		}

		void NodeDWord::Read(UINT32_UT Value, UINT16 offset, PUINT8 val)
		{
			switch(offset)
			{
			case 0:
				*val = Value.byte.byte0;
				break;
			case 1:
				*val = Value.byte.byte1;
				break;
			case 2:
				*val = Value.byte.byte2;
				break;
			case 3:
				*val = Value.byte.byte3;
				break;
			default:
				throw Exc(this->Id(), "Byte offset >3");
			}
		}

		void NodeDWord::Read(UINT32_UT Value, UINT16 offset, PUINT16 val)
		{
			switch (offset)
			{
			case 0:
				*val = Value.word.word0;
				break;
			case 1:
				*val = Value.word.word1;
				break;			
			default:
				throw Exc(this->Id(), "Word offset >1");
			}
		}

		void NodeDWord::Read(PUINT8 val)
		{
			UINT32_UT Value;

			{
				CAutoLock al(m_rLock);
				Value.dword = m_rValue;
				m_rFlag = 0;
			}

			switch(m_Type)
			{
			case NodeType_Byte:
				Read(Value, m_Offset, val);
				break;
			default:
				break;
			}
		}

		void NodeDWord::Read(PUINT16 val)
		{
			UINT32_UT Value;

			{
				CAutoLock al(m_rLock);
				Value.dword = m_rValue;
				m_rFlag = 0;
			}

			switch (m_Type)
			{
			case NodeType_Word:
				Read(Value, m_Offset, val);
				break;
			default:
				break;
			}
		}

		void NodeDWord::Write(PUINT8 val)
		{
			UINT32_UT Value;

			{
				CAutoLock al(m_wLock);
				Value.dword = m_wValue;
			}

			switch(m_Type)
			{
			case NodeType_Byte:
				Read(Value, m_Offset, val);
				break;
			default:
				break;
			}
		}

		void NodeDWord::Write(PUINT16 val)
		{
			UINT32_UT Value;

			{
				CAutoLock al(m_wLock);
				Value.dword = m_wValue;
			}

			switch (m_Type)
			{
			case NodeType_Word:
				Read(Value, m_Offset, val);
				break;
			default:
				break;
			}
		}

		void NodeDWord::Write(UINT8 val)
		{
			UINT32_UT Value;

			CAutoLock al(m_wLock);
			
			Value.dword = m_wValue;

			switch(m_Type)
			{
			case NodeType_Byte:
				switch(m_Offset)
				{
				case 0:
					Value.byte.byte0 = val;
					break;
				case 1:
					Value.byte.byte1 = val;
					break;
				case 2:
					Value.byte.byte2 = val;
					break;
				case 3:
					Value.byte.byte3 = val;
					break;
				default:
					throw Exc(this->Id(), "Byte offset >3");
				}
				break;
			default:
				break;
			}

			m_wValue = Value.dword;
			m_wFlag++;
		}

		void NodeDWord::Write(UINT16 val)
		{
			UINT32_UT Value;

			CAutoLock al(m_wLock);

			Value.dword = m_wValue;

			switch (m_Type)
			{
			case NodeType_Word:
				switch (m_Offset)
				{
				case 0:
					Value.word.word0 = val;
					break;
				case 1:
					Value.word.word1 = val;
					break;
				
				default:
					throw Exc(this->Id(), "Word offset >1");
				}
				break;
			default:
				break;
			}

			m_wValue = Value.dword;
			m_wFlag++;
		}

		void NodeDWord::SetTimerDefaultVal(UINT16 val)
		{
			UINT32_UT Value;
			Value.dword = val;

			switch(m_Type)
			{
			case NodeType_Bit:
				switch(m_Offset)
				{
				case 0:
					m_Timer.SetVal(Value.bit.b0);
					break;
				case 1:
					m_Timer.SetVal(Value.bit.b1);
					break;
				case 2:
					m_Timer.SetVal(Value.bit.b2);
					break;
				case 3:
					m_Timer.SetVal(Value.bit.b3);
					break;
				case 4:
					m_Timer.SetVal(Value.bit.b4);
					break;
				case 5:
					m_Timer.SetVal(Value.bit.b5);
					break;
				case 6:
					m_Timer.SetVal(Value.bit.b6);
					break;
				case 7:
					m_Timer.SetVal(Value.bit.b7);
					break;
				case 8:
					m_Timer.SetVal(Value.bit.b8);
					break;
				case 9:
					m_Timer.SetVal(Value.bit.b9);
					break;
				case 10:
					m_Timer.SetVal(Value.bit.b10);
					break;
				case 11:
					m_Timer.SetVal(Value.bit.b11);
					break;
				case 12:
					m_Timer.SetVal(Value.bit.b12);
					break;
				case 13:
					m_Timer.SetVal(Value.bit.b13);
					break;
				case 14:
					m_Timer.SetVal(Value.bit.b14);
					break;
				case 15:
					m_Timer.SetVal(Value.bit.b15);
					break;
				case 16:
					m_Timer.SetVal(Value.bit.b16);
					break;
				case 17:
					m_Timer.SetVal(Value.bit.b17);
					break;
				case 18:
					m_Timer.SetVal(Value.bit.b18);
					break;
				case 19:
					m_Timer.SetVal(Value.bit.b19);
					break;
				case 20:
					m_Timer.SetVal(Value.bit.b20);
					break;
				case 21:
					m_Timer.SetVal(Value.bit.b21);
					break;
				case 22:
					m_Timer.SetVal(Value.bit.b22);
					break;
				case 23:
					m_Timer.SetVal(Value.bit.b23);
					break;
				case 24:
					m_Timer.SetVal(Value.bit.b24);
					break;
				case 25:
					m_Timer.SetVal(Value.bit.b25);
					break;
				case 26:
					m_Timer.SetVal(Value.bit.b26);
					break;
				case 27:
					m_Timer.SetVal(Value.bit.b27);
					break;
				case 28:
					m_Timer.SetVal(Value.bit.b28);
					break;
				case 29:
					m_Timer.SetVal(Value.bit.b29);
					break;
				case 30:
					m_Timer.SetVal(Value.bit.b30);
					break;
				case 31:
					m_Timer.SetVal(Value.bit.b31);
					break;
				}
				break;
			}
		}

		void NodeDWord::DelayTimer(UINT32 DelayON_MS, UINT32 DelayOFF_MS)
		{ 
			switch(m_Type)
			{
			case NodeType_Bit:
				m_Timer.SetDelayON(DelayON_MS); 
				m_Timer.SetDelayOFF(DelayOFF_MS); 
				break;
			default:
				throw Exc(this->Id(), "Delay Timer supported only for Bit node");
				break;
			}
		}

		void NodeDWord::Refresh(void)
		{
			if(m_rValueInit)
			{
				m_rValueInit = false;
				m_rValueLast = m_rValue;

				//init timer default value				
				SetTimerDefaultVal(m_rValue);
			}
			else
			{
				if(m_rValue==m_rValueLast)
					return;

				UINT32_UT Value;
				UINT32_UT last;
				Value.dword = m_rValue;
				last.dword = m_rValueLast;

				switch(m_Type)
				{
				case NodeType_Bit:
					switch(m_Offset)
					{
					case 0:
						if(m_Timer.Delayed(Value.bit.b0) != last.bit.b0)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b0 = Value.bit.b0;							
							OnValueChanged(last.bit.b0);
						}
						break;
					case 1:
						if(m_Timer.Delayed(Value.bit.b1) != last.bit.b1)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b1 = Value.bit.b1;							
							OnValueChanged(last.bit.b1);
						}
						break;
					case 2:
						if(m_Timer.Delayed(Value.bit.b2) != last.bit.b2)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b2 = Value.bit.b2;							
							OnValueChanged(last.bit.b2);
						}
						break;
					case 3:
						if(m_Timer.Delayed(Value.bit.b3) != last.bit.b3)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b3 = Value.bit.b3;							
							OnValueChanged(last.bit.b3);
						}
						break;
					case 4:
						if(m_Timer.Delayed(Value.bit.b4) != last.bit.b4)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b4 = Value.bit.b4;							
							OnValueChanged(last.bit.b4);
						}
						break;
					case 5:
						if(m_Timer.Delayed(Value.bit.b5) != last.bit.b5)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b5 = Value.bit.b5;							
							OnValueChanged(last.bit.b5);
						}
						break;
					case 6:
						if(m_Timer.Delayed(Value.bit.b6) != last.bit.b6)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b6 = Value.bit.b6;							
							OnValueChanged(last.bit.b6);
						}
						break;
					case 7:
						if(m_Timer.Delayed(Value.bit.b7) != last.bit.b7)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b7 = Value.bit.b7;							
							OnValueChanged(last.bit.b7);
						}
						break;
					case 8:
						if(m_Timer.Delayed(Value.bit.b8) != last.bit.b8)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b8 = Value.bit.b8;							
							OnValueChanged(last.bit.b8);
						}
						break;
					case 9:
						if(m_Timer.Delayed(Value.bit.b9) != last.bit.b9)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b9 = Value.bit.b9;
							OnValueChanged(last.bit.b9);
						}
						break;
					case 10:
						if(m_Timer.Delayed(Value.bit.b10) != last.bit.b10)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b10 = Value.bit.b10;							
							OnValueChanged(last.bit.b10);
						}
						break;
					case 11:
						if(m_Timer.Delayed(Value.bit.b11) != last.bit.b11)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b11 = Value.bit.b11;							
							OnValueChanged(last.bit.b11);
						}
						break;
					case 12:
						if(m_Timer.Delayed(Value.bit.b12) != last.bit.b12)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b12 = Value.bit.b12;							
							OnValueChanged(last.bit.b12);
						}
						break;
					case 13:
						if(m_Timer.Delayed(Value.bit.b13) != last.bit.b13)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b13 = Value.bit.b13;							
							OnValueChanged(last.bit.b13);
						}
						break;
					case 14:
						if(m_Timer.Delayed(Value.bit.b14) != last.bit.b14)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b14 = Value.bit.b14;							
							OnValueChanged(last.bit.b14);
						}
						break;
					case 15:
						if(m_Timer.Delayed(Value.bit.b15) != last.bit.b15)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b15 = Value.bit.b15;							
							OnValueChanged(last.bit.b15);
						}
						break;
					case 16:
						if (m_Timer.Delayed(Value.bit.b16) != last.bit.b16)
						{
							{
								CAutoLock al(m_CounterLock);
								if (m_Changes == MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.bit.b16 = Value.bit.b16;							
							OnValueChanged(last.bit.b16);
						}
						break;
					default:
						throw Exc(this->Id(), "Bit offset >31");
					}
					break;
				case NodeType_Byte:
					switch(m_Offset)
					{
					case 0:
						if(Value.byte.byte0!=last.byte.byte0)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.byte.byte0 = Value.byte.byte0;							
							OnValueChanged(last.byte.byte0);
						}
						break;
					case 1:
						if(Value.byte.byte1!=last.byte.byte1)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.byte.byte1 = Value.byte.byte1;							
							OnValueChanged(last.byte.byte1);
						}	
						break;
					case 2:
						if (Value.byte.byte2 != last.byte.byte2)
						{
							{
								CAutoLock al(m_CounterLock);
								if (m_Changes == MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.byte.byte2 = Value.byte.byte2;							
							OnValueChanged(last.byte.byte2);
						}
						break;
					case 3:
						if (Value.byte.byte3 != last.byte.byte3)
						{
							{
								CAutoLock al(m_CounterLock);
								if (m_Changes == MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.byte.byte3 = Value.byte.byte3;							
							OnValueChanged(last.byte.byte3);
						}
						break;
					default:
						throw Exc(this->Id(), "Byte offset >3");
					}
					break;
				case NodeType_Word:
					switch(m_Offset)
					{
					case 0:
						if(Value.word.word0!=last.word.word0)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.word.word0 = Value.word.word0;							
							OnValueChanged(last.word.word0);
						}
						break;
					case 1:
						if (Value.word.word1 != last.word.word1)
						{
							{
								CAutoLock al(m_CounterLock);
								if (m_Changes == MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.word.word1 = Value.word.word1;							
							OnValueChanged(last.word.word1);
						}
						break;
					default:
						throw Exc(this->Id(), "Word offset >1");
					}
					break;
				case NodeType_DWord:
					switch (m_Offset)
					{
					case 0:
						if (Value.dword != last.dword)
						{
							{
								CAutoLock al(m_CounterLock);
								if (m_Changes == MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.dword = Value.dword;
							OnValueChanged(last.dword);							
						}
						break;
					default:
						throw Exc(this->Id(), "DWord offset >0");
					}
				case NodeType_Real32:
					switch (m_Offset)
					{
					case 0:
						if (Value.real32 != last.real32)
						{
							{
								CAutoLock al(m_CounterLock);
								if (m_Changes == MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.real32 = Value.real32;							
							OnValueChanged(last.dword);
						}
						break;
					default:
						throw Exc(this->Id(), "Real32 offset >0");
					}
				}
				m_rValueLast = last.dword;
			}
			

			//refresh subnodes
			map<string, INode::INodePtr>::iterator it = m_Nodes.begin();
			while(it!=m_Nodes.end())
			{
				it->second->Refresh();
				it++;
			}
		}
	}
}
