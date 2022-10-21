#include "../symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 16

#define TEST(NAME, DESCRIPTION)													\
void NAME() {																	\
	printf("[%s] %s\n", #NAME, DESCRIPTION);									\
    Symtable *table = ST_initTable(TABLE_SIZE);


#define FAIL(REASON) printf("\x1b[31m[FAILED] %s\033[0m\n", REASON);
#define PASS() printf("\x1b[32m[PASSED]\033[0m\n");

#define ASSERT(EXPRESSION, REASON2FAIL)											\
if(!(EXPRESSION)){																\
	FAIL(REASON2FAIL);															\
    return;																		\
}

#define ENDTEST																	\
	/*Try to free*/																\
	ST_freeTable(table);														\
	PASS()																		\
}

const STItem items[] = {
	{.key="var1", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},
	{.key="var2", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},

	{.key="aab", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},
	{.key="aba", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},
	{.key="baa", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},
};


TEST(test_initTable, "Initializing symtable")
	ASSERT(table->count == 0, "Table->count should be zero!")
	ASSERT(table->size == TABLE_SIZE, "Table->size isn't right!")
	for(int i = 0; i < TABLE_SIZE; i++){
		ASSERT(table->items[i] == NULL, "Items are not NULL!")
	}
ENDTEST

TEST(test_insertItem, "Inserting items")
	//Inserting 1 item
	ST_insertItem(table, items[0].key, items[0].type, items[0].data);
	STItem *itemFound = ST_searchTable(table, items[0].key);
	ASSERT(itemFound != NULL, "First inserted item was not found!")
	ASSERT(!strcmp(items[0].key,itemFound->key), "The key of first inserted item does not have the right key!")
	ASSERT(items[0].type == itemFound->type, "The type of first inserted item does not have the right type!")
	ASSERT(items[0].data.varData.VarType == itemFound->data.varData.VarType, "The data of first inserted item differ!")

	//Inserting second item
	ST_insertItem(table, items[1].key, items[1].type, items[1].data);
	itemFound = ST_searchTable(table, items[1].key);
	ASSERT(itemFound != NULL, "Second inserted item was not found! (Different index than first)")
	ASSERT(!strcmp(items[1].key,itemFound->key), "The key of second inserted item does not have the right key!")
	ASSERT(items[1].type == itemFound->type, "The type of second inserted item does not have the right type!")
	ASSERT(items[1].data.varData.VarType == itemFound->data.varData.VarType, "The data of second inserted item differ!")

	//Inserting 2 items mapping to the same index
	ST_insertItem(table, items[2].key, items[2].type, items[2].data);
	ST_insertItem(table, items[3].key, items[3].type, items[3].data);
	ASSERT(ST_searchTable(table, items[2].key),"Inserted 2 items mapping to same index. First was not found!")
	ASSERT(ST_searchTable(table, items[3].key),"Inserted 2 items mapping to same index. Second was not found!")
	ASSERT(ST_searchTable(table, items[2].key)->nextItem != NULL, "Inserted 2 items mapping to same index. They're not on the same index!")

	//Inserting item with its index occupied
	ST_insertItem(table, items[4].key, items[4].type, items[4].data);
	ASSERT(ST_searchTable(table, items[2].key),"Inserted third item mapping to occupied index. It was not found!")
ENDTEST

TEST(test_removeItem, "Adding and removing items")
	ST_insertItem(table, items[0].key, items[0].type, items[0].data);
	ST_insertItem(table, items[1].key, items[1].type, items[1].data);
	ST_insertItem(table, items[2].key, items[2].type, items[2].data);
	ST_insertItem(table, items[3].key, items[3].type, items[3].data);
	ST_insertItem(table, items[4].key, items[4].type, items[4].data);

	//Removing 1 item
	ST_removeItem(table, items[0].key);
	ASSERT(ST_searchTable(table, items[0].key) == NULL,"Removed item was found!")

	//Removing items on the same index
	ST_removeItem(table, items[3].key);
	ASSERT(ST_searchTable(table, items[3].key) == NULL,"Removed item was found!")
	ASSERT(ST_searchTable(table, items[2].key) != NULL,"Non-removed item was not found!")
	ASSERT(ST_searchTable(table, items[4].key) != NULL,"Non-removed item was not found!")
	ST_removeItem(table, items[2].key);
	ASSERT(ST_searchTable(table, items[2].key) == NULL,"Removed item was found!")

ENDTEST

int main()
{
	printf("IFJ: Symtable as hashtable Tests\n");
	printf("======================================\n");

	test_initTable();
	test_insertItem();
	test_removeItem();

	return 0;
}
