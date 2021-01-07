#include "stdafx.h"
#include "NodeInterface.h"

namespace Utilities
{
	namespace Node
	{
		NodeInterface::~NodeInterface()
		{
			m_Nodes.clear();
		}

		bool NodeInterface::NodeExist(const char* Id)
		{
			string id(Id);
			std::map<string, NodeInterface::NodeInterfacePtr>::iterator it = m_Nodes.find(id);
			return (it != m_Nodes.end());
		}

		NodeInterface* NodeInterface::GetNodeInterface(const char* Id)
		{
			string id(Id);
			map<string, NodeInterface::NodeInterfacePtr>::iterator it = m_Nodes.find(id);
			if (it != m_Nodes.end())
			{
				return it->second.get();
			}
			else
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "Unknown node id '%s'", Id);
				throw Exc(this->Id(), msg);
			}
		}

		void NodeInterface::RemoveNode(const char* Id)
		{
			string id(Id);
			map<string, NodeInterface::NodeInterfacePtr>::iterator it = m_Nodes.find(id);
			if (it != m_Nodes.end())
			{
				m_Nodes.erase(it);
			}
		}

		vector<NodeInterface*> NodeInterface::GetNodes()
		{
			vector<NodeInterface*> nodelist;
			map<string, NodeInterface::NodeInterfacePtr>::iterator it = m_Nodes.begin();
			while (it != m_Nodes.end())
			{
				nodelist.insert(nodelist.end(), it->second.get());
				it++;

			}
			return nodelist;
		}

		list<NodeInterface*> NodeInterface::SearchNodeInterface(const char* Id)
		{
			string path(Id);
			string id(Id);
			string subid("");
			list<NodeInterface*> nodelist;

			//is path???
			size_t pos = path.find('.');
			if (pos != -1)
			{
				id = path.substr(0, pos);
				subid = path.substr(pos + 1, path.length());
			}

			map<string, NodeInterface::NodeInterfacePtr>::iterator it = m_Nodes.find(id);
			if (it != m_Nodes.end())
			{
				if (subid.length() > 0)
				{
					list<NodeInterface*> sublist = it->second.get()->SearchNodeInterface(subid.c_str());
					nodelist.insert(nodelist.end(), sublist.begin(), sublist.end());
				}
				else
					nodelist.push_back(it->second.get());
			}
			else if (subid.length() == 0)
			{
				it = m_Nodes.begin();
				while (it != m_Nodes.end())
				{
					if (it->second->NodesCount() > 0)
					{
						list<NodeInterface*> sublist = it->second->SearchNodeInterface(Id);
						nodelist.insert(nodelist.end(), sublist.begin(), sublist.end());
					}
					it++;
				}
			}

			return nodelist;
		}

		void NodeInterface::SetException(Exception e)
		{
			CAutoLock al(m_LockNodeInterface);
			m_Exception = Exception::ExceptionPtr(new Exception(e));
		}

		void NodeInterface::SetExceptionForAllNodes(Exception e)
		{
			CAutoLock al(m_LockNodeInterface);
			m_Exception = Exception::ExceptionPtr(new Exception(e));

			map<string, NodeInterface::NodeInterfacePtr>::iterator it = m_Nodes.begin();
			while (it != m_Nodes.end())
			{
				it->second->SetExceptionForAllNodes(e);
				it++;
			}
		}

		void NodeInterface::ClearException()
		{
			CAutoLock al(m_LockNodeInterface);
			m_Exception.reset();
		}

		void NodeInterface::ClearExceptionForAllNodes()
		{
			CAutoLock al(m_LockNodeInterface);
			m_Exception.reset();

			map<string, NodeInterface::NodeInterfacePtr>::iterator it = m_Nodes.begin();
			while (it != m_Nodes.end())
			{
				it->second->ClearExceptionForAllNodes();
				it++;
			}
		}

		Exception* NodeInterface::GetLastException()
		{
			CAutoLock al(m_LockNodeInterface);
			return m_Exception.get();
		}
	}
}