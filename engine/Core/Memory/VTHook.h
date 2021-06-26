#pragma once
namespace Core {
	namespace Memory {
		class VMHook
		{
		private:
			uintptr_t* vmt = nullptr;
		public:
			std::vector<uintptr_t**> p_interfaces;
			uintptr_t** p_interface = nullptr;

			uintptr_t* original_vmt = nullptr;

			uint32_t methodCount = 0;
			VMHook() = default;

			VMHook(void* p_interface)
			{
				this->p_interface = reinterpret_cast<uintptr_t**>(p_interface);
				p_interfaces.push_back(this->p_interface);

				auto method_count = 0u;

				while (reinterpret_cast<uintptr_t*>(*this->p_interface)[method_count])
					method_count++;

				original_vmt = *this->p_interface;

				vmt = new uintptr_t[method_count + 1];

				memcpy(vmt, &original_vmt[-1], sizeof(uintptr_t) * (method_count + 1));
			}
			void HookVM(void* method, size_t methodIndex) {
				if (!vmt) {
					return;
				}
				vmt[methodIndex + 1] = reinterpret_cast<uintptr_t>(method);
			}

			template<class Fn>
			Fn GetOriginalMethod(size_t methodIndex) {
				return reinterpret_cast<Fn>(original_vmt[methodIndex]);
			}

			void ApplyVMT() {
				if (!vmt || !this->p_interface) {
					return;
				}
				*this->p_interface = &vmt[1];
			}
			void ApplyVMT(void* p_interface) {
				if (!vmt) {
					return;
				}
				auto p_interface_ptr = reinterpret_cast<uintptr_t**>(p_interface);
				if (*p_interface_ptr != &vmt[1]) {
					p_interfaces.push_back(p_interface_ptr);
					*p_interface_ptr = &vmt[1];
				}
			}

			void ReleaseVMT() {
				if (!vmt||!original_vmt) {
					return;
				}
				for (auto it : p_interfaces) {
					*it = original_vmt;
				}
				delete[] vmt;
			}
		};
	}
}