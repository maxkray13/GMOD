#pragma once
#include "../../../include/stdafx.h"
template <typename T>
T lexical_cast(const std::string& str)
{
	T var;
	std::istringstream iss;
	iss.str(str);
	iss >> var;

	return var;
}

//#include <boost/lexical_cast.hpp>
//#include <boost/array.hpp>
namespace Core {

	namespace VarUtil {
		class _Member_Func {
		public:
			virtual void Save(FILE * Out) {};
			virtual void Load(std::string& buffer) {};
		};

		template<class T>
		class Var_Member;

		class unk_Var_Member : public _Member_Func {
		public:
			template<class T>
			Var_Member<T>* ExtractAs() {
				if (t_info&&typeid(T) == *t_info) {
					return (Var_Member<T>*)this;
				}
				return nullptr;
			}

			const std::type_info*	t_info;
		};

		template<class T>
		class Var_Member :public unk_Var_Member {
		public:
			void Save(FILE * Out) {
				char buff[255];
				ZeroMemory(buff, sizeof(buff));
				auto mnemonic = t_info->name();

				if (strcmp(mnemonic,"bool")==0) {
					sprintf_s(buff, "%d", value);
				}
				else if (strcmp(mnemonic, "int") == 0) {
					sprintf_s(buff, "%d", value);
				}
				else if (strcmp(mnemonic, "unsigned long") == 0) {
					sprintf_s(buff, "%u", value);
				}
				else if (strcmp(mnemonic, "float") == 0) {
					sprintf_s(buff, "%f", value);
				}
				
				fputs(buff, Out);
			}
			void Load(std::string& buffer) {

				this->value = lexical_cast<T>(buffer);
			}
			void Init(T value) {
				this->t_info = &typeid(T);
				this->value = value;
			}
		public:
			T value;
		};
	};
};