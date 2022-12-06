/**
 * @file error.h
 * @author Vojta Kuchař
 * @brief Contains all defined error return codes
 * @version 1.0
 * @date 2022-10-06
 */

#ifndef _IFJ_ERROR
#define _IFJ_ERROR

typedef enum {
    ERR_LEX = 1,
    ERR_SYN = 2,
    ERR_FUNDEF = 3,
    ERR_RUNPAR = 4,
    ERR_UNDEF = 5,
    ERR_EXPRES = 6,
    ERR_TYPE = 7,
    ERR_OTHER = 8,
    ERR_INTERN = 99
}ErrorType;
/**
 * 1  - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
 * 2  - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, chybějící hlavička, atp.).
 * 3  - sémantická chyba v programu – nedefinovaná funkce, pokus o redefinice funkce.
 * 4  - sémantická/běhová chyba v programu – špatný počet/typ parametrů u volání funkce či typ návratové hodnoty z funkce.
 * 5  - sémantická chyba v programu – použití nedefinované proměnné.
 * 6  - sémantická/běhová chyba v programu – chybějící/přebývající výraz v příkazu návratu z funkce.
 * 7  - sémantická/běhová chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech.
 * 8  - ostatní sémantické chyby.
 * 99 - interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti atd.).
 */
#endif