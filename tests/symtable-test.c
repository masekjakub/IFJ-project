#include "../symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST(NAME, DESCRIPTION)													\
void NAME() {																	\
	printf("[%s] %s\n", #NAME, DESCRIPTION);									\
    Symtable *table = ST_initTable(TABLE_SIZE);


#define FAIL(REASON) printf("\x1b[31m[FAILED] %s\033[0m\n", REASON);
#define PASS() printf("\x1b[32m[PASSED]\033[0m\n");

#define ASSERT(EXPRESSION, REASON2FAIL)											\
if(!(EXPRESSION)){																\
	ST_freeTable(table);														\
	FAIL(REASON2FAIL);															\
    return;																		\
}

#define ENDTEST																	\
	/*Try to free*/																\
	ST_freeTable(table);														\
	PASS()																		\
	SUCCESSFUL_TESTS++;															\
}

#define TABLE_SIZE 16
#define TEST_NUM 5

unsigned int SUCCESSFUL_TESTS = 0;

const STItem ITEMS[] = {
	{.key="var1", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},
	{.key="var2", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},

	{.key="aab", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},
	{.key="aba", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},
	{.key="baa", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},

	{.key="var3", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},
	{.key="var4", .type=ST_ITEM_TYPE_VARIABLE, .data.varData.VarType = 'i'},
};

void test_ST_printTable(Symtable *table){
	STItem *curItem;
	printf("Table\n");
	for(unsigned int i = 0; i < table->size; i++){
		printf("|-[%d]",i);
		curItem = table->items[i];
		while (curItem != NULL){
			printf("->[\"%s\"]",curItem->key);
			curItem = curItem->nextItem;
		}
		printf("\n");
	}
}

TEST(test_initTable, "Initializing symtable")
	ASSERT(table->count == 0, "Table->count should be zero!")
	ASSERT(table->size == TABLE_SIZE, "Table->size isn't right!")
	for(int i = 0; i < TABLE_SIZE; i++){
		ASSERT(table->items[i] == NULL, "Items are not NULL!")
	}
ENDTEST

TEST(test_insertItem, "Inserting items")
	//Inserting 1 item
	ST_insertItem(table, ITEMS[0].key, ITEMS[0].type, ITEMS[0].data);
	STItem *itemFound = ST_searchTable(table, ITEMS[0].key);
	ASSERT(itemFound != NULL, "First inserted item was not found!")
	ASSERT(!strcmp(ITEMS[0].key,itemFound->key), "The key of first inserted item does not have the right key!")
	ASSERT(ITEMS[0].type == itemFound->type, "The type of first inserted item does not have the right type!")
	ASSERT(ITEMS[0].data.varData.VarType == itemFound->data.varData.VarType, "The data of first inserted item differ!")
	ASSERT(table->count == 1, "Table count is not right!")
	//Inserting second item
	ST_insertItem(table, ITEMS[1].key, ITEMS[1].type, ITEMS[1].data);
	itemFound = ST_searchTable(table, ITEMS[1].key);
	ASSERT(itemFound != NULL, "Second inserted item was not found! (Different index than first)")
	ASSERT(!strcmp(ITEMS[1].key,itemFound->key), "The key of second inserted item does not have the right key!")
	ASSERT(ITEMS[1].type == itemFound->type, "The type of second inserted item does not have the right type!")
	ASSERT(ITEMS[1].data.varData.VarType == itemFound->data.varData.VarType, "The data of second inserted item differ!")
	ASSERT(table->count == 2, "Table count is not right!")

	//Inserting 2 items mapping to the same index
	ST_insertItem(table, ITEMS[2].key, ITEMS[2].type, ITEMS[2].data);
	ST_insertItem(table, ITEMS[3].key, ITEMS[3].type, ITEMS[3].data);
	ASSERT(ST_searchTable(table, ITEMS[2].key),"Inserted 2 items mapping to same index. First was not found!")
	ASSERT(ST_searchTable(table, ITEMS[3].key),"Inserted 2 items mapping to same index. Second was not found!")
	ASSERT(ST_searchTable(table, ITEMS[2].key)->nextItem != NULL, "Inserted 2 items mapping to same index. They're not on the same index!")
	ASSERT(table->count == 4, "Table count is not right!")

	//Inserting item with its index occupied
	ST_insertItem(table, ITEMS[4].key, ITEMS[4].type, ITEMS[4].data);
	ASSERT(ST_searchTable(table, ITEMS[2].key),"Inserted third item mapping to occupied index. It was not found!")
	ASSERT(table->count == 5, "Table count is not right!")
ENDTEST

TEST(test_removeItem, "Adding and removing items")
	ST_insertItem(table, ITEMS[0].key, ITEMS[0].type, ITEMS[0].data);
	ST_insertItem(table, ITEMS[1].key, ITEMS[1].type, ITEMS[1].data);
	ST_insertItem(table, ITEMS[2].key, ITEMS[2].type, ITEMS[2].data);
	ST_insertItem(table, ITEMS[3].key, ITEMS[3].type, ITEMS[3].data);
	ST_insertItem(table, ITEMS[4].key, ITEMS[4].type, ITEMS[4].data);
	ASSERT(table->count == 5, "Table count is not right!")
	
	//Removing 1 item
	ST_removeItem(table, ITEMS[1].key);
	ASSERT(ST_searchTable(table, ITEMS[1].key) == NULL,"Removed item was found!")
	ST_removeItem(table, ITEMS[0].key);
	ASSERT(ST_searchTable(table, ITEMS[0].key) == NULL,"Removed item was found!")
	ASSERT(table->count == 3, "Table count is not right!")

	//Removing first item of two on the same index
	ST_removeItem(table, ITEMS[2].key);
	ASSERT(ST_searchTable(table, ITEMS[2].key) == NULL,"Removed item was found!")
	ASSERT(ST_searchTable(table, ITEMS[3].key) != NULL,"Non-removed item was not found!")
	ASSERT(ST_searchTable(table, ITEMS[4].key) != NULL,"Non-removed item was not found!")
	ASSERT(table->count == 2, "Table count is not right!")
	ST_insertItem(table, ITEMS[2].key, ITEMS[2].type, ITEMS[2].data);

	//Removing middle item of 3 on the same index ([3]->[4]->[2])
	ST_removeItem(table, ITEMS[4].key);
	ASSERT(ST_searchTable(table, ITEMS[4].key) == NULL,"Removed item was found!")
	ASSERT(ST_searchTable(table, ITEMS[2].key) != NULL,"Non-removed item was not found!")
	ASSERT(ST_searchTable(table, ITEMS[3].key) != NULL,"Non-removed item was not found!")
	ASSERT(table->count == 2, "Table count is not right!")
	
	//Removing the last of 2 items on the same index ([3]->[2])
	ST_removeItem(table, ITEMS[2].key);
	ASSERT(ST_searchTable(table, ITEMS[2].key) == NULL,"Removed item was found!")
	ASSERT(ST_searchTable(table, ITEMS[3].key) != NULL,"Non-removed item was not found!")
	ASSERT(table->count == 1, "Table count is not right!")
ENDTEST

TEST(test_expand, "Adding items to resize ST")
	//Creating and inserting filler  until expansion
	STItem fillerItems[3*TABLE_SIZE];
	int fillerItemCount = 0;
	for(int i = 0; i < 3*TABLE_SIZE; i++){
		fillerItems[i].data.varData.VarType = 'i';
		fillerItems[i].type=ST_ITEM_TYPE_VARIABLE;
		//Allocating names: x0,x1,x2,...
		if(0 > asprintf(&fillerItems[i].key, "x%d", i)) exit(1);
		ST_insertItem(table, fillerItems[i].key, fillerItems[i].type, fillerItems[i].data);
	}
	ASSERT(table->size != TABLE_SIZE, "ST did not expand!")

	//Searching for filler items
	for(int i = 0; i < fillerItemCount; i++){
		ASSERT(ST_searchTable(table, fillerItems[i].key) != NULL, "Filler item was not found!");
	}

	//Inserting after expansion
	ST_insertItem(table, ITEMS[0].key, ITEMS[0].type, ITEMS[0].data);
	ASSERT(ST_searchTable(table, ITEMS[0].key) != NULL, "Item inserted after expansion was not found!");
	
	//Freeing fillerItems' keys
	for(int i = 0; i < fillerItemCount; i++){
		free(fillerItems[i].key);
	}
ENDTEST

TEST(test_shrink, "Removing items to resize ST")
	//Inserting 7 items
	for(int i = 0; i < 7; i++){
		ST_insertItem(table, ITEMS[i].key, ITEMS[i].type, ITEMS[i].data);
	}
	ASSERT(table->count == 7, "ST doesn't have the right item count!")
	ASSERT(table->size == TABLE_SIZE, "ST doesn't have the right size!")

	//Removing 1 item
	ST_removeItem(table, ITEMS[6].key);
	ASSERT(table->size == TABLE_SIZE, "Removed 1 item. ST shouldn't have shrunk!")

	//Removing 2 more items (ST should shrink)
	ST_removeItem(table, ITEMS[5].key);
	ST_removeItem(table, ITEMS[0].key);
	ASSERT(table->size < TABLE_SIZE, "ST didn't shrink!")

	ST_insertItem(table, ITEMS[0].key, ITEMS[0].type, ITEMS[0].data);
	ASSERT(table->size < TABLE_SIZE, "ST should't have expanded")

	//Searching for items after shrinking
	for(int i = 0; i < 5; i++){
		ASSERT(ST_searchTable(table, ITEMS[i].key) != NULL, "Item after shrinking was not found!");
	}
ENDTEST

int main()
{
	printf("IFJ/IAL Project: Symtable as hashtable Tests\n");
	printf("================================================\n");

	test_initTable();
	test_insertItem();
	test_removeItem();
	test_expand();
	test_shrink();

	printf("================================================\n");
	float score = (float)SUCCESSFUL_TESTS/(float)TEST_NUM;
	if(score == 1.0){
		printf("\x1b[32m[ALL TESTS PASSED]\033[0m\n");
		return 0;
	}
	if(score == 0.0){
		printf("\x1b[31m[ALL TESTS FAILED]\033[0m\n");
		return 0;
	}
	printf("\033[93m[SCORE] %.1f%%\033[0m\n",score*100);
	return 0;
}
