#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <sqlite3.h>

#include "intel_x86_ref.h"

#define SELECT_ALL_FROM_INSTRUCTIONS	"SELECT * FROM instructions"
#define SELECT_INSTRUCTION_BY_MNEMONIC	"SELECT * FROM instructions WHERE mnemonic=(?)"

struct ref_database_s
{
	sqlite3* db;
};

struct instruction_s
{
	const char* mnemonic;
	const char* opcode;
	const char* short_desc;
	const char* long_desc;
};

struct instructions_list_s
{
	struct instruction_s* instruction;
	struct instructions_list_s* next;
};

instruction_t* x86_ref_create_instruction(const char* mnemonic,
										  const char* opcode,
										  const char* short_desc)
{
	instruction_t* new_instruction = malloc(sizeof(instruction_t));
	if (!new_instruction)
		return NULL;

#ifdef _WIN32
	#pragma warning(push)
	#pragma warning(disable: 4996)
#endif /* _WIN32 */
	new_instruction->mnemonic   = strdup(mnemonic);
	new_instruction->opcode     = strdup(opcode);
	new_instruction->short_desc = strdup(short_desc);
#ifdef _WIN32
	#pragma warning(pop)
#endif /* _WIN32 */
	return new_instruction;
}

void x86_ref_destroy_instruction(instruction_t** instruction)
{
	assert(instruction);
	assert(*instruction);

	if (instruction && *instruction)
	{
		free((char*)(*instruction)->mnemonic);
	}

	*instruction = NULL;
}

const char* x86_ref_get_instruction_mnemonic(const instruction_t* instruction)
{
	assert(instruction);

	return instruction->mnemonic;
}

const char* x86_ref_get_instruction_opcode(const instruction_t* instruction)
{
	assert(instruction);

	return instruction->opcode;
}

const char* x86_ref_get_instruction_short_desc(const instruction_t* instruction)
{
	assert(instruction);

	return instruction->short_desc;
}

int x86_ref_destroy_instructions_list(instructions_list_t* instructions_list)
{
	instructions_list_t* node_to_deallocate = NULL;

	assert(instructions_list);

	if (!instructions_list)
		return 1;

	while (instructions_list)
	{
		node_to_deallocate = instructions_list;
		instructions_list = instructions_list->next;

		node_to_deallocate->next = NULL;
		x86_ref_destroy_instruction(&node_to_deallocate->instruction);
		free(node_to_deallocate);
	}

	return X86_REF_OK;
}

int x86_ref_add_instruction_to_list(instructions_list_t** instructions_list,
									instruction_t* instruction)
{
	instructions_list_t* new_element = NULL;

	assert(instructions_list);

	if (!instructions_list)
		return 1;

	new_element = malloc(sizeof(instructions_list_t));
	assert(new_element);
	if (!new_element)
		return 1;

	new_element->next = NULL;
	new_element->instruction = instruction;

	while (*instructions_list && (*instructions_list)->next)
		instructions_list = &((*instructions_list)->next);

	if (!*instructions_list)
		*instructions_list = new_element;
	else
		(*instructions_list)->next = new_element;

	return X86_REF_OK;
}

instruction_t* 	x86_ref_next_instruction_from_list(instructions_list_t** instructions_list)
{
	instruction_t* next_instruction = NULL;

	assert(instructions_list);

	if (instructions_list && *instructions_list)
	{
		next_instruction = (*instructions_list)->instruction;
		*instructions_list = (*instructions_list)->next;
	}

	return next_instruction;
}

ref_database_t* x86_ref_open_database(const char* db_path)
{
	sqlite3* sqlite3_db    = NULL;
	int rc                 = -1;
	ref_database_t* ref_db = NULL;

	rc = sqlite3_open(db_path, &sqlite3_db);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Could not open database at path [%s], reason: %s\n",
				db_path,
				sqlite3_errmsg(sqlite3_db));
		sqlite3_close(sqlite3_db);
		return NULL;
	}

	ref_db = malloc(sizeof(ref_database_t));
	if (!ref_db)
	{
#ifdef _WIN32
	#pragma warning(push)
	#pragma warning(disable: 4996)
#endif /* _WIN32 */
		fprintf(stderr, "Could not allocate memory, reason: %s\n", strerror(errno));
#ifdef _WIN32
	#pragma warning(pop)
#endif /* _WIN32 */
		return NULL;
	}

	ref_db->db = sqlite3_db;

	return ref_db;
}

