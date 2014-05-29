#ifndef _INTEL_X86_REF_H_
#define _INTEL_X86_REF_H_

#define X86_REF_OK 0

typedef struct ref_database_s 		ref_database_t;
typedef struct instruction_s 		instruction_t;
typedef struct instructions_list_s 	instructions_list_t;

instructions_list_t* 	x86_ref_create_instructions_list(void);
int 					x86_ref_add_instruction_to_list(instructions_list_t** instructions_list,
														instruction_t* instruction);
instruction_t* 			x86_ref_next_instruction_from_list(instructions_list_t** instructions_list);
instruction_t* 			x86_ref_get_instruction_by_mnemonic(const ref_database_t* ref_db,
															const char* mnemonic);

const char* x86_ref_get_instruction_mnemonic(const instruction_t* instruction);
const char* x86_ref_get_instruction_opcode(const instruction_t* instruction);
const char* x86_ref_get_instruction_short_desc(const instruction_t* instruction);

ref_database_t* x86_ref_open_database(const char* db_path);
int 			x86_ref_close_database(ref_database_t** ref_db);

const char* 	x86_ref_errmsg(ref_database_t* ref_db);

int 			x86_ref_search_mnemonic(const ref_database_t* ref_db,
										const char* pattern,
										instruction_t* instructions);

instructions_list_t* x86_ref_list_instructions(const ref_database_t* ref_db);

#endif /* _INTEL_X86_REF_H_ */