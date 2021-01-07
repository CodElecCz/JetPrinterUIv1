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
			NodeType_Bit = 0,		//NodeBit.h		NodeByte.h		NodeWord.h		NodeDWord.h
			NodeType_Byte,			//				NodeByte.h		NodeWord.h		NodeDWord.h
			NodeType_Word,			//								NodeWord.h		NodeDWord.h
			NodeType_DWord,			//												NodeDWord.h
			NodeType_Real32,		//												NodeDWord.h
			NodeType_UInt64,		//												NodeUInt64.h
			NodeType_Real64,		//												NodeUInt64.h
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
			"DWord",
			"Real32",
			"UInt64",
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

		class LIB_API NodeInterface
		{
		public:
			NodeInterface(string Id, NodeType Type)
				: m_Id(Id), m_Type(Type), m_Access((NodeAccess)(NodeAccess_Read | NodeAccess_Write)), m_Description(""), m_Counter(0)
			{}
			NodeInterface(string Id, NodeType Type, NodeAccess Access)
				: m_Id(Id), m_Type(Type), m_Access(Access), m_Description(""), m_Counter(0)
			{}
			NodeInterface(string Id, NodeType Type, NodeAccess Access, UINT32 Offset)
				: m_Id(Id), m_Type(Type), m_Access(Access), m_Offset(Offset), m_Description(""), m_Counter(0)
			{}
			virtual ~NodeInterface();

		public:
			NodeType Type() const { return m_Type; }
			NodeAccess Access() const { return m_Access; }
			const char* TypeStr() const { return NodeType_Str[m_Type]; }
			const char* Id() const { return m_Id.c_str(); }
			UINT32 Offset() const { return m_Offset;  }

			virtual NodeInterface* AddNode(const char*, NodeType, UINT16 /* offset */) { throw Exc(this->Id(), "Not supported function"); }
			virtual NodeInterface* AddNode(const char*, NodeType, UINT16 /* offset */, NodeAccess) { throw Exc(this->Id(), "Not supported function"); }
			void RemoveNode(const char*);

			NodeInterface* GetNodeInterface(const char* Id);
			virtual list<NodeInterface*> SearchNodeInterface(const char* Id);
			vector<NodeInterface*> GetNodes();
			UINT32 NodesCount() const { return (UINT32)m_Nodes.size(); }
			bool NodeExist(const char* id);

			//set node exception
			void SetException(Exception e);
			//set node exception for this node and all subnodes
			void SetExceptionForAllNodes(Exception e);
			void ClearException();
			void ClearExceptionForAllNodes();
			Exception* GetLastException();

			//node description
			void Description(const char* d) { m_Description = string(d); }
			//node description
			const char* Description() const { return m_Description.c_str(); }

			virtual void Refresh(void) = 0;
			virtual SYSTEMTIME ReadTimeStamp() = 0;
			virtual SYSTEMTIME WriteTimeStamp() = 0;

			//read BIT
			virtual void Read(bool*) { throw Exc(this->Id(), "Not supported function"); }
			//read BYTE
			virtual void Read(PUINT8) { throw Exc(this->Id(), "Not supported function"); }
			//read WORD
			virtual void Read(PUINT16) { throw Exc(this->Id(), "Not supported function"); }
			//read DWORD
			virtual void Read(PUINT32) { throw Exc(this->Id(), "Not supported function"); }
			//read UINT64
			virtual void Read(PUINT64) { throw Exc(this->Id(), "Not supported function"); }
			//read REAL32
			virtual void Read(float*) { throw Exc(this->Id(), "Not supported function"); }
			//read REAL64
			virtual void Read(double*) { throw Exc(this->Id(), "Not supported function"); }
			//read ARRAY bit
			virtual void Read(bool*, PUINT32) { throw Exc(this->Id(), "Not supported function"); }
			//read ARRAY U8
			virtual void Read(PUINT8, PUINT32) { throw Exc(this->Id(), "Not supported function"); }
			//read ARRAY U16
			virtual void Read(PUINT16, PUINT32) { throw Exc(this->Id(), "Not supported function"); }
			//read ARRAY U32
			virtual void Read(PUINT32, PUINT32) { throw Exc(this->Id(), "Not supported function"); }
			//read ARRAY U64
			virtual void Read(PUINT64, PUINT32) { throw Exc(this->Id(), "Not supported function"); }

			//write BIT
			virtual void Write(bool) { throw Exc(this->Id(), "Not supported function"); }
			//write BYTE
			virtual void Write(UINT8) { throw Exc(this->Id(), "Not supported function"); }
			//write WORD
			virtual void Write(UINT16) { throw Exc(this->Id(), "Not supported function"); }
			//read DWORD
			virtual void Write(UINT32) { throw Exc(this->Id(), "Not supported function"); }
			//write UINT64
			virtual void Write(UINT64) { throw Exc(this->Id(), "Not supported function"); }
			//write REAL32
			virtual void Write(float) { throw Exc(this->Id(), "Not supported function"); }
			//write REAL64
			virtual void Write(double) { throw Exc(this->Id(), "Not supported function"); }
			//write ARRAY bit
			virtual void Write(bool*, UINT32) { throw Exc(this->Id(), "Not supported function"); }
			//write ARRAY U8
			virtual void Write(PUINT8, UINT32) { throw Exc(this->Id(), "Not supported function"); }
			//write ARRAY U16
			virtual void Write(PUINT16, UINT32) { throw Exc(this->Id(), "Not supported function"); }
			//write ARRAY U32
			virtual void Write(PUINT32, UINT32) { throw Exc(this->Id(), "Not supported function"); }
			//write ARRAY U64
			virtual void Write(PUINT64, UINT32) { throw Exc(this->Id(), "Not supported function"); }

			//read written value
			virtual void Write(bool*) { throw Exc(this->Id(), "Not supported function"); }
			//read written value
			virtual void Write(PUINT8) { throw Exc(this->Id(), "Not supported function"); }
			//read written value
			virtual void Write(PUINT16) { throw Exc(this->Id(), "Not supported function"); }
			//read DWORD
			virtual void Write(PUINT32) { throw Exc(this->Id(), "Not supported function"); }
			//read UINT64
			virtual void Write(PUINT64) { throw Exc(this->Id(), "Not supported function"); }
			//read REAL32
			virtual void Write(float*) { throw Exc(this->Id(), "Not supported function"); }
			//read REAL64
			virtual void Write(double*) { throw Exc(this->Id(), "Not supported function"); }
			//write ARRAY bit
			virtual void Write(bool*, PUINT32) { throw Exc(this->Id(), "Not supported function"); }
			//write ARRAY U8
			virtual void Write(PUINT8, PUINT32) { throw Exc(this->Id(), "Not supported function"); }
			//write ARRAY U16
			virtual void Write(PUINT16, PUINT32) { throw Exc(this->Id(), "Not supported function"); }
			//write ARRAY U32
			virtual void Write(PUINT32, PUINT32) { throw Exc(this->Id(), "Not supported function"); }
			//write ARRAY U64
			virtual void Write(PUINT64, PUINT32) { throw Exc(this->Id(), "Not supported function"); }

			//new data available after last reading
			virtual bool ReadDataAvailable() { throw Exc(this->Id(), "Not supported function"); }
			//data was written
			virtual bool WriteDataAcknowledge() { throw Exc(this->Id(), "Not supported function"); }

			//changes counter get
			virtual UINT32 GetCounter() { CAutoLock al(m_CounterLock); return m_Counter; }
			//changes counter reset
			virtual void ResetCounter() { CAutoLock al(m_CounterLock); m_Counter = 0; }
			//set delay timer for BIT node
			virtual void DelayTimer(UINT32, UINT32) { throw Exc(this->Id(), "Not supported function"); }

		public:
			typedef shared_ptr<NodeInterface> NodeInterfacePtr;

#pragma warning(push)
#pragma warning(disable:4251)
			event<NodeInterface*, UINT64> OnNodeValueChanged;
			event<const char*, UINT64> OnValueChanged;
#pragma warning(pop)

		protected:
			NodeType m_Type;
			NodeAccess m_Access;
			UINT32 m_Counter;	
			UINT32 m_Offset; //real data offset from area

#pragma warning(push)
#pragma warning(disable:4251)
			map<string, NodeInterface::NodeInterfacePtr> m_Nodes;
			string m_Id;
			string m_Description;
			CLock m_CounterLock;
#pragma warning(pop)

		private:
#pragma warning(push)
#pragma warning(disable:4251)
			Exception::ExceptionPtr m_Exception;
			CLock m_LockNodeInterface;
#pragma warning(pop)
		};
	}
}
