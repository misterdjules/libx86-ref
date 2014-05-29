#ifndef _INTEL_X86_REF_H_
#define _INTEL_X86_REF_H_

#define X86_REF_OK 0

#ifndef X86_REF_API
	#define X86_REF_API
#endif /* X86_REF_API */

typedef struct ref_database_s 		ref_database_t;
typedef struct instruction_s 		instruction_t;
typedef struct instructions_list_s 	instructions_list_t;

X86_REF_API instructions_list_t* 	x86_ref_create_instructions_list(void);
X86_REF_API int 					x86_ref_add_instruction_to_list(instructions_list_t** instructions_list,
														instruction_t* instruction);
X86_REF_API instruction_t* 			x86_ref_next_instruction_from_list(instructions_list_t** instructions_list);
X86_REF_API instruction_t* 			x86_ref_get_instruction_by_mnemonic(const ref_database_t* ref_db,
															const char* mnemonic);

X86_REF_API const char* x86_ref_get_instruction_mnemonic(const instruction_t* instruction);
X86_REF_API const char* x86_ref_get_instruction_opcode(const instruction_t* instruction);
X86_REF_API const char* x86_ref_get_instruction_short_desc(const instruction_t* instruction);

X86_REF_API ref_database_t* x86_ref_open_database(const char* db_path);
X86_REF_API int 			x86_ref_close_database(ref_database_t** ref_db);

X86_REF_API const char* 	x86_ref_errmsg(ref_database_t* ref_db);

X86_REF_API int 			x86_ref_search_mnemonic(const ref_database_t* ref_db,
										const char* pattern,
										instruction_t* instructions);

X86_REF_API instructions_list_t* x86_ref_list_instructions(const ref_database_t* ref_db);

#endif /* _INTEL_X86_REF_H_ */