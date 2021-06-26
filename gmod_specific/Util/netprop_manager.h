#pragma once
#include "../../include/stdafx.h"
#include "../../engine/Core/Basic/Basic.h"
namespace SDK {
	namespace Util {
		class NetPropManager : public Basic::Singleton< NetPropManager >
		{
		public:
			bool Create(IBaseClientDLL* base_client);
			void Destroy();

			int Get(const std::string& array_name, const std::string& prop_name, RecvProp** prop_out = nullptr);

			void Dump(std::string file_name);
			auto dump_recursive(const char* base_class, RecvTable* table, const std::uint16_t offset, FILE * file) -> void;
		private:
			void Dump_itertor(RecvTable *table ,FILE * file);
			int Get(RecvTable* recv_entry, const std::string& prop_name, RecvProp** prop_out = nullptr);
			RecvTable* GetArrayEntry(const std::string& array_name);

		private:
			std::vector< RecvTable* > m_recv_array = {};
		};
	}
}