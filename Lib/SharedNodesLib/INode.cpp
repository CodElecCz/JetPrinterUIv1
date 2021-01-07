#include "stdafx.h"
#include "INode.h"

namespace Utilities
{
	namespace Node
	{
		INode::~INode()
		{
			m_Nodes.clear();
		}

		bool INode::NodeExist(const char* Id)
		{
			string id(Id);
			std::map<string, INode::INodePtr>::iterator it = m_Nodes.find(id);
			return (it != m_Nodes.end());
		}

		INode* INode::GetNodeInterface(const char* Id)
		{
			string id(Id);
			map<string, INode::INodePtr>::iterator it = m_Nodes.find(id);
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

		void INode::RemoveNode(const char* Id)
		{
			string id(Id);
			map<string, INode::INodePtr>::iterator it = m_Nodes.find(id);
			if (it != m_Nodes.end())
			{
				m_Nodes.erase(it);
			}
		}

		vector<INode*> INode::GetNodes()
		{
			vector<INode*> nodelist;
			map<string, INode::INodePtr>::iterator it = m_Nodes.begin();
			while (it != m_Nodes.end())
			{
				nodelist.insert(nodelist.end(), it->second.get());
				it++;

			}
			return nodelist;
		}

		list<INode*> INode::SearchNodeInterface(const char* Id)
		{
			string path(Id);
			string id(Id);
			string subid("");
			list<INode*> nodelist;

			//is path???
			size_t pos = path.find('.');
			if (pos != -1)
			{
				id = path.substr(0, pos);
				subid = path.substr(pos + 1, path.length());
			}

			map<string, INode::INodePtr>::iterator it = m_Nodes.find(id);
			if (it != m_Nodes.end())
			{
				if (subid.length() > 0)
				{
					list<INode*> sublist = it->second.get()->SearchNodeInterface(subid.c_str());
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
						list<INode*> sublist = it->second->SearchNodeInterface(Id);
						nodelist.insert(nodelist.end(), sublist.begin(), sublist.end());
					}
					it++;
				}
			}

			return nodelist;
		}

		void INode::SetException(INodeException e)
		{
			CAutoLock al(m_LockNodeInterface);
			m_Exception = INodeException::INodeExceptionPtr(new INodeException(e));
			
			OnErrorChanged();
		}

		void INode::SetExceptionForAllNodes(INodeException e)
		{
			CAutoLock al(m_LockNodeInterface);
			m_Exception = INodeException::INodeExceptionPtr(new INodeException(e));

			map<string, INode::INodePtr>::iterator it = m_Nodes.begin();
			while (it != m_Nodes.end())
			{
				it->second->SetExceptionForAllNodes(e);
				it++;
			}
		}

		void INode::ClearException()
		{
			CAutoLock al(m_LockNodeInterface);
			m_Exception.reset();
		}

		void INode::ClearExceptionForAllNodes()
		{
			CAutoLock al(m_LockNodeInterface);
			m_Exception.reset();

			map<string, INode::INodePtr>::iterator it = m_Nodes.begin();
			while (it != m_Nodes.end())
			{
				it->second->ClearExceptionForAllNodes();
				it++;
			}
		}

		INodeException* INode::GetLastException()
		{
			CAutoLock al(m_LockNodeInterface);
			return m_Exception.get();
		}
	}
}