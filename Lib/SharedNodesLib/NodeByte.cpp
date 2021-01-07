#include "stdafx.h"
#include "NodeByte.h"

namespace Utilities
{
	namespace Node
	{
		INode* NodeByte::CreateNode(const char* Id, NodeType Type, UINT16 Offset)
		{
			return CreateNode(Id, Type, Offset, m_Access);
		}

		INode* NodeByte::CreateNode(const char* Id, NodeType type, UINT16 Offset, NodeAccess Access)
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
				if(Offset>15)
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg), "Bit offset >15 for node '%s'", Id);
					throw Exc(this->Id(), msg);					
				}
				break;
			case NodeType_Byte:
				if(Offset>1)
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg), "Byte offset >1 for node '%s'", Id);
					throw Exc(this->Id(), msg);
				}
				break;
			case NodeType_Word:
				if(Offset>0)
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg), "Word offset >0 for node '%s'", Id);
					throw Exc(this->Id(), msg);
				}
				break;
			}

			NodeByte::NodeBytePtr node(new NodeByte(id,
													type, 
													m_Access,
													Offset, 
													m_rValue, 
													m_wValue, 
													m_rFlag,
													m_wFlag, 
													m_ReadLock, 
													m_WriteLock,
													m_ReadTime,
													m_WriteTime));
			m_Nodes[id] = node;

			return dynamic_cast<INode*>(node.get());
		}

		NodeByte* NodeByte::GetNode(const char* Id)
		{
			return dynamic_cast<NodeByte*>(GetNodeInterface(Id));
		}

		void NodeByte::Read(UINT8_UT Value, UINT16 offset, bool* val)
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
			default:
				throw Exc(this->Id(), "Bit offset >15");
			}
		}

		void NodeByte::Read(bool* val)
		{
			UINT8_UT Value;

			{
				CAutoLock al(m_ReadLock);
				Value.byte = m_rValue;
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

		void NodeByte::Write(bool* val)
		{
			UINT8_UT Value;

			{
				CAutoLock al(m_WriteLock);
				Value.byte = m_wValue;
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

		void NodeByte::Write(bool val)
		{
			UINT8_UT Value;

			CAutoLock al(m_WriteLock);
			
			Value.byte = m_wValue;

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
				default:
					throw Exc(this->Id(), "Bit offset >15");
				}
				break;
			default:
				break;
			}

			m_wValue = Value.byte;
			m_wFlag++;
		}

		void NodeByte::SetTimerDefaultVal(UINT8 val)
		{
			UINT8_UT Value;
			Value.byte = val;

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
				}
				break;
			}
		}

		void NodeByte::DelayTimer(UINT32 DelayON_MS, UINT32 DelayOFF_MS)
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

		void NodeByte::Refresh(void)
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

				UINT16_UT Value;
				UINT16_UT last;
				Value.word = m_rValue;
				last.word = m_rValueLast;

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
					default:
						throw Exc(this->Id(), "Bit offset >15");
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
					default:
						throw Exc(this->Id(), "Byte offset >1");
					}
					break;
				case NodeType_Word:
					switch(m_Offset)
					{
					case 0:
						if(Value.word!=last.word)
						{
							{
								CAutoLock al(m_CounterLock);
								if(m_Changes==MAXUINT32)
									m_Changes = 0;
								else
									m_Changes++;
							}
							last.word = Value.word;							
							OnValueChanged(last.word);
						}
						break;
					default:
						throw Exc(this->Id(), "Word offset >0");
					}
					break;
				}
				m_rValueLast = last.word;
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
