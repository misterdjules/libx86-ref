#include <gtest/gtest.h>

#include <intel_x86_ref.h>

#define X86_REF_DATABASE_PATH "./x86-ref.sqlite"

TEST(x86_ref_get_all_instructions, basic)
{
	ref_database_t* ref_db                 = x86_ref_open_database(X86_REF_DATABASE_PATH);
	instructions_list_t* instructions_list = NULL;
	instruction_t* instruction             = NULL;

	ASSERT_TRUE(ref_db != NULL);

	instructions_list = x86_ref_get_all_instructions(ref_db);
	EXPECT_TRUE(instructions_list != NULL);

	while ((instruction = x86_ref_next_instruction_from_list(&instructions_list)) != NULL)
	{
		fprintf(stdout, "Instruction: %s\n", x86_ref_get_instruction_mnemonic(instruction));
		fprintf(stdout, "OpCode: %s\n", x86_ref_get_instruction_opcode(instruction));
		fprintf(stdout, "Short desc: %s\n", x86_ref_get_instruction_short_desc(instruction));
	}

	instruction = x86_ref_get_instruction_by_mnemonic(ref_db, "ADD");
	ASSERT_TRUE(instruction != NULL);
	EXPECT_STREQ("ADD", x86_ref_get_instruction_mnemonic(instruction));

	ASSERT_EQ(x86_ref_close_database(&ref_db), X86_REF_OK);
}