int x86_ref_close_database(ref_database_t** ref_db)
{
	assert(ref_db && *ref_db);

	if (!*ref_db || !(*ref_db)->db)
	{
		return 1;
	}

	if (sqlite3_close((*ref_db)->db) != SQLITE_OK)
	{
		return 1;
	}

	free(*ref_db);
	*ref_db = NULL;

	return X86_REF_OK;
}

const char* x86_ref_errmsg(ref_database_t* ref_db)
{
	assert(ref_db);

	if (ref_db && ref_db->db)
	{
		return sqlite3_errmsg(ref_db->db);
	}

	return NULL;
}

int x86_ref_search_instructions_by_mnemonic(const ref_database_t* ref_db,
							const char* pattern,
							instruction_t* instructions)
{
	return 1;
}

instructions_list_t* x86_ref_get_all_instructions(const ref_database_t* ref_db)
{
	int rc             = -1;
	sqlite3_stmt* stmt = NULL;
	instructions_list_t* instructions_list = NULL;

	assert(ref_db);
	if (!ref_db)
		return NULL;

	rc = sqlite3_prepare_v2(ref_db->db,
						 	SELECT_ALL_FROM_INSTRUCTIONS,
						 	strlen(SELECT_ALL_FROM_INSTRUCTIONS),
						 	&stmt,
						 	NULL);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Could not prepare statement [%s], reason: %s\n",
				SELECT_ALL_FROM_INSTRUCTIONS,
				sqlite3_errmsg(ref_db->db));
		sqlite3_close(ref_db->db);
		return NULL;
	}

	instructions_list = NULL;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		const char* mnemonic   = (const char*)sqlite3_column_text(stmt, 0);
		const char* opcode     = (const char*)sqlite3_column_text(stmt, 1);
		const char* short_desc = (const char*)sqlite3_column_text(stmt, 2);

		if (x86_ref_add_instruction_to_list(&instructions_list,
											x86_ref_create_instruction(mnemonic,
																	   opcode,
																	   short_desc)
											) != X86_REF_OK)
		{
			x86_ref_destroy_instructions_list(instructions_list);
			return NULL;
		}
	}

	if (rc != SQLITE_DONE)
	{
		x86_ref_destroy_instructions_list(instructions_list);
		return NULL;
	}

	if (sqlite3_finalize(stmt) != SQLITE_OK)
	{
		x86_ref_destroy_instructions_list(instructions_list);
		return NULL;
	}

	return instructions_list;
}

instruction_t* x86_ref_get_instruction_by_mnemonic(const ref_database_t* ref_db,
												   const char* mnemonic)
{
	int rc                     = -1;
	sqlite3_stmt* stmt         = NULL;
	unsigned int row_number    = 0;
	instruction_t* instruction = NULL;

	const char* opcode     = NULL;
	const char* short_desc = NULL;

	assert(ref_db);
	if (!ref_db)
		return NULL;

	rc = sqlite3_prepare_v2(ref_db->db,
							SELECT_INSTRUCTION_BY_MNEMONIC,
							strlen(SELECT_INSTRUCTION_BY_MNEMONIC),
							&stmt,
							NULL);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Couldn't prepare statement [%s], reason: %s\n",
			SELECT_INSTRUCTION_BY_MNEMONIC,
			sqlite3_errmsg(ref_db->db));
		sqlite3_close(ref_db->db);
		return NULL;
	}

	rc = sqlite3_bind_text(stmt, 1, mnemonic, strlen(mnemonic), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Couldn't bin the mnemonic parameter to the SQL statement, reason: %s\n",
			    sqlite3_errmsg(ref_db->db));
		sqlite3_close(ref_db->db);
		return NULL;
	}

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		assert(row_number == 0);
		if (row_number != 0)
			break;

		opcode     = (const char*)sqlite3_column_text(stmt, 1);
		short_desc = (const char*)sqlite3_column_text(stmt, 2);

		instruction = x86_ref_create_instruction(mnemonic, opcode, short_desc);

		++row_number;
	}

	if (rc != SQLITE_DONE)
	{
		fprintf(stderr, "Warning: SQLite statement not done\n");
	}

	if (sqlite3_finalize(stmt) != SQLITE_OK)
	{
		fprintf(stderr, "Couldn't finalize SQLite statement\n");
	}

	return instruction;
}