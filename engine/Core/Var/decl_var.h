#pragma once
#include "decl_member.h"
#define DEFAULT_GROUP_NAME "__GLOBAL"

namespace Core {

	namespace VarUtil {

		enum var_flags {
			v_static = 0,
			v_dynamic
		};


		template<class T>
		class Var {
		public:
			void Save(FILE * Out) {
				fputs(this->t_value.t_info->name(), Out);
				fputs("%", Out);
				fputs(this->s_Name, Out);
				fputs(" ", Out);

				char buff[255];
				ZeroMemory(buff, sizeof(buff));
				auto mnemonic = t_value.t_info->name();

				if (strcmp(mnemonic, "bool") == 0) {
					sprintf_s(buff, "%d", t_value.value);
				}
				else if (strcmp(mnemonic, "int") == 0) {
					sprintf_s(buff, "%d", t_value.value);
				}
				else if (strcmp(mnemonic, "unsigned long") == 0) {
					sprintf_s(buff, "%u", t_value.value);
				}
				else if (strcmp(mnemonic, "float") == 0) {
					sprintf_s(buff, "%f", t_value.value);
				}
				else if (strcmp(mnemonic, "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >") == 0) {
					auto val = reinterpret_cast<std::string*>(&t_value.value);
					sprintf_s(buff, "%s", val->c_str());
				}
				fputs(buff, Out);

				//this->t_value.Save(Out);
			}
			void Load(std::string& buffer) {
				auto mnemonic = t_value.t_info->name();
				if (strcmp(mnemonic, "bool") == 0) {
					t_value.value = std::atoi(buffer.c_str());
				}
				else if (strcmp(mnemonic, "int") == 0) {
					t_value.value = std::atoi(buffer.c_str());
				}
				else if (strcmp(mnemonic, "unsigned long") == 0) {
					t_value.value = std::atoll(buffer.c_str());
				}
				else if (strcmp(mnemonic, "float") == 0) {
					t_value.value = std::atof(buffer.c_str());
				}
				else if (strcmp(mnemonic, "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >") == 0) {
					auto val = reinterpret_cast<std::string*>(&t_value.value);
					*val = buffer;
				}
			}
			void Load(char* buffer) {
				auto mnemonic = t_value.t_info->name();
				if (strcmp(mnemonic, "bool") == 0) {
					t_value.value = std::atoi(buffer);
				}
				else if (strcmp(mnemonic, "int") == 0) {
					t_value.value = std::atoi(buffer);
				}
				else if (strcmp(mnemonic, "unsigned long") == 0) {
					t_value.value = std::atoll(buffer);
				}
				else if (strcmp(mnemonic, "float") == 0) {
					t_value.value = std::atof(buffer);
				}
				else if (strcmp(mnemonic, "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >") == 0) {
					auto val = reinterpret_cast<std::string*>(&t_value.value);
					*val = buffer;
				}
			}
			void Set(T val) {
				this->t_value.value = val;
			}
			T& Get() {
				return this->t_value.value;
			}
			Var() {
			};
			size_t					glob_hash;
			char*					s_Name;
			char*					s_Group;
			size_t					group_hash;
			var_flags				v_flags;
			Var_Member<T>			t_value;
		private:
			UINT_PTR				garbage = 0xFEEBDEAD;
		};

		class unk_Var {
		public:
			template<class T>
			Var<T>* ExtractAs() {
				if (this->unk_val.ExtractAs<T>()) {
					return (Var<T>*)this;
				}
				return nullptr;
			}
			void Save(FILE * Out) {
				fputs(this->unk_val.t_info->name(), Out);
				fputs("%", Out);
				fputs(this->s_Name, Out);
				fputs(" ", Out);
				this->unk_val.Save(Out);
			}
			void Load(std::string& buffer) {
				return this->unk_val.Load(buffer);
			}
			void Load(char* buffer) {
				return this->unk_val.Load(std::string(buffer));
			}
			size_t					glob_hash;
			char*					s_Name;
			char*					s_Group;
			size_t					group_hash;
			var_flags				v_flags;

			unk_Var_Member			unk_val;
		};



		class sysVar {
		public:

			void BeginGroup(char* GroupName) {
				if (GroupName) {
					this->group = GroupName;
				}
			}

