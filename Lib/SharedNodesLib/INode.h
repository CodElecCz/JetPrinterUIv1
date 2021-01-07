#pragma once

#include "stdafx.h"
#include "Def.h"

using namespace std;
using namespace Utilities;

namespace Utilities
{
	namespace Node
	{
		enum NodeType
		{
			NodeType_Bit = 0,		//NodeBit.h		NodeByte.h		NodeWord.h		NodeDWord.h		NodeUInt64.h
			NodeType_Byte,			//				NodeByte.h		NodeWord.h		NodeDWord.h		NodeUInt64.h
			NodeType_Word,			//								NodeWord.h		NodeDWord.h		NodeUInt64.h
			NodeType_Int16,			//								NodeWord.h		NodeDWord.h		NodeUInt64.h	
			NodeType_DWord,			//												NodeDWord.h		NodeUInt64.h
			NodeType_Int32,			//												NodeDWord.h		NodeUInt64.h
			NodeType_Real32,		//												NodeDWord.h		NodeUInt64.h
			NodeType_UInt64,		//																NodeUInt64.h
			NodeType_Int64,			//																NodeUInt64.h
			NodeType_Real64,		//																NodeUInt64.h
			NodeType_BitArray,		//NodeBitArray.h
			NodeType_ByteArray,		//NodeByteArray.h
			NodeType_WordArray,		//NodeWordArray.h
			NodeType_DWordArray,	//NodeDWordArray.h
			NodeType_UInt64Array,	//NodeUINT64Array.h
			NodeType_Task,
			/* size */
			NodeType_Size
		};

		const char* const  NodeType_Str[NodeType_Size] =
		{
			"Bit",
			"Byte",
			"Word",
			"Int16",
			"DWord",
			"Int32",
			"Real32",
			"UInt64",
			"Int64",
			"Real64",
			"Bit Array",
			"Byte Array",
			"Word Array",
			"DWord Array",
			"UInt64 Array",
			"Task"
		};

		enum NodeAccess
		{
			NodeAccess_Read = 0x1,
			NodeAccess_Write = 0x2
		};

		class LIB_API INode
		{
		public:
			INode(string Id, NodeType Type) :				
				OnValueChanged(this),
				OnErrorChanged(this),
				m_Id(Id), 				
				m_Type(Type), 
				m_Access((NodeAccess)(NodeAccess_Read | NodeAccess_Write)), 
				m_Description(""), 
				m_Changes(0)			
			{}
			INode(string Id, NodeType Type, NodeAccess Access) :				
				OnValueChanged(this),
				OnErrorChanged(this),
				m_Id(Id), 
				m_Type(Type), 
				m_Access(Access), 
				m_Description(""), 
				m_Changes(0)
			{}
			INode(string Id, NodeType Type, NodeAccess Access, UINT32 Offset) :				
				OnValueChanged(this),
				OnErrorChanged(this),
				m_Id(Id), 
				m_Type(Type), 
				m_Access(Access), 
				m_Offset(Offset), 
				m_Description(""), 
				m_Changes(0)
			{}
			virtual ~INode();

		public:
			//properties
			NodeType Type() const { return m_Type; }
			NodeAccess Access() const { return m_Access; }
			const char* TypeStr() const { return NodeType_Str[m_Type]; }
			const char* Id() const { return m_Id.c_str(); }
			UINT32 Offset() const { return m_Offset;  }
			void Description(const char* d) { m_Description = string(d); }
			const char* Description() const { return m_Description.c_str(); }

			//nodes
			virtual INode* CreateNode(const char*, NodeType, UINT16 /* offset */) { throw Exc(this->Id(), "Not supported function: CreateNode()"); }
			virtual INode* CreateNode(const char*, NodeType, UINT16 /* offset */, NodeAccess) { throw Exc(this->Id(), "Not supported function: CreateNode()"); }
			virtual list<INode*> SearchNodeInterface(const char* Id);
			virtual void Refresh(void) = 0;
			
			//nodes defined
			bool NodeExist(const char* id);
			INode* GetNodeInterface(const char* Id);
			void RemoveNode(const char*);
			vector<INode*> GetNodes();
			UINT32 NodesCount() const { return (UINT32)m_Nodes.size(); }

			//exceptions
			void SetException(INodeException e);
			void SetExceptionForAllNodes(INodeException e);
			void ClearException();
			void ClearExceptionForAllNodes();
			INodeException* GetLastException();

