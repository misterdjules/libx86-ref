#ifndef _INTEL_X86_REF_H_
#define _INTEL_X86_REF_H_

#define X86_REF_OK 0

#ifdef _WIN32
	#ifdef X86_REF_DLL_EXPORT
		#define X86_REF_API __declspec(dllexport)
	#elif !defined(X86_REF_STATIC)
		#define X86_REF_API __declspec(dllimport)
	#else
		#define X86_REF_API
	#endif /* X86_REF_DLL_EXPORT */
#else
	#define X86_REF_API
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif /* _cplusplus */

typedef struct ref_database_s 		ref_database_t;
typedef struct instruction_s 		instruction_t;
typedef struct instructions_list_s 	instructions_list_t;

/*
 * Give a file path to a SQLite database, returns a pointer to
 * a ref_database_t that can be used to query the x86 reference documentation.
 * Returns NULL in case of error.
 */
X86_REF_API ref_database_t* x86_ref_open_database(const char* db_path);

/*
 * Closes and releases the resources of the x86 reference database "ref_db".
 * Returns X86_REF_OK if successful, or any other value otherwise.
 */
X86_REF_API int 			x86_ref_close_database(ref_database_t** ref_db);

/*
 * Returns a pointer to a instructions_list_t that can be used to iterate over all
 * instructions stored in the database "ref_db".
 * The returned list can later be destroyed by using the "x86_ref_destroy_instructions_list".
 */
X86_REF_API instructions_list_t* 	x86_ref_get_all_instructions(const ref_database_t* ref_db);

/*
 * Returns a pointer to a instructions_list_t that can be used to iterate over all
 * instructions stored in the database "ref_db" that match the prefix "prefix".
 * For instance:
 * x86_ref_search_instructions_by_mnemonic(ref_db, "A");
 * will return the instructions "AAA", "AAD", "AAM", "AAS", "ADC", "ADD", etc.
 * The returned list can later be destroyed by using the "x86_ref_destroy_instructions_list".
 */
X86_REF_API instructions_list_t* 	x86_ref_search_instructions_by_mnemonic(const ref_database_t* ref_db,
																			const char* prefix);

/*
 * Returns a pointer to the next instruction_t object stored in the list "instructions_list".
 * Returns NULL when it reaches the end of the list.
 */
X86_REF_API instruction_t* 	x86_ref_next_instruction_from_list(instructions_list_t** instructions_list);

/*
 * Returns the number of elements in the list "instructions_list".
 */
X86_REF_API size_t x86_ref_instructions_list_get_size(const instructions_list_t* instructions_list);

/*
 * Deallocates all resources used by the list "instructions_list".
 * Returns X86_REF_OK if successful, any other value otherwise.
 */
X86_REF_API int x86_ref_destroy_instructions_list(instructions_list_t* instructions_list);

/*
 * Returns a pointer to an instruction_t given a mnemonic.
 */
X86_REF_API instruction_t* 	x86_ref_get_instruction_by_mnemonic(const ref_database_t* ref_db,
																const char* mnemonic);

/*
 * Returns a string representing the mnemonic for instruction "instruction".
 * For instance, if "instruction" points to an ADD instruction,
 * x86_ref_get_instruction_mnemonic will return the string "ADD".
 *
 * Returns NULL if instruction is NULL.
 */
X86_REF_API const char* x86_ref_get_instruction_mnemonic(const instruction_t* instruction);

/*
 * Returns a string representing the synopsis for instruction "instruction".
 * For instance, if "instruction" points to an ADD instruction,
 * x86_ref_get_instruction_mnemonic will return the string "Add".
 *
 * Returns NULL if instruction is NULL.
 */
X86_REF_API const char* x86_ref_get_instruction_synopsis(const instruction_t* instruction);

/*
 * Returns a string representing the opcode for instruction "instruction".
 * For instance, if "instruction" points to an ADD instruction,
 * x86_ref_get_instruction_opcode will return the string "04".
 *
 * Returns NULL if instruction is NULL.
 */
X86_REF_API const char* x86_ref_get_instruction_opcode(const instruction_t* instruction);

/*
 * Returns a string representing the short description for instruction "instruction".
 * For instance, if "instruction" points to an ADD instruction,
 * x86_ref_get_instruction_short_desc will return a string that describes what the ADD
 * instruction does.
 *
 * Returns NULL if instruction is NULL.
 */
X86_REF_API const char* x86_ref_get_instruction_short_desc(const instruction_t* instruction);


X86_REF_API const char* x86_ref_errmsg(ref_database_t* ref_db);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _INTEL_X86_REF_H_ */