			void EndGroup() {
				this->group = DEFAULT_GROUP_NAME;
			}

			template<typename T>
			void RegisterVar(Var<T> & var, T def_value, std::string name, var_flags v_flags = v_static) {
				
				var.s_Name = _strdup(name.c_str());
				if (!group) {
					var.s_Group = _strdup(DEFAULT_GROUP_NAME);
				}
				else {
					var.s_Group = _strdup(group);
				}
				char group_buff[255];
				sprintf_s(group_buff, "%s", var.s_Group);
				var.group_hash = std::hash<std::string>()(group_buff);
				var.v_flags = v_flags;
				char glob_buff[255];
				sprintf_s(glob_buff, "%s%s", var.s_Name, var.s_Group);
				var.glob_hash = std::hash<std::string>()(glob_buff);
				var.t_value.Init(def_value);
				auto link = reinterpret_cast<unk_Var*>(&var);
				links.push_back(link);

				++ui_var_count;
			}

			template<typename T>
			void RegisterVar(Var<T> & var, T def_value, char * name, var_flags v_flags = v_static) {
				var.s_Name = _strdup(name);
				if (!group) {
					var.s_Group = _strdup(DEFAULT_GROUP_NAME);
				}
				else {
					var.s_Group = _strdup(group);
				}
				char group_buff[255];
				sprintf_s(group_buff, "%s", var.s_Group);
				var.group_hash = std::hash<std::string>()(group_buff);
				var.v_flags = v_flags;
				char glob_buff[255];
				sprintf_s(glob_buff, "%s%s", var.s_Name, var.s_Group);
				var.glob_hash = std::hash<std::string>()(glob_buff);
				var.t_value.Init(def_value);
				auto link = reinterpret_cast<unk_Var*>(&var);
				links.push_back(link);

				++ui_var_count;
			}
			template<typename T>
			void DeleteVar(Var<T> & var) {
				auto _off = 0u;

				if (!ui_var_count)
					return;

				for (auto it : links) {
					if (reinterpret_cast<unk_Var *>(&var) == it) {
						free(var.s_Group);
						free(var.s_Name);
						break;
					}
					_off++;
				}
				links.erase(links.begin() + _off);
				ZeroMemory(&var, sizeof(var));

				--ui_var_count;
			}

			size_t GetVarSize(unk_Var& var) {
				auto _memory = (CHAR*)&var;
				auto iter = 0u;
				while (*(UINT_PTR*)(&_memory[iter]) != 0xFEEBDEAD) {
					iter++;
				}
				return iter + sizeof(UINT_PTR);
			}

			void DeleteVar(unk_Var& var) {
				if (auto i_var = var.ExtractAs<int>()) {
					DeleteVar(*i_var);
				}
				else if (auto b_var = var.ExtractAs<bool>()) {
					DeleteVar(*b_var);
				}
				else if (auto f_var = var.ExtractAs<float>()) {
					DeleteVar(*f_var);
				}
				else if (auto ul_var = var.ExtractAs<unsigned long>()) {
					DeleteVar(*ul_var);
				}
				else if (auto s_var = var.ExtractAs<std::string>()) {
					DeleteVar(*s_var);
				}
			}
			std::vector<unk_Var*> ParseVarFlags(var_flags flag) {
				std::vector<unk_Var*> ret;
				ret.clear();
				for (auto it : links) {
					if (it->v_flags == flag) {
						ret.push_back(it);
					}
				}
				return ret;
			}
			void DeleteVarWithFlags(var_flags flag) {
				for (auto it : links) {
					if (it->v_flags == flag) {
						DeleteVar(*it);
					}
				}
			}

			void DeleteAllVar() {
				for (auto it : links) {
					DeleteVar(*it);
				}
			}

			unk_Var* FindVar(size_t hash) {
				for (auto it : links) {
					if (it->glob_hash == hash) {
						return it;
					}
				}
				return nullptr;
			}
			unk_Var* FindVar(std::string name, std::string group) {

				for (auto it : links) {
					if (!it || !it->s_Name || !it->s_Group)
						continue;
					if (strcmp(name.c_str(),it->s_Name)==0&& strcmp(group.c_str(), it->s_Group) == 0) {
						return it;
					}
				}
				
				return nullptr;
			}
			unk_Var* FindVar(char* name, char* group) {
				for (auto it : links) {
					if (!it || !it->s_Name || !it->s_Group)
						continue;
					if (strcmp(name, it->s_Name) == 0 && strcmp(group, it->s_Group) == 0) {
						return it;
					}
				}

				return nullptr;
			}

