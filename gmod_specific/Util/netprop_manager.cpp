#include "../SDK.h"
#include "netprop_manager.h"
namespace SDK {
	namespace Util {
		bool NetPropManager::Create(IBaseClientDLL * base_client)
		{
			m_recv_array.clear();

			if (!base_client)
				return false;

			for (auto entry = base_client->GetAllClasses(); entry; entry = entry->m_pNext)
			{
				auto recv_entry = entry->m_pRecvTable;

				if (recv_entry)
					m_recv_array.emplace_back(recv_entry);
			}

			return !m_recv_array.empty();
		}

		void NetPropManager::Destroy()
		{
			m_recv_array.clear();
		}

		int NetPropManager::Get(const std::string & array_name, const std::string & prop_name, RecvProp ** prop_out)
		{
			auto array_entry = GetArrayEntry(array_name);
			return Get(array_entry, prop_name, prop_out);
		}

		void NetPropManager::Dump(std::string file_name)
		{
			FILE * Out = nullptr;
			if (fopen_s(&Out, file_name.c_str(), "w")!= 0 || !Out) {
				return;
			}

			if (m_recv_array.size()) {
				for (auto& it : m_recv_array) {

					std::string arr_name (it->m_pNetTableName);
					arr_name += '\n';
					fputs(arr_name.c_str(), Out);
					dump_recursive(it->m_pNetTableName, it, 0u, Out);
					//Dump_itertor(it,Out);
				}
			}
			fclose(Out);
		}

		std::string type2str(SendPropType t)
		{
			switch (t)
			{
			case DPT_Int:
				return "Int";
			case DPT_Float:
				return "Float";
			case DPT_Vector:
				return "Vector";
			case DPT_String:
				return "String";
			case DPT_Array:
				return "Array";
			case DPT_DataTable:
				return "DataTable";
			case DPT_NUMSendPropTypes:
				return "NSPT";
			default:
				return "Unknown Type";
			}
		}



		auto NetPropManager::dump_recursive(const char* base_class, RecvTable* table, const std::uint16_t offset, FILE * file) -> void
		{
			for (auto i = 0; i < table->m_nProps; ++i)
			{
				const auto prop_ptr = &table->m_pProps[i];

				//Skip trash array items
				if (!prop_ptr || isdigit(prop_ptr->m_pVarName[0]))
					continue;

				//We dont care about the base class, we already know that
				if (strcmp(prop_ptr->m_pVarName,"baseclass") == 0)
					continue;
				

				if (prop_ptr->m_pDataTable != nullptr )
				{
					dump_recursive(base_class, prop_ptr->m_pDataTable, std::uint16_t(offset + prop_ptr->m_Offset), file);
				}
				char hash_name[256];

				strcpy_s(hash_name, base_class);
				strcat_s(hash_name, "->");
				strcat_s(hash_name, prop_ptr->m_pVarName);

				const auto total_offset = std::uint16_t(offset + prop_ptr->m_Offset);

				char s_fp[260];
				sprintf_s(s_fp, "\t%s \t\t0x%04X \t %s \n", type2str(prop_ptr->m_RecvType).c_str(), total_offset, prop_ptr->m_pVarName);
				fputs(s_fp, file);
			}
		}

		void NetPropManager::Dump_itertor(RecvTable *table,FILE * file)
		{
			if (!table)
				return;
			for (int i = 0; i < table->m_nProps; i++)
			{
				auto& recvProp = table->m_pProps[i];

				//if (recvProp.m_RecvType == SendPropType::DPT_Array)
				//	continue;
				if (strcmp(recvProp.m_pVarName, "baseclass") == 0)
					continue;
				char buff[255];

				sprintf_s(buff, "\t%s %s 0x%p \n", type2str(recvProp.m_RecvType).c_str(), recvProp.m_pVarName, recvProp.m_Offset);

				//sprintf_s(buff, "name %s ;offset 0x%p ;type %s \n", recvProp->m_pVarName, recvProp->m_Offset , type2str( recvProp->m_RecvType).c_str());
				fputs(buff, file);
				if (recvProp.m_RecvType == SendPropType::DPT_DataTable || recvProp.m_RecvType == SendPropType::DPT_Array)
				{
					
					Dump_itertor(recvProp.m_pDataTable, file);
				}
				//
				if (recvProp.m_RecvType == SendPropType::DPT_Array)
				{
				}
			}
		}

		int NetPropManager::Get(RecvTable * recv_entry, const std::string & prop_name, RecvProp ** prop_out)
		{
			if (!recv_entry)
				return 0;

			auto extra = 0;

			for (auto index = 0; index < recv_entry->m_nProps; index++)
			{
				auto prop = &recv_entry->m_pProps[index];
				auto child = prop->m_pDataTable;

				if (child && child->m_nProps)
				{
					auto add = Get(child, prop_name, prop_out);

					if (add)
						extra += (prop->m_Offset + add);
				}

				if (!prop_name.compare(prop->m_pVarName))
				{
					if (prop_out)
						*prop_out = prop;

					return (prop->m_Offset + extra);
				}
			}

			return extra;
		}

		RecvTable * NetPropManager::GetArrayEntry(const std::string & array_name)
		{
			for (auto& entry : m_recv_array)
			{
				if (!array_name.compare(entry->m_pNetTableName))
					return entry;
			}

			return nullptr;
		}


	}
}

