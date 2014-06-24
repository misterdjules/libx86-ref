#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <sqlite3.h>

#include <debug.h>
#include <sqlite_helper.h>
#include <jstring.h>

#include "intel_x86_ref.h"

CREATE_DEBUG_CTX(x86_ref);

struct ref_database_s
{
	sqlite3* db;
};

struct instruction_s
{
	const char* mnemonic;
	const char* synopsis;
	const char* opcode;
	const char* short_desc;
	const char* long_desc;
	const char* affected_flags;
};

struct instructions_list_s
{
	struct instruction_s* instruction;
	struct instructions_list_s* next;
};

instruction_t* x86_ref_create_empty_instruction(void)
{
	instruction_t* new_instruction = malloc(sizeof(instruction_t));
	memset(new_instruction, 0, sizeof(*new_instruction));

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

const char* x86_ref_get_instruction_synopsis(const instruction_t* instruction)
{
	assert(instruction);

	return instruction->synopsis;
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

const char* x86_ref_get_instruction_long_desc(const instruction_t* instruction)
{
	assert(instruction);

	return instruction->long_desc;
}

typedef int (*set_field_operation_t)(instruction_t* instruction, const char* value);

typedef struct set_fields_operation_spec_s
{
	const char* 			field_name;
	set_field_operation_t 	op;
} set_field_operation_spec_t;

static int set_field_mnemonic(instruction_t* instruction, const char* value)
{
	/*
	 * strduping the value here since it's managed by sqlite3 and will
	 * be deallocated in the near future.
	 */
	instruction->mnemonic = strdup(value);

	return X86_REF_OK;
}

static int set_field_opcode(instruction_t* instruction, const char* value)
{
	/*
	 * strduping the value here since it's managed by sqlite3 and will
	 * be deallocated in the near future.
	 */
	instruction->opcode = strdup(value);

	return X86_REF_OK;
}

static int set_field_synopsis(instruction_t* instruction, const char* value)
{
	/*
	 * strduping the value here since it's managed by sqlite3 and will
	 * be deallocated in the near future.
	 */
	x86_ref_debug("Setting synopsis to value [%s]\n", value);
	instruction->synopsis = strdup(value);

	return X86_REF_OK;
}

static int set_field_short_desc(instruction_t* instruction, const char* value)
{
	/*
	 * strduping the value here since it's managed by sqlite3 and will
	 * be deallocated in the near future.
	 */
	instruction->short_desc = strdup(value);

	return X86_REF_OK;
}

static int set_field_long_desc(instruction_t* instruction, const char* value)
{
	/*
	 * strduping the value here since it's managed by sqlite3 and will
	 * be deallocated in the near future.
	 */
	instruction->long_desc = strdup(value);

	return X86_REF_OK;
}

static int set_field_affected_flags(instruction_t* instruction, const char* value)
{
	/*
	 * strduping the value here since it's managed by sqlite3 and will
	 * be deallocated in the near future.
	 */
	instruction->affected_flags = strdup(value);

	return X86_REF_OK;
}

static set_field_operation_spec_t set_fields_op[] = {
	{ "mnemonic", 			set_field_mnemonic },
	{ "opcode", 			set_field_opcode },
	{ "synopsis", 			set_field_synopsis },
	{ "short_description", 	set_field_short_desc },
	{ "long_description", 	set_field_long_desc },
	{ "affected_flags", 	set_field_affected_flags },
};
static size_t nb_instruction_fields = 	sizeof(set_fields_op) /
										sizeof(set_field_operation_spec_t);

static int instructions_have_field(const char* field_name)
{
	size_t field_index = 0;

	for (; field_index < nb_instruction_fields; ++field_index)
	{
		if (!strcmp(set_fields_op[field_index].field_name, field_name))
		{
			return 1;
		}
	}

	return 0;
}

int x86_ref_instruction_set_field(instruction_t* instruction,
								  const char* field_name,
								  const char* value)
{
	int valid_field = 0;
	int field_op_index = 0;

	assert(instruction);
	assert(field_name);
	assert(value);

	if (!instruction || !field_name || !value)
	{
		return 1;
	}

	valid_field = instructions_have_field(field_name);
	assert(valid_field);
	if (!valid_field)
	{
		return 1;
	}

	for (field_op_index = 0; field_op_index < nb_instruction_fields; ++field_op_index)
	{
		set_field_operation_spec_t field_op = set_fields_op[field_op_index];

		if (!strcmp(field_op.field_name, field_name))
		{
			if (field_op.op(instruction, value) != X86_REF_OK)
				return 1;

			return X86_REF_OK;
		}
	}

	/* We did not find the field with name "field_name" in the instructions'
	 * field table, return an error.
	 */
	return 1;
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

	rc = sqlite3_open_v2(db_path, &sqlite3_db, SQLITE_OPEN_READONLY, NULL);
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
	/*
	 * Disable warning about strerror being unsafe
	 * because it doesn't affect us.
	 */
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

size_t x86_ref_instructions_list_get_size(const instructions_list_t* instructions_list)
{
	size_t size = 0;
	while (instructions_list)
	{
		++size;
		instructions_list = instructions_list->next;
	}

	return size;
}

static int get_all_instructions_cb(void* instructions,
								   int nb_columns,
								   char** values,
								   char** names)
{
	int column_index                       = 0;
	instruction_t* new_instruction         = NULL;
	instructions_list_t** instructions_list = (instructions_list_t**)instructions;

	x86_ref_debug("get_all_instructions_cb called!\n");

	new_instruction = x86_ref_create_empty_instruction();
	for (column_index = 0; column_index < nb_columns; ++column_index)
	{
		x86_ref_debug("column: [%s], value: [%s]\n", names[column_index], values[column_index]);
		if (values[column_index] &&
			(x86_ref_instruction_set_field(new_instruction,
										   names[column_index],
										   values[column_index]) != X86_REF_OK))
		{
			return 1;
		}
	}

	if (x86_ref_add_instruction_to_list(instructions_list, new_instruction) != X86_REF_OK)
	{
		x86_ref_debug("Couldn't add instruction to instructions list!");
		return 1;
	}

	return 0;
}

static instructions_list_t* get_instructions_list(const ref_database_t* ref_db,
												const char** fields,
												const char* where_clause)
{
	instructions_list_t* instructions_list 	= NULL;
	sql_statement_t* sql_statement         	= NULL;
	const char* sql_statement_string 		= NULL;
	char* err_msg 							= NULL;

	assert(ref_db);

	if (!ref_db)
		return NULL;

	sql_statement = sqlite_helper_create_sql_statement("SELECT",
													   fields,
													   "instructions",
													   where_clause);
	if (!sql_statement)
		return NULL;

	sql_statement_string = sqlite_helper_sql_statement_to_string(sql_statement);
	x86_ref_debug("SQL statement: [%s]\n", sql_statement_string);
	if (sqlite3_exec(ref_db->db,
					 sql_statement_string,
					 &get_all_instructions_cb,
					 (void*)&instructions_list,
					 &err_msg) != SQLITE_OK)
	{
		fprintf(stderr, "An error occured when executing an SQL statement:\n%s\n",
				sql_statement_string);

		if (err_msg)
			fprintf(stderr, "reason: %s\n", err_msg);

		return NULL;
	}

	return instructions_list;
}

instructions_list_t* x86_ref_get_all_instructions(const ref_database_t* ref_db)
{
	instructions_list_t* instructions_list = NULL;
	const char* row_fields[]               = { "*", NULL };

	return get_instructions_list(ref_db, row_fields, NULL);
}

X86_REF_API instructions_list_t* x86_ref_search_instructions_by_mnemonic(const ref_database_t* ref_db,
																		 const char* prefix)
{
	const char* row_fields[]               = { "*", NULL };
	const char* where_clause = NULL;

	where_clause = jstr_format("mnemonic LIKE '%s%%'", prefix);
	if (!where_clause)
		return NULL;

	return get_instructions_list(ref_db, row_fields, where_clause);
}

instruction_t* x86_ref_get_instruction_by_mnemonic(const ref_database_t* ref_db,
												   const char* mnemonic)
{
	instructions_list_t* instructions_list 	= NULL;
	const char* where_clause               	= NULL;
	int nb_instructions 					= 0;
	const char* row_columns[] 				= { "*", NULL};

	assert(ref_db);
	assert(mnemonic);

	if (!ref_db || !mnemonic)
		return NULL;

	where_clause = jstr_format("mnemonic='%s'", mnemonic);
	if (!where_clause)
		return NULL;

	instructions_list = get_instructions_list(ref_db, row_columns, where_clause);
	nb_instructions = x86_ref_instructions_list_get_size(instructions_list);
	assert(nb_instructions <= 1);
	if (nb_instructions > 1)
	{
		return NULL;
	}

	return x86_ref_next_instruction_from_list(&instructions_list);
}