			std::vector<unk_Var*> ParseVarGroup(size_t hash) {
				std::vector<unk_Var*> ret;
				ret.clear();
				for (auto it : links) {
					if (it->group_hash == hash) {
						ret.push_back(it);
					}
				}
				return ret;
			}
			std::vector<unk_Var*> ParseVarGroup(std::string group) {
				return ParseVarGroup(std::hash<std::string>()(group));
			}
			std::vector<unk_Var*> ParseVarGroup(char* group) {
				return ParseVarGroup(std::hash<std::string>()(group));
			}

			template<typename T>
			std::vector<unk_Var*> ParseVarType() {
				std::vector<unk_Var*> ret;
				ret.clear();
				for (auto it : links) {
					if (*it->unk_val.t_info == typeid(T)) {
						ret.push_back(it);
					}
				}
				return ret;
			}

			bool SaveVarToFile(const char * FileName) {
				if (!Directory.size()) {
					this->InitDefaultDirectory();
				}
				char File[MAX_PATH];
				sprintf_s(File, "%s%s.cfg", Directory.c_str(), FileName);

				FILE * Out = nullptr;
				fopen_s(&Out, File, "w");
				if (!Out) {
					return false;
				}
				
				std::vector<size_t> Groups = {};
				for (auto it : links) {
					bool should_save = true;
					for (auto gr_hash : Groups) {
						if (it->group_hash == gr_hash) {
							should_save = false;
						}
					}
					if (should_save) {
						Groups.push_back(it->group_hash);
					}
				}
				for (auto gr_hash : Groups) {
					bool is_first = true;
					for (auto it : links) {
						if (it->group_hash == gr_hash) {
							if (is_first) {
								fputs("[", Out);
								fputs(it->s_Group, Out);
								fputs("]\n", Out);
								is_first = false;
							}
							if (auto i_var = it->ExtractAs<int>()) {
								i_var->Save(Out);
							}
							else if (auto b_var = it->ExtractAs<bool>()) {
								b_var->Save(Out);
							}
							else if (auto f_var = it->ExtractAs<float>()) {
								f_var->Save(Out);
							}
							else if (auto ul_var = it->ExtractAs<unsigned long>()) {
								ul_var->Save(Out);
							}
							else if (auto s_var = it->ExtractAs<std::string>()) {
								s_var->Save(Out);
							}
							fputs("\n", Out);
						}
					}

				}
				fclose(Out);
				return true;
			}