			//timestamp
			virtual SYSTEMTIME ReadTimeStamp() = 0;
			virtual SYSTEMTIME WriteTimeStamp() = 0;

			//message counter
			virtual UINT64 ReadCounter() { throw Exc(this->Id(), "Not supported function: ReadCounter()"); }
			virtual UINT64 WriteCounter() { throw Exc(this->Id(), "Not supported function: WriteCounter()"); }
			virtual double GetNodeReadRate() { throw Exc(this->Id(), "Not supported function: GetNodeReadRate()"); }

			//read BIT
			virtual void Read(bool*) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read BYTE
			virtual void Read(PUINT8) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read WORD
			virtual void Read(PUINT16) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read INT16
			virtual void Read(PINT16) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read DWORD
			virtual void Read(PUINT32) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read UINT64
			virtual void Read(PUINT64) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read REAL32
			virtual void Read(float*) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read REAL64
			virtual void Read(double*) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read ARRAY bit
			virtual void Read(bool*, PUINT32) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read ARRAY U8
			virtual void Read(PUINT8, PUINT32) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read ARRAY U16
			virtual void Read(PUINT16, PUINT32) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read ARRAY U32
			virtual void Read(PUINT32, PUINT32) { throw Exc(this->Id(), "Not supported function: Read()"); }
			//read ARRAY U64
			virtual void Read(PUINT64, PUINT32) { throw Exc(this->Id(), "Not supported function: Read()"); }

			//write BIT
			virtual void Write(bool) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write BYTE
			virtual void Write(UINT8) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write WORD
			virtual void Write(UINT16) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write INT16
			virtual void Write(INT16) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//read DWORD
			virtual void Write(UINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write UINT64
			virtual void Write(UINT64) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write REAL32
			virtual void Write(float) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write REAL64
			virtual void Write(double) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write ARRAY bit
			virtual void Write(bool*, UINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write ARRAY U8
			virtual void Write(PUINT8, UINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write ARRAY U16
			virtual void Write(PUINT16, UINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write ARRAY U32
			virtual void Write(PUINT32, UINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write ARRAY U64
			virtual void Write(PUINT64, UINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }

			//read written value
			//read BIT
			virtual void Write(bool*) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//read BYTE
			virtual void Write(PUINT8) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//read WORD
			virtual void Write(PUINT16) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//read INT16
			virtual void Write(PINT16) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//read DWORD
			virtual void Write(PUINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//read UINT64
			virtual void Write(PUINT64) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//read REAL32
			virtual void Write(float*) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//read REAL64
			virtual void Write(double*) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write ARRAY bit
			virtual void Write(bool*, PUINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write ARRAY U8
			virtual void Write(PUINT8, PUINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write ARRAY U16
			virtual void Write(PUINT16, PUINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write ARRAY U32
			virtual void Write(PUINT32, PUINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }
			//write ARRAY U64
			virtual void Write(PUINT64, PUINT32) { throw Exc(this->Id(), "Not supported function: Write()"); }

			//new data available after last reading
			virtual bool ReadDataAvailable() { throw Exc(this->Id(), "Not supported function: ReadDataAvailable()"); }
			//data was written
			virtual bool WriteDataAcknowledge() { throw Exc(this->Id(), "Not supported function: WriteDataAcknowledge()"); }

			//changes counter
			virtual UINT32 GetChangesCnt() { CAutoLock al(m_CounterLock); return m_Changes; }
			virtual void ResetChangesCnt() { CAutoLock al(m_CounterLock); m_Changes = 0; }
			
			//set delay timer for BIT node
			virtual void DelayTimer(UINT32, UINT32) { throw Exc(this->Id(), "Not supported function: DelayTimer()"); }

		public:
			typedef shared_ptr<INode> INodePtr;

#pragma warning(push)
#pragma warning(disable:4251)
			event<INode, UINT64> OnValueChanged;			
			event<INode> OnErrorChanged;
#pragma warning(pop)

		protected:
			NodeType m_Type;
			NodeAccess m_Access;
			UINT32 m_Changes;
			UINT32 m_Offset; //real data offset from area

#pragma warning(push)
#pragma warning(disable:4251)
			map<string, INode::INodePtr> m_Nodes;
			string m_Id;
			string m_Description;
			CLock m_CounterLock;
#pragma warning(pop)

		private:
#pragma warning(push)
#pragma warning(disable:4251)
			INodeException::INodeExceptionPtr m_Exception;
			CLock m_LockNodeInterface;
#pragma warning(pop)
		};
	}
}