			bool LoadVarFromFile(const char * FileName) {
				std::ifstream InFile;
				if (!Directory.size()) {
					this->InitDefaultDirectory();
				}
				char File[MAX_PATH];
				sprintf_s(File, "%s%s.cfg", Directory.c_str(), FileName);
				InFile.open(File, std::ios_base::in);

				if (!InFile.is_open()) {
					InFile.close();
					return false;
				}
			//	DeleteVarWithFlags(var_flags::v_dynamic);

				auto ent_vec = ParseVarGroup("Ent_vec");
				for (auto it : ent_vec) {
					DeleteVar(*it);
				}
				auto fr_list = ParseVarGroup("FriendList");
				for (auto it : fr_list) {
					DeleteVar(*it);
				}
				std::string Temp;
				char TempGroup[255];
				while (!InFile.eof()) {
					std::getline(InFile, Temp);
					if (InFile.eof()) {
						break;
					}
					if (!Temp.size())
						continue;
					if (Temp[0] == '[') {
						Temp.erase(0, 1);
						Temp.erase(Temp.find(']'), 1);
						sprintf_s(TempGroup, "%s", Temp.c_str());
						continue;
					}
					else {

						
						auto dollar_pos = Temp.find('%');
						auto void_pos = Temp.find_last_of(' ');

						auto s_name(Temp);
						s_name.erase(0, dollar_pos + 1);
						s_name.erase(void_pos - dollar_pos - 1, s_name.size() - void_pos + dollar_pos + 1);

						auto s_val(Temp);
						s_val.erase(0, void_pos + 1);

						auto s_type(Temp);
						s_type.erase(dollar_pos, 0xFFFF);

						auto var = FindVar(s_name.c_str(), TempGroup);
						if (var&&strcmp(var->unk_val.t_info->name(), s_type.c_str()) == 0) {
							if (strcmp(s_type.c_str(), "bool") == 0) {
								if (auto b_var = var->ExtractAs<bool>()) {
									b_var->Load(s_val);

								}
							}
							else if (strcmp(s_type.c_str(), "int") == 0) {
								if (auto i_var = var->ExtractAs<int>()) {
									i_var->Load(s_val);

								}
							}
							else if (strcmp(s_type.c_str(), "unsigned long") == 0) {
								if (auto ul_var = var->ExtractAs<unsigned long>()) {
									ul_var->Load(s_val);

								}
							}
							else if (strcmp(s_type.c_str(), "float") == 0) {
								if (auto f_var = var->ExtractAs<float>()) {
									f_var->Load(s_val);
									
								}
							}
							else if (strcmp(s_type.c_str(), "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >") == 0) {
								if (auto s_var = var->ExtractAs<std::string>()) {
									s_var->Load(s_val);

								}
							}
						}
						else {
							for (auto it : links) {
								if (strcmp(it->unk_val.t_info->name(), s_type.c_str()) == 0) {
									auto var_size = 60u;
									if (s_type == "bool") {
										var_size = sizeof(Var<bool>);
									}
									else if (s_type == "int") {
										var_size = sizeof(Var<int>);
									}
									else if (s_type == "unsigned long") {
										var_size = sizeof(Var<unsigned long>);
									}
									else if (s_type == "float") {
										var_size = sizeof(Var<float>);
									}
									else if (s_type == "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >") {
										var_size = sizeof(Var<std::string>);
									}

									auto memory = new CHAR[var_size];
									memcpy(memory, it, var_size);
									auto dyn_var = reinterpret_cast<unk_Var*>(memory);

									dyn_var->s_Group = _strdup(TempGroup);
									char group_buff[255];
									sprintf_s(group_buff, "%s", dyn_var->s_Group);
									dyn_var->s_Name = _strdup(s_name.c_str());
									char glob_buff[255];
									sprintf_s(glob_buff, "%s%s", dyn_var->s_Name, dyn_var->s_Group);

									dyn_var->glob_hash = std::hash<std::string>()(glob_buff);
									dyn_var->group_hash = std::hash<std::string>()(group_buff);
									dyn_var->v_flags = v_dynamic;
									if (strcmp(s_type.c_str(), "bool") == 0) {
										if (auto b_var = dyn_var->ExtractAs<bool>()) {
											b_var->Load(s_val);

										}
									}
									else if (strcmp(s_type.c_str(), "int") == 0) {
										if (auto i_var = dyn_var->ExtractAs<int>()) {
											i_var->Load(s_val);

										}
									}
									else if (strcmp(s_type.c_str(), "unsigned long") == 0) {
										if (auto ul_var = dyn_var->ExtractAs<unsigned long>()) {
											ul_var->Load(s_val);

										}
									}
									else if (strcmp(s_type.c_str(), "float") == 0) {
										if (auto f_var = dyn_var->ExtractAs<float>()) {
											f_var->Load(s_val);

										}
									}
									else if (strcmp(s_type.c_str(), "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >") == 0) {
										if (auto s_var = dyn_var->ExtractAs<std::string>()) {
											s_var->Load(s_val);

										}
									}

									//dyn_var->Load(s_val);
									links.push_back(dyn_var);

									++ui_var_count;
									break;
								}
							}


						}
					}
				}
				InFile.close();
				return true;
			}




			void InitDefaultDirectory() {
				char buff[MAX_PATH];
				ZeroMemory(buff, sizeof(buff));
				GetEnvironmentVariable("APPDATA", buff, MAX_PATH);
				std::string str(buff);
				str += "\\";

				this->Directory = str;
			}

			std::string Directory;
			const char * Prefix = ".cfg";

			char* group ;
			std::vector<unk_Var*> links;

			UINT_PTR ui_var_count = 0u;
		};
	};
